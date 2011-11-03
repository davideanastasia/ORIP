/*
* License
*
* Copyright 2011 Davide Anastasia <davideanastasia@users.sourceforge.net>
*/

#ifndef ORIP_FRAMEREADER_H
#define ORIP_FRAMEREADER_H

#include <string>

namespace ORIP
{
enum ReaderMode {YUV420, V4L};

class FrameReader
{
public:
    FrameReader();
    virtual ~FrameReader();

    static FrameReader* getReader(ReaderMode);

    virtual void open(const std::string& filename) = 0;
    virtual bool getY(unsigned char* Y) = 0;
    virtual bool getYUV420(unsigned char* Y, unsigned char* U, unsigned char* V) = 0;
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

#endif // ORIP_FRAMEREADER_H