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

#define LIMIT           ((sizeof(my_t)*8) - 1)

typedef long my_t;
typedef my_t** matrix;

// -- packing ---
extern my_t    __max_value;
extern long    __M;                   // 12 ==> __max value is 2^11 = 2048
extern my_t    __inv_eps;                 // 2^(12)
extern long    __num_pack;                    // (12 + )12 + 12 + 12 + 12 = 48 // it should be 5, but 4 is easier for me
extern my_t    __mask;
// --------------

void show_env_status();

void setup_env(matrix coeff_matrix, int size, int bit4bitplane = 1);
void setup_env(int size);

void setup_block_matrix(matrix input, int size, int max_value);
void setup_block_avc(matrix input);
void setup_block_bin_dct(matrix input);
void setup_block_frext(matrix input);

void block_multiply(matrix img, matrix block, int blk_row, int blk_col, matrix result);
void block_multiply_t(matrix img, matrix block, int rows, int cols, matrix result);

void pack_matrix(matrix* input_b, int elem, matrix output_p, int width, int height);

void read_block(matrix img, int img_w, int img_h, matrix block, int blk_w, int blk_h, int n_blk);
void write_block(matrix block, int blk_w, int blk_h, matrix img, int img_w, int img_h, int n_blk, int bitplane, int bits);

void unpack_matrix_and_store_i(matrix input, matrix* output, int elem, int width, int height);
void unpack_matrix_and_store(matrix input, matrix* output, int elem, int width, int height, int bitplane);
#endif // __LIBRARY_H_

