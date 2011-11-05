/*
* License
*
* Copyright 2011 Davide Anastasia <davideanastasia@users.sourceforge.net>
*/

#include "framewriter.h"
#include "yuvwriter.h"

namespace ORIP
{

FrameWriter::FrameWriter()
{}

FrameWriter::~FrameWriter()
{}

FrameWriter* FrameWriter::getWriter(WriterMode _mode)
{
    switch (_mode)
    {
    case O_YUV420:
    default:
        {
            return (new YUVWriter);
        }
        break;
    }

}

template<>
bool storeFrame(FrameWriter& writer, Matrix<char>& frame)
{
    /*
     * I know, assert is not the best way to deal with this kind of error,
     * but it can be definitely improved in the future
     */
    assert(writer.getFrameWidth() == frame.get_cols());
    assert(writer.getFrameHeight() == frame.get_rows());

    return writer.storeY(frame.data());
}

}
