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
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "common_lib.h"

using namespace std;

typedef double my_t;
typedef my_t** matrix;

void setup_conv_matrix(matrix input, int col, int row, int max_val);
void setup_conv_gaussian_12x12(matrix input);
void setup_conv_gaussian_18x18(matrix input);
void setup_conv_gaussian_6x6(matrix input);

void conv2(matrix mat1, int rows1, int cols1, matrix mat2, int rows2, int cols2, matrix res, int n_shift = 0);
void conv2_symmetric(matrix mat1, int rows1, int cols1, matrix mat2, int rows2, int cols2, matrix res);

#endif // __LIBRARY_H_

