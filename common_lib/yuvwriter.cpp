/*
* License
*
* Copyright 2011 Davide Anastasia <davideanastasia@users.sourceforge.net>
*/

#include "yuvwriter.h"

namespace ORIP
{

YUVWriter::YUVWriter():
m_Width(352), m_Height(288)
{}

YUVWriter::~YUVWriter()
{
    close();
}

void YUVWriter::open(const std::string& filename)
{
    // if there is a file open already associated with this object
    // I close the stream
    if ( m_OutputFileStream.is_open() ) m_OutputFileStream.close();

    m_OutputFileStream.open(filename.c_str(), std::ios::out|std::ios::binary);
}

void YUVWriter::close()
{
    // is it safe to call multiple time a close() function on the stream?
    m_OutputFileStream.close();
}

void YUVWriter::setFrameSize(int width, int height)
{
    m_Width = width;
    m_Height = height;
}

int YUVWriter::getFrameWidth()
{
    return m_Width;
}

int YUVWriter::getFrameHeight()
{
    return m_Height;
}

bool YUVWriter::storeY(char* Y)
{
    if ( !m_OutputFileStream.is_open() ) return false;
    if ( !m_OutputFileStream.good() ) return false;

    // I should remove this type conversion
    // read char(s) into supplied buffer Y (and hope the user did the right thing,
    // or everything will crash badly!)
    m_OutputFileStream.write(static_cast<const char*>(Y), m_Width*m_Height);

    // Move GET cursor forward, skipping the U and V portion
    char filler = 0;
    int idx = m_Width*(m_Height >> 1);
    while ( idx-- )
        m_OutputFileStream.put(filler);

    return m_OutputFileStream.good();
}

bool YUVWriter::storeYUV420(char* Y, char* U, char* V)
{
    if ( !m_OutputFileStream.is_open() ) return false;
    if ( !m_OutputFileStream.good() ) return false;

    // I should remove this type conversion
    // read char(s) into supplied buffer Y (and hope the user did the right thing,
    // or everything will crash badly!)

    m_OutputFileStream.write(static_cast<const char*>(Y), m_Width*m_Height);

    // Same applies for U and V

    m_OutputFileStream.write(static_cast<const char*>(U), ((m_Width*m_Height) >> 2)); // a quarter of the Y frame
    m_OutputFileStream.write(static_cast<const char*>(V), ((m_Width*m_Height) >> 2)); // a quarter of the Y frame

    return m_OutputFileStream.good();
}



} // namespace ORIP
