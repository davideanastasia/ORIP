/* ORIP_v2 �> Op Ref Im Proc -> Oper Refin Imag Process -> Operational Refinement of Image Processing
 *
 * Copyright Davide Anastasia and Yiannis Andreopoulos, University College London
 *
 * http://www.ee.ucl.ac.uk/~iandreop/ORIP.html
 * If you have not retrieved this source code via the above link, please contact danastas@ee.ucl.ac.uk and iandreop@ee.ucl.ac.uk
 * 
 */


#define   _CRT_SECURE_NO_DEPRECATE

#include <iostream>

#include "matrix.h"
#include "convolution.h"
#include "framereader.h"
#include "framewriter.h"

template <class T>
void set_input(ORIP::Matrix<T>& M)
{
    for (int i=0; i<M.get_rows(); i++)
        for (int j=0; j<M.get_cols(); j++)
            M(i,j) = static_cast<T>(j);
}

template <class T>
void set_kernel(ORIP::Matrix<T>& M)
{
    M(0,0) = 1;
    M(0,1) = 1;
    M(0,2) = 1;

    M(1,0) = 1;
    M(1,1) = 1;
    M(1,2) = 1;

    M(2,0) = 1;
    M(2,1) = 1;
    M(2,2) = 1;
}

template <>
void set_kernel(ORIP::Matrix<float>& M)
{
    M(0,0) = 0.5f;
    M(0,1) = 0.5f;
    M(0,2) = 0.5f;

    M(1,0) = 0.5f;
    M(1,1) = 0.5f;
    M(1,2) = 0.5f;

    M(2,0) = 0.5f;
    M(2,1) = 0.5f;
    M(2,2) = 0.5f;
}

/*
int main(int ac, char* av[])
{
    using std::cout;
    using std::endl;
  
    ORIP::Matrix<int> input(6, 6);
    set_input(input);
    cout << input;

    ORIP::Matrix<int> kernel(3, 3);
    set_kernel(kernel);
    cout << kernel;

    ORIP::Matrix<int> output(6, 6);

    ORIP::convolution(input, kernel, output);

    cout << output;
    cout << "Test Floating Point" << endl;

    ORIP::Matrix<float> inputf(6, 6);
    set_input(inputf);
    cout << inputf;

    ORIP::Matrix<float> kernelf(3, 3);
    set_kernel(kernelf);
    cout << kernelf;

    ORIP::Matrix<float> outputf(6, 6);

    ORIP::convolution(inputf, kernelf, outputf);

    cout << outputf;

    return 0;                                   // 0: ok!
}
*/



int main()
{
    std::cout << "test" << std::endl;

    ORIP::FrameReader* my_reader = ORIP::FrameReader::getReader(ORIP::I_YUV420);
    ORIP::FrameWriter* my_writer = ORIP::FrameWriter::getWriter(ORIP::O_YUV420);


    my_reader->open("../hall.yuv");
    my_writer->open("../test.yuv");

    ORIP::Matrix<char> my_matrix(my_reader->getFrameHeight(), my_reader->getFrameWidth());
    ORIP::Matrix<char> my_output(my_reader->getFrameHeight(), my_reader->getFrameWidth());
    ORIP::Matrix<char> kernel(3, 3);
    set_kernel(kernel);

    int idx = 0;
    while (ORIP::getY(*my_reader, my_matrix))
    {
        ORIP::convolution(my_matrix, kernel, my_output);

        my_writer->storeY(my_output.data());
        std::cout << "Frame " << idx++ << std::endl;
    }

    return 0;
}

