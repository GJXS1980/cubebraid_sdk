#pragma once

// --- 防御性定义 LOGGER_API 宏 ---
#ifndef LOGGER_API
    #if defined(_WIN32) || defined(_WIN64)
        #ifdef LOGGER_EXPORTS
            #define LOGGER_API __declspec(dllexport)
        #else
            #define LOGGER_API __declspec(dllimport)
        #endif
    #else
        #define LOGGER_API
    #endif
#endif

#include <string>
#include <fstream>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <sstream>
#include <type_traits>


namespace logger_sdk 
{
    // ==========================================
    // 基础类型 Traits 判断
    // ==========================================
    template <typename T>
    struct is_string 
    {
        static constexpr bool value = std::is_same<typename std::decay<T>::type, std::string>::value;
    };

    template <typename T>
    struct is_container 
    {
        private:
            template <typename C>
            static char test(typename C::const_iterator*);

            template <typename C>
            static int test(...);

        public:
            static constexpr bool value = sizeof(test<typename std::decay<T>::type>(nullptr)) == sizeof(char) 
                                        && !is_string<typename std::decay<T>::type>::value;
    };

    // ==========================================
    // SystemLogger 类定义
    // ==========================================
    class LOGGER_API SystemLogger 
    {
        public:
            // 获取单例实例
            static SystemLogger& instance(const std::string& name = "systemlogger");

            // 析构函数必须为 public，以允许 std::unique_ptr / std::default_delete 正常释放资源
            virtual ~SystemLogger();

            // 初始化日志文件
            void init(const std::string& logPath);

            // 基础日志输出接口
            void log(const std::string& msg);

            // 模板日志输出方法
            template<typename... Args>
            void log(const char* format, Args&&... args) 
            {
                log(formatMessage(format, std::forward<Args>(args)...));
            }

            // 紧急停止日志方法
            template<typename... Args>
            void emergencyStop(const char* reasonFormat, Args&&... args) 
            {
                log("紧急停止！原因：" + formatMessage(reasonFormat, std::forward<Args>(args)...));
            }

        private:
            // 构造函数保持 private，确保无法在外部被直接实例化
            SystemLogger() = default;
            
            SystemLogger(const SystemLogger&) = delete;
            SystemLogger& operator=(const SystemLogger&) = delete;

            // 获取当前格式化时间字符串
            std::string getCurrentTime();

            // 容器类型输出展开
            template<typename T>
            typename std::enable_if<is_container<T>::value>::type
            printValue(std::ostream& os, const T& container) 
            {
                os << "[";
                auto it = container.begin();
                while (it != container.end()) 
                {
                    os << *it;
                    if (++it != container.end()) os << ", ";
                }
                os << "]";
            }

            // 非容器标量类型输出展开
            template<typename T>
            typename std::enable_if<!is_container<T>::value>::type
            printValue(std::ostream& os, const T& value) 
            {
                os << value;
            }

            // 递归格式化解析实现（多参数）
            template<typename T, typename... Args>
            void formatRecursive(std::ostream& os, const char* format, T&& value, Args&&... args) 
            {
                for (; *format != '\0'; ++format) 
                {
                    if (*format == '%' && *(format + 1) != '%') 
                    {
                        printValue(os, std::forward<T>(value));
                        formatRecursive(os, format + 1, std::forward<Args>(args)...);
                        return;
                    } 
                    else if (*format == '%' && *(format + 1) == '%') 
                    {
                        os << '%';
                        ++format;
                    } 
                    else 
                    {
                        os << *format;
                    }
                }
            }

            // 递归格式化解析终止条件（无剩余参数）
            void formatRecursive(std::ostream& os, const char* format) 
            {
                for (; *format != '\0'; ++format) 
                {
                    if (*format == '%' && *(format + 1) == '%') 
                    {
                        ++format;
                        os << '%';
                    } 
                    else 
                    {
                        os << *format;
                    }
                }
            }

            // 消息格式化拼接函数
            template<typename... Args>
            std::string formatMessage(const char* format, Args&&... args) 
            {
                std::ostringstream oss;
                formatRecursive(oss, format, std::forward<Args>(args)...);
                return oss.str();
            }

        private:
            std::ofstream logFile;
            std::mutex logMutex;
    };

} // namespace logger_sdk

// ==========================================
// 用于 Python (ctypes/ffi) 调用的 C API 导出声明
// ==========================================
#ifdef __cplusplus
extern "C" {
#endif

    /**
     * @brief 初始化指定名称的日志组件
     * @param instance_name 单例实例名（传入 NULL 或空串默认使用 "systemlogger"）
     * @param log_path 日志输出文件路径
     */
    LOGGER_API void logger_init(const char* instance_name, const char* log_path);

    /**
     * @brief 写入一条日志
     * @param instance_name 单例实例名
     * @param msg 日志内容
     */
    LOGGER_API void logger_log(const char* instance_name, const char* msg);

    /**
     * @brief 写入紧急停止日志
     * @param instance_name 单例实例名
     * @param reason 紧急停止原因
     */
    LOGGER_API void logger_emergency_stop(const char* instance_name, const char* reason);

#ifdef __cplusplus
}
#endif