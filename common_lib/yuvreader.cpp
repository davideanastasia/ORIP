/*
* License
*
* Copyright 2011 Davide Anastasia <davideanastasia@users.sourceforge.net>
*/

#include "yuvreader.h"

namespace ORIP
{

YUVReader::YUVReader():
m_Width(352), m_Height(288)
{}

YUVReader::~YUVReader()
{
    close();
}

void YUVReader::open(const std::string& filename)
{
    // if there is a file open already associated with this object
    // I close the stream
    if ( m_InputFileStream.is_open() ) m_InputFileStream.close();

    m_InputFileStream.open(filename.c_str(), std::ios::in|std::ios::binary);
}

bool YUVReader::getY(char* Y)
{
    if ( !m_InputFileStream.is_open() ) return false;
    if ( !m_InputFileStream.good() ) return false;

    // I should remove this type conversion
    // read char(s) into supplied buffer Y (and hope the user did the right thing,
    // or everything will crash badly!)
    m_InputFileStream.read(Y, m_Width*m_Height);
    
    // Move GET cursor forward, skipping the U and V portion
    m_InputFileStream.seekg(m_Width*(m_Height >> 1), std::ios::cur);

    return m_InputFileStream.good();
}

bool YUVReader::getYUV420(char* Y, char* U, char* V)
{
    if ( !m_InputFileStream.is_open() ) return false;
    if ( !m_InputFileStream.good() ) return false;

    // I should remove this type conversion
    // read char(s) into supplied buffer Y (and hope the user did the right thing,
    // or everything will crash badly!)
    m_InputFileStream.read(Y, m_Width*m_Height);
    
    // Same applies for U and V
    m_InputFileStream.read(U, ((m_Width*m_Height) >> 2)); // a quarter of the Y frame
    m_InputFileStream.read(V, ((m_Width*m_Height) >> 2)); // a quarter of the Y frame

    return m_InputFileStream.good();
}

void YUVReader::close()
{
    // is it safe to call multiple time a close() function on the stream?
    m_InputFileStream.close();
}

void YUVReader::setFrameSize(int width, int height)
{
    m_Width = width;
    m_Height = height;
}

int YUVReader::getFrameWidth()
{
    return m_Width;
}

int YUVReader::getFrameHeight()
{
    return m_Height;
}


} // namespace ORIP
