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

#define LIMIT           50.0
#define MAX_PACKING     51.0
#define exp_error(v)    ( pow(2.0, -(MAX_PACKING))*v / pow(2.0, (-(__num_pack-1)*__M)) ) 

typedef long my_t;
typedef my_t** matrix;

// -- packing ---
extern int    __max_value;
extern int    __M;
extern double __eps;
extern int    __inv_eps;
extern int    __num_pack;
// --------------

void show_env_status();

void setup_env(matrix coeff_matrix, int size, int bit4bitplane = 1);
void setup_env(int size);

void setup_block_matrix(matrix input, int size, int max_value);
void setup_block_avc(matrix input);
void setup_block_bin_dct(matrix input);
void setup_block_frext(matrix input);

void block_multiply(double** img, matrix block, int blk_row, int blk_col, double** result);
void block_multiply_t(double** img, matrix block, int rows, int cols, double** result);

void pack_matrix(matrix* input_b, int elem, double** input_p, int width, int height);

void read_block(matrix img, int img_w, int img_h, matrix block, int blk_w, int blk_h, int n_blk);
void write_block(matrix block, int blk_w, int blk_h, matrix img, int img_w, int img_h, int n_blk, int bitplane, int bits);

void unpack_matrix_and_store_i(double** input, matrix* output, int elem, int width, int height /*, int bitplane, int bits = 1*/ );
void unpack_matrix_and_store(double** input, matrix* output, int elem, int width, int height, int bitplane);
#endif // __LIBRARY_H_

