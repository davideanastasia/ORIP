/*
* License
*
* Copyright 2011 Davide Anastasia <davideanastasia@users.sourceforge.net>
*/

#include "framereader.h"
#include "yuvreader.h"

namespace ORIP
{
FrameReader::FrameReader()
{}

FrameReader::~FrameReader()
{}

FrameReader* FrameReader::getReader(ReaderMode _mode)
{
    switch (_mode)
    {
    case I_V4L:
        {
            return 0;
        }
    case I_YUV420:
    default:
        {
            return (new YUVReader);
        }
        break;
    }

}

template<>
bool getY<char>(FrameReader& reader, Matrix<char>& frame)
{
    /*
     * I know, assert is not the best way to deal with this kind of error,
     * but it can be definitely improved in the future
     */
    assert(reader.getFrameWidth() == frame.get_cols());
    assert(reader.getFrameHeight() == frame.get_rows());

    if ( reader.getY(frame.data()) )
        return true;
    else
        return false;
}

}
