/* ORIP_v2 –> Op Ref Im Proc -> Oper Refin Imag Process -> Operational Refinement of Image Processing
 *
 * Copyright Davide Anastasia and Yiannis Andreopoulos, University College London
 *
 * http://www.ee.ucl.ac.uk/~iandreop/ORIP.html
 * If you have not retrieved this source code via the above link, please contact danastas@ee.ucl.ac.uk and iandreop@ee.ucl.ac.uk
 * 
 */


// [row][col]

#include "common_lib.h"
#include "library.h"

using namespace std;

/*
  img = input image matrix
  img_w = image width
  img_h = image height
  block = output block matrix
  blk_w = block width  
  blk_h = block height
  n_blk = block to extract (from 0 to n, linear order)
*/
void read_block(matrix img, int img_w, int img_h, matrix block, int blk_w, int blk_h, int n_blk)
{
  static int w_block = img_w / blk_w;
  static int h_block = img_h / blk_h;

  int h_coord = n_blk / w_block;
  int w_coord = n_blk % w_block;//n_blk - (h_coord * w_block);

  int h_offset = h_coord * blk_h;
  int w_offset = w_coord * blk_w;

  for (int i = 0; i < blk_h; i++)
    for (int j = 0; j < blk_w; j++)
      block[i][j] = img[h_offset + i][w_offset + j];
}

/*
  img = input image matrix
  img_w = image width
  img_h = image height
  block = output block matrix
  blk_w = block width  
  blk_h = block height
  n_blk = block to extract (from 0 to n, linear order)
*/
void write_block(matrix block, int blk_w, int blk_h, matrix img, int img_w, int img_h, int n_blk)
{
  static int w_block = img_w / blk_w;
  static int h_block = img_h / blk_h;

  int h_coord = n_blk / w_block;
  int w_coord = n_blk % w_block;//n_blk - (h_coord * w_block);

  int h_offset = h_coord * blk_h;
  int w_offset = w_coord * blk_w;

  for (int i = 0; i < blk_h; i++)
    for (int j = 0; j < blk_w; j++)
      img[h_offset + i][w_offset + j] = block[i][j];
}

void block_multiply(matrix img, matrix block, int rows, int cols, matrix result)
{

  // -- multiplication
  for (int i = 0; i < rows; i++) 
    for (int j = 0; j < cols; j++)
      for (int k = 0; k < rows; k++)
      {
        // this function works only if rows == cols (2x2, 3x3, 4x4, ...)
        // result[v_idx + i][h_idx + j] +=  block[i][k] * img[v_idx + k][h_idx + j];
        result[i][j] +=  block[i][k] * img[k][j];
      }
      // ----

}

void block_multiply_t(matrix img, matrix block, int rows, int cols, matrix result)
{

  // -- multiplication
  for (int i = 0; i < rows; i++) 
    for (int j = 0; j < cols; j++)
      for (int k = 0; k < rows; k++)
      {
        // this function works only if rows == cols (2x2, 3x3, 4x4, ...)
        // result[v_idx + j][h_idx + i] +=  block[i][k] * img[v_idx + j][h_idx + k];
        result[j][i] +=  block[i][k] * img[j][k];
      }
      // ----

}

void setup_block_matrix(matrix input, int size, int max_value)
{
  srand ( 0 );

  for (int i = 0; i < size; i++)
  {
    for ( int j = 0; j < size; j++)
    {
      input[i][j] = (rand() % max_value); // 0 ... 15
    }
  }
}

void setup_block_avc(matrix input)
{
  // it should be 4x4
  input[0][0] = 1;  // row col
  input[0][1] = 1;
  input[0][2] = 1;
  input[0][3] = 1;

  input[1][0] = 2;  // row col
  input[1][1] = 1;
  input[1][2] = -1;
  input[1][3] = -2;

  input[2][0] = 1;  // row col
  input[2][1] = -1;
  input[2][2] = -1;
  input[2][3] = 1;

  input[3][0] = 1;  // row col
  input[3][1] = -2;
  input[3][2] = 2;
  input[3][3] = -1;
}

void setup_block_avc_inv(matrix input)
{
  // it should be 4x4
  input[0][0] = 5;  // row col
  input[0][1] = 4;
  input[0][2] = 5;
  input[0][3] = 2;

  input[1][0] = 5;  // row col
  input[1][1] = 2;
  input[1][2] = -5;
  input[1][3] = -4;

  input[2][0] = 5;  // row col
  input[2][1] = -2;
  input[2][2] = -5;
  input[2][3] = 4;

  input[3][0] = 5;  // row col
  input[3][1] = -4;
  input[3][2] = 5;
  input[3][3] = -2;
}

void setup_block_bin_dct(matrix input)
{
  // it should be 8x8
  input[0][0] = 64;  // row col
  input[0][1] = 64;
  input[0][2] = 64;
  input[0][3] = 64;
  input[0][4] = 64;
  input[0][5] = 64;
  input[0][6] = 64;
  input[0][7] = 64;

  input[1][0] = 64;  // row col
  input[1][1] = 64;
  input[1][2] = 24;
  input[1][3] = 0;
  input[1][4] = 0;
  input[1][5] = -24;
  input[1][6] = -64;
  input[1][7] = -64;

  input[2][0] = 55;  // row col
  input[2][1] = 24;
  input[2][2] = -24;
  input[2][3] = -55;
  input[2][4] = -55;
  input[2][5] = -24;
  input[2][6] = 24;
  input[2][7] = 55;

  input[3][0] = 36;  // row col
  input[3][1] = -16;
  input[3][2] = -38;
  input[3][3] = -32;
  input[3][4] = 32;
  input[3][5] = 38;
  input[3][6] = 16;
  input[3][7] = -36;

  input[4][0] = 32;  // row col
  input[4][1] = -32;
  input[4][2] = -32;
  input[4][3] = 32;
  input[4][4] = 32;
  input[4][5] = -32;
  input[4][6] = -32;
  input[4][7] = 32;

  input[5][0] = 56;  // row col
  input[5][1] = -96;
  input[5][2] = 28;
  input[5][3] = 64;
  input[5][4] = -64;
  input[5][5] = -28;
  input[5][6] = 96;
  input[5][7] = -56;

  input[6][0] = -24;  // row col
  input[6][1] = 64;
  input[6][2] = -64;
  input[6][3] = 24;
  input[6][4] = 24;
  input[6][5] = -64;
  input[6][6] = 64;
  input[6][7] = -24;

  input[7][0] = -8;  // row col
  input[7][1] = 32;
  input[7][2] = -52;
  input[7][3] = 64;
  input[7][4] = -64;
  input[7][5] = 52;
  input[7][6] = -32;
  input[7][7] = 8;
}

void setup_block_frext(matrix input)
{
  // it should be 8x8
  input[0][0] = 8;  // row col
  input[0][1] = 8;
  input[0][2] = 8;
  input[0][3] = 8;
  input[0][4] = 8;
  input[0][5] = 8;
  input[0][6] = 8;
  input[0][7] = 8;

  input[1][0] = 12;  // row col
  input[1][1] = 10;
  input[1][2] = 6;
  input[1][3] = 3;
  input[1][4] = -3;
  input[1][5] = -6;
  input[1][6] = -10;
  input[1][7] = -12;

  input[2][0] = 8;  // row col
  input[2][1] = 4;
  input[2][2] = -4;
  input[2][3] = -8;
  input[2][4] = -8;
  input[2][5] = -4;
  input[2][6] = 4;
  input[2][7] = 8;

  input[3][0] = 10;  // row col
  input[3][1] = -3;
  input[3][2] = -12;
  input[3][3] = -6;
  input[3][4] = 6;
  input[3][5] = 12;
  input[3][6] = 3;
  input[3][7] = -10;

  input[4][0] = 8;  // row col
  input[4][1] = -8;
  input[4][2] = -8;
  input[4][3] = 8;
  input[4][4] = 8;
  input[4][5] = -8;
  input[4][6] = -8;
  input[4][7] = 8;

  input[5][0] = 6;  // row col
  input[5][1] = -12;
  input[5][2] = 3;
  input[5][3] = 10;
  input[5][4] = 10;
  input[5][5] = -3;
  input[5][6] = 12;
  input[5][7] = -6;

  input[6][0] = 4;  // row col
  input[6][1] = -8;
  input[6][2] = 8;
  input[6][3] = -4;
  input[6][4] = -4;
  input[6][5] = 8;
  input[6][6] = -8;
  input[6][7] = 4;

  input[7][0] = 3;  // row col
  input[7][1] = -6;
  input[7][2] = 10;
  input[7][3] = -12;
  input[7][4] = 12;
  input[7][5] = -10;
  input[7][6] = 6;
  input[7][7] = -3;
}
