/*
* License
*
* Copyright 2011 Davide Anastasia <davideanastasia@users.sourceforge.net>
*/

#ifndef FRAMEWRITER_H
#define FRAMEWRITER_H

#include <cassert>
#include <string>
#include "matrix.h"

namespace ORIP
{
enum WriterMode {O_YUV420};

class FrameWriter
{
public:
    FrameWriter();
    virtual ~FrameWriter();

    static FrameWriter* getWriter(WriterMode);

    virtual void open(const std::string& filename) = 0;
    virtual bool storeY(char* Y) = 0;
    virtual bool storeYUV420(char* Y, char* U, char* V) = 0;
    virtual void close() = 0;

    /*
     * Define the frame size: depending on the type of input, the concrete FrameReader
     * can accept of not the request
     */
    virtual void setFrameSize(int width, int height) = 0;
    virtual int getFrameWidth() = 0;
    virtual int getFrameHeight() = 0;

};

}

#endif // FRAMEWRITER_H
