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

typedef long my_t;
typedef my_t** matrix;

struct match
{
  int src_x;
  int src_y;
  int dest_x;
  int dest_y;
  int m_value;
};

match** create_matrix(int col, int row);
void print_matrix(match** matrix, int c_size, int r_size, int print_col, int print_row);

//void read_search_area(matrix input, int w_in, int h_in, int x, int y, matrix output, int w_out, int h_out, int search_range);
//void read_block(matrix input, int width, int height, int x, int y, matrix output, int width_b, int height_b);
void write_block(matrix input, int width_b, int height_b, int x, int y, matrix output, int width, int height);

//void block_match(matrix input, int width, int height, matrix block_ref, int width_b, int height_b, int x, int y, int range, matrix output);
void pick_best_and_store(matrix input, match& output, int x, int y, int range);

void block_match(matrix search_area, int w_s_area, int h_s_area,
                 matrix block, int w_blk, int h_blk,
                 int x, int y, int w_img, int h_img, int search_range, matrix output);

void block_match_v2(matrix search_area, int w_s_area, int h_s_area,
                           matrix block, int w_blk, int h_blk,
                           int x, int y, int w_img, int h_img, int search_range, matrix output);

void block_match_and_store(matrix search_area, int w_s_area, int h_s_area,
                 matrix block, int w_blk, int h_blk,
                 int x, int y, int w_img, int h_img, int search_range,
                 match& output);

void block_match_and_store_v2(matrix search_area, int w_s_area, int h_s_area,
                           matrix block, int w_blk, int h_blk,
                           int x, int y, int w_img, int h_img, int search_range,
                           match& output);

void block_match_log_search_and_store(matrix search_area, int w_s_area, int h_s_area,
                 matrix block, int w_blk, int h_blk,
                 int x, int y, int w_img, int h_img, int search_range,
                 match& output);

void block_match_i_spiral(matrix ref_area, int w_ref_area, int h_ref_area,
                          matrix block, int w_blk, int h_blk,
                          int x, int y, int search_range, match& output, int spiral_size);


/* b_idx = block index */
template <class T>
void read_block(T** input, int width, int height, int x, int y, T** output, int width_b, int height_b)
{
  // min border
  int idx_y=(y<0)?(0):(y);
  int idx_x=(x<0)?(0):(x);

  // max border
  idx_y = (idx_y+height_b>=height)?(height-height_b):idx_y;
  idx_x = (idx_x+width_b>=width)?(width-width_b):idx_x;

  for (int i = 0; i < height_b; i++)
    for (int j = 0; j < width_b; j++)
      output[i][j] = input[idx_y + i][idx_x + j];
}

/* b_idx = block index */
template <class T>
void read_search_area(T** input, int w_in, int h_in, int x, int y, T** output, int w_out, int h_out, int search_range)
{
  for (int i = 0; i < h_out; i++) // row
  {
    for (int j = 0; j < w_out; j++) // col
    {
      int idx_y = y + i - search_range;
      int idx_x = x + j - search_range;

      // check Y
      idx_y = (idx_y < 0) ? (0) : (idx_y); // zero border
      idx_y = (idx_y >= h_in) ? (h_in - 1) : (idx_y);

      // check X
      idx_x = (idx_x < 0) ? (0) : (idx_x); // zero border
      idx_x = (idx_x >= w_in) ? (w_in - 1) : (idx_x);

      output[i][j] = input[idx_y][idx_x];
    }
  }
}

#endif // __LIBRARY_H_

