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
    case V4L:
        {
            return (new YUVReader);
        }
    case YUV420:
    default:
        {
            return 0;
        }
        break;
    }

}

}