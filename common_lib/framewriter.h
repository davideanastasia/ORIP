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

template<typename Type>
bool storeFrame(FrameWriter& writer, Matrix<Type>& frame)
{
    /*
     * I know, assert is not the best way to deal with this kind of error,
     * but it can be definitely improved in the future
     */
    assert(writer.getFrameWidth() == frame.get_cols());
    assert(writer.getFrameHeight() == frame.get_rows());

    /*
     * I allocate a new buffer, but I would love not too!
     */
    char* _temp_buffer = new char[frame.get_elems()];

    for (int idx = 0; idx < frame.get_elems(); idx++)
        _temp_buffer[idx] = static_cast<char>(frame(idx));

    if ( writer.storeY(_temp_buffer) )
    {
        delete [] _temp_buffer;
        return true;
    } else {
        delete [] _temp_buffer;
        return false;
    }
}

/*
  * Template specialization
  */
template<>
bool storeFrame(FrameWriter& writer, Matrix<char>& frame);

}

#endif // FRAMEWRITER_H
