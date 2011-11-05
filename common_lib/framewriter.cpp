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


}
