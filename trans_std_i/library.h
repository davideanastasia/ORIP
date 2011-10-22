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

#define M         256				          // 2^8
#define EPS       0.001953125				  // 2^(-9)
#define INV_EPS   512			              // 2^(15)

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "common_lib.h"

using namespace std;

typedef long my_t;
typedef my_t** matrix;

void setup_block_matrix(matrix input, int size, int max_value);
void setup_block_avc(matrix input);
void setup_block_avc_inv(matrix input);
void setup_block_bin_dct(matrix input);
void setup_block_frext(matrix input);

void block_multiply(matrix img, matrix block, int rows, int cols, matrix result);
void block_multiply_t(matrix img, matrix block, int rows, int cols, matrix result);

void write_block(matrix block, int blk_w, int blk_h, matrix img, int img_w, int img_h, int n_blk);
void read_block(matrix img, int img_w, int img_h, matrix block, int blk_w, int blk_h, int n_blk);

#endif // __LIBRARY_H_

