/*
* License
*
* Copyright 2011 Davide Anastasia <davideanastasia@users.sourceforge.net>
*/

#ifndef ORIP_YUVREADER_H
#define ORIP_YUVREADER_H

#include <iostream>
#include <fstream>
#include <string>
#include "framewriter.h"

namespace ORIP
{

class YUVWriter : public FrameWriter
{
public:
    /*
     * ctor
     */ 
    YUVWriter();
    /*
     * virtual dtor
     */ 
    virtual ~YUVWriter();

    /*
     * open an YUV video file
     */
    void open(const std::string& filename);

    /*
     * get only the Y part of the YUV frame
     * return if the status of the read operation
     */ 
    bool storeY(char* Y);

    /*
     * get the YUV frame in planar format
     * return if the status of the read operation
     */
    bool storeYUV420(char* Y, char* U, char* V);

    /*
     * close the YUV video file
     */
    void close();

    /*
     * Set frame dimension (CIF 352x288 is set by default)
     */
    void setFrameSize(int width, int height);

    /*
     * Return current Width;
     */
    int getFrameWidth();

    /*
     * Return current Height;
     */
    int getFrameHeight();

private:
    int m_Width;
    int m_Height;

    std::ofstream m_OutputFileStream;
};

}

#endif // ORIP_FRAMEREADER_H
