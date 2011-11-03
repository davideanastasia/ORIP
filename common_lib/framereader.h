/*
* License
*
* Copyright 2011 Davide Anastasia <davideanastasia@users.sourceforge.net>
*/

#ifndef ORIP_FRAMEREADER_H
#define ORIP_FRAMEREADER_H

#include <cassert>
#include <string>
#include "matrix.h"

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

template<typename _T>
bool getY(FrameReader& reader, Matrix<_T>& frame)
{
    /*
     * I know, assert is not the best way to deal with this kind of error,
     * but it can be definitely improved in the future
     */
    assert(reader.getFrameWidth() == frame.get_cols());
    assert(reader.getFrameHeight() == frame.get_rows());

    /*
     * I allocate a new buffer, but I would love not too!
     * I can use the Matrix buffer as a temporary one using the member Matrix<T>::data()
     */ 
    unsigned char* _temp_buffer = new unsigned char[frame.get_elems()];

    if ( reader.getY(_temp_buffer) ) {
        /*
        * If everything is fine, I have my data ready to be stored in my frame
        */
        for (int idx = 0; idx < frame.get_elems(); idx++)
            frame(idx) = static_cast<_T>(_temp_bufferp[idx]);

        return true;
    }
    else
        return false;
}

}

#endif // ORIP_FRAMEREADER_H