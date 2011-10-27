#ifndef ORIP_CONVOLUTION
#define ORIP_CONVOLUTION

#include <cassert>
#include <cmath>

#include "matrix.h"

//#define CONV_DEBUG

namespace ORIP
{

template <typename _M, typename _K>
void convolution(const Matrix<_M>& in_mat, const Matrix<_K>& kernel_mat, Matrix<_M>& out_mat)
{
    assert( in_mat.get_cols() == out_mat.get_cols() );
    assert( in_mat.get_rows() == out_mat.get_rows() );

    const int offset_rows = (kernel_mat.get_rows() - 1)/2; //(( rows2%2 == 0 ) ? (rows2/2-1) : (rows2/2));
    const int offset_cols = (kernel_mat.get_cols() - 1)/2; //(( cols2%2 == 0 ) ? (cols2/2-1) : (cols2/2));
#ifdef CONV_DEBUG
    std::cerr << "(" << offset_rows << "," << offset_cols << ")" << std::endl;
#endif

    _M temp_item;

    for (int i = 0; i < in_mat.get_rows(); i++)
    {
        for (int j = 0; j < in_mat.get_cols(); j++)
        {
            temp_item = 0; // reset
#ifdef CONV_DEBUG
            std::cerr << "(" << i << "," << j << ")" << std::endl;
            std::cerr << "ik (" << ((i-offset_rows < 0)? (offset_rows-i): 0) << "," << ((i+offset_rows >= in_mat.get_rows())? (kernel_mat.get_rows() - (in_mat.get_rows()+1-i-offset_rows)): kernel_mat.get_rows()) << ")" << std::endl;
#endif

            for (int ik = ((i-offset_rows < 0)? (offset_rows-i): 0);
                     ik < ((i+offset_rows >= in_mat.get_rows())? (kernel_mat.get_rows() - (in_mat.get_rows()+1-i-offset_rows)): kernel_mat.get_rows());
                     ik++)
            {
#ifdef CONV_DEBUG
            std::cerr << "jk (" << ((j-offset_cols < 0)? (offset_cols-j): 0) << "," << ((j+offset_cols >= in_mat.get_cols())? (kernel_mat.get_cols() - (in_mat.get_cols()+1-j-offset_cols)): kernel_mat.get_cols()) << ")" << std::endl;
#endif
                for (int jk = ((j-offset_cols < 0)? (offset_cols-j): 0);
                         jk < ((j+offset_cols >= in_mat.get_cols())? (kernel_mat.get_cols() - (in_mat.get_cols()+1-j-offset_cols)): kernel_mat.get_cols());
                         jk++)
                {
                    temp_item += ( in_mat(i + ik - offset_rows, j + jk - offset_cols) * kernel_mat(ik, jk) );
                }
            }
            out_mat(i, j) = temp_item; // set final result
        }
    }
}
}

#endif