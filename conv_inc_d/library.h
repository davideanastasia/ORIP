/* ORIP_v2 –> Op Ref Im Proc -> Oper Refin Imag Process -> Operational Refinement of Image Processing
 *
 * Copyright Davide Anastasia and Yiannis Andreopoulos, University College London
 *
 * http://www.ee.ucl.ac.uk/~iandreop/ORIP.html
 * If you have not retrieved this source code via the above link, please contact danastas@ee.ucl.ac.uk and iandreop@ee.ucl.ac.uk
 * 
 */


#ifndef __LIBRARY_H_
#define __LIBRARY_H_

#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include "common_lib.h"

using namespace std;

#define LIMIT           (52.0)
#define MAX_PACKING     (LIMIT)  // this value must be estimated
#define exp_error(v)    ( pow(2.0, -(LIMIT))*v / pow(2.0, (-(__num_pack-1)*__M)) ) 

//typedef double  double;
typedef long    my_t;
typedef my_t** matrix;

// -- packing ---
extern long __max_value;
extern int __M;
extern double __eps;
extern int __inv_eps;
extern int __num_pack;
// --------------

void show_env_status();
void setup_env(matrix coeff_matrix, int _col, int _row, int bit4bitplane = 1);

void setup_conv_matrix(matrix input, int col, int row, int max_value = 255);
void setup_conv_gaussian_12x12(matrix input);
void setup_conv_gaussian_18x18(matrix input);
void setup_conv_gaussian_6x6(matrix input);

void conv2(double** mat1, int rows1, int cols1, matrix mat2, int rows2, int cols2, double** res, int n_shift = 0);
void conv2_symmetric(double** mat1, int rows1, int cols1, matrix mat2, int rows2, int cols2, double** res);

void conv2_unpack_store_sym(double** in_packed, int rows1, int cols1, matrix kernel, int rows2, int cols2, // double** res)
                            matrix output, int o_width, int o_height, int elem, int bitplane, int bits);
void conv2_unpack_store(double** in_packed, int rows1, int cols1, matrix kernel, int rows2, int cols2, // double** res)
                            matrix output, int o_width, int o_height, int elem, int bitplane, int bits);

void pack_matrix(matrix input, int i_width, int i_height, double** output, int o_width, int o_height, int elem);
void unpack_matrix_and_store_i(double** input, int i_width, int i_height, matrix output, int o_width, int o_height, int elem, int bitplane, int bits = 1);

#endif // __LIBRARY_H_

