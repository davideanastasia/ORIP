#ifndef ORIP_CONVOLUTION
#define ORIP_CONVOLUTION

#include <cassert>
#include <cmath>

#include "matrix.h"

//#define CONV_DEBUG

namespace ORIP
{

template <typename TypeIO, typename TypeK>
void convolution(const Matrix<TypeIO>& in_mat, const Matrix<TypeK>& kernel_mat, Matrix<TypeIO>& out_mat)
{
    assert( in_mat.get_cols() == out_mat.get_cols() );
    assert( in_mat.get_rows() == out_mat.get_rows() );

    const int offset_rows = (kernel_mat.get_rows() - 1)/2; //(( rows2%2 == 0 ) ? (rows2/2-1) : (rows2/2));
    const int offset_cols = (kernel_mat.get_cols() - 1)/2; //(( cols2%2 == 0 ) ? (cols2/2-1) : (cols2/2));
#ifdef CONV_DEBUG
    std::cerr << "offsets: (" << offset_rows << "," << offset_cols << ")" << std::endl;
#endif

    TypeIO temp_item;

    for (int i = 0; i < in_mat.get_rows(); i++)
    {
        for (int j = 0; j < in_mat.get_cols(); j++)
        {
            temp_item = 0; // reset
#ifdef CONV_DEBUG
            std::cerr << " pos: (" << i << "," << j << ")";
            std::cerr << " ik: (" << ((i-offset_rows < 0)? (offset_rows-i): 0) << "," << ((i+offset_rows >= in_mat.get_rows())? (kernel_mat.get_rows() - (in_mat.get_rows()+1-i-offset_rows)): kernel_mat.get_rows()) << ")";
#endif

            for (int ik = ((i-offset_rows < 0)? (offset_rows-i): 0);
                     ik < ((i+offset_rows >= in_mat.get_rows())? (kernel_mat.get_rows() - (in_mat.get_rows()+1-i-offset_rows)): kernel_mat.get_rows());
                     ik++)
            {
#ifdef CONV_DEBUG
            std::cerr << " jk: (" << ((j-offset_cols < 0)? (offset_cols-j): 0) << "," << ((j+offset_cols >= in_mat.get_cols())? (kernel_mat.get_cols() - (in_mat.get_cols()+1-j-offset_cols)): kernel_mat.get_cols()) << ")";
#endif
                for (int jk = ((j-offset_cols < 0)? (offset_cols-j): 0);
                         jk < ((j+offset_cols >= in_mat.get_cols())? (kernel_mat.get_cols() - (in_mat.get_cols()+1-j-offset_cols)): kernel_mat.get_cols());
                         jk++)
                {
                    temp_item += ( in_mat(i + ik - offset_rows, j + jk - offset_cols) * kernel_mat(ik, jk) );
                }
            }
            out_mat(i, j) = temp_item; // set final result

#ifdef CONV_DEBUG
            std::cerr << std::endl;
#endif
        }
    }
}

}

#endif