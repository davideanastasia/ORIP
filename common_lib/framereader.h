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
enum ReaderMode {I_YUV420, I_V4L};

class FrameReader
{
public:
    FrameReader();
    virtual ~FrameReader();

    static FrameReader* getReader(ReaderMode);

    virtual void open(const std::string& filename) = 0;
    virtual bool getY(char* Y) = 0;
    virtual bool getYUV420(char* Y, char* U, char* V) = 0;
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
bool loadFrame(FrameReader& reader, Matrix<Type>& frame)
{
    /*
     * I know, assert is not the best way to deal with this kind of error,
     * but it can be definitely improved in the future
     */
    assert(reader.getFrameWidth() == frame.get_cols());
    assert(reader.getFrameHeight() == frame.get_rows());

    /*
     * I allocate a new buffer, but I would love not too!
     */ 
    char* _temp_buffer = new char[frame.get_elems()];

    if ( reader.getY(_temp_buffer) ) {
        /*
        * If everything is fine, I have my data ready to be stored in my frame
        * I copy the data backwards
        */
        for (int idx = 0; idx < frame.get_elems(); idx++)
            frame(idx) = static_cast<Type>(_temp_buffer[idx]);

        delete [] _temp_buffer;
        return true;
    }
    else
    {
        delete [] _temp_buffer;
        return false;
    }
}

/*
 * Specialization for Matrix<char>
 */
template<>
bool loadFrame<char>(FrameReader& reader, Matrix<char>& frame);


}

#endif // ORIP_FRAMEREADER_H
