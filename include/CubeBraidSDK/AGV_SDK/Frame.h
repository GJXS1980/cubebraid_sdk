/*
 * Copyright 2022 The Picasso Group Authors
 * 2022-04-22 chaosir <chaosir@picasso-robot.com>
 */
#ifndef _PLNS_FRAME_H
#define _PLNS_FRAME_H

#include <memory>
#include "CubeBraidSDK/AGV_SDK/main.pb.h"

namespace proto 
{

typedef std::shared_ptr<Message> Message_ptr;

/**
 * |-------------------------------|
 * | FRAME_SIZE  |       DATA      |
 * |-------------------------------|
 * |             |    FRAME_SIZE   |
 * |     HEADER  |       BODY      |
 *
 */
class Frame;

typedef std::shared_ptr<Frame> Frame_ptr;

class Frame
{
public:

    static const int FRAME_HEADER_LEN = 4;

    static const int MAX_SIZE = 128 * 1024;

    uint8_t data[MAX_SIZE];

    size_t total_size;

    uint8_t* get_frame_data() 
    {
        return data + FRAME_HEADER_LEN;
    }

    void clear();

    static size_t getFrameBodySize(const Frame &frame);

    static Message_ptr buildMessage(Frame &frame);

    static Frame_ptr buildFrame(Message_ptr);
};

}

#endif // _PLNS_FRAME_H