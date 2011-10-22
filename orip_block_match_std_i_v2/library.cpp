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
#include <iomanip>

#define metric(x) (abs(x)) // (x*x)

using namespace std;


match** create_matrix(int col, int row)
{
  // init memory
  match** mem = new match*[row];
  for (int k = 0; k < row; k++)
  {
    mem[k] = new match[col];
  }

  // reset memory
  for (int i = 0; i < row; i++)
    for (int j = 0; j < col; j++)
    {
      mem[i][j].src_x = 0;
      mem[i][j].src_y = 0;
      mem[i][j].dest_x = 0;
      mem[i][j].dest_y = 0;
      mem[i][j].m_value = INT_MAX;
    }

  return mem;
}

/*
void read_block(matrix input, int width, int height, int x, int y, matrix output, int width_b, int height_b)
{
  for (int i = 0; i < height_b; i++)
    for (int j = 0; j < width_b; j++)
      output[i][j] = input[i+y][j+x];
}
*/

void write_block(matrix input, int width_b, int height_b, int x, int y, matrix output, int width, int height)
{
  for (int i = 0; i < height_b; i++)
    for (int j = 0; j < width_b; j++)
      output[i+y][j+x] = input[i][j];
}

#define SPACE_S 3
#define SPACE_M 6

void print_matrix(match** matrix, int c_size, int r_size, int print_col, int print_row)
{
  for (int i = 0 ; i < print_row; i++)
  {
    for (int j = 0 ; j < print_col; j++)
    {
      cout << "    " << "[(" << setw(SPACE_S) << matrix[i][j].src_x << "," << setw(SPACE_S) << matrix[i][j].src_y << ")" 
        << " > " << setw(SPACE_M) << matrix[i][j].m_value << " >" 
        << " (" << setw(SPACE_S) << matrix[i][j].dest_x << "," << setw(SPACE_S) << matrix[i][j].dest_y << ")]";
    }
    cout << endl;
  }
}

void pick_best_and_store(matrix input, match& output, int x, int y, int range)
{
  int mat_dim = range*2+1;

  output.src_x = x;
  output.src_y = y;
  output.m_value = INT_MAX; // reset

  for (int i = 0; i < mat_dim; i++ ) // row
  {
    for (int j = 0; j < mat_dim; j++) // col
    {
      if ( input[i][j] < output.m_value )
      {
        output.m_value = input[i][j];
        output.dest_x = x - range + j;
        output.dest_y = y - range + i;
      }
    }
  }
}

void block_match(matrix search_area, int w_s_area, int h_s_area,
                           matrix block, int w_blk, int h_blk,
                           int x, int y, int w_img, int h_img, int search_range, matrix output)
{
  for (int i = 0; i < h_s_area; i++ ) // row
  {
    for (int j = 0; j < w_s_area; j++) // col
    {
      int bound_up = y + i - search_range;
      int bound_down = bound_up + h_blk;
      int bound_left = x + j - search_range;
      int bound_right = bound_left + w_blk;

      if ( bound_up < 0 || bound_down > h_img ) //bound_down >= h_img )
      {
        output[i][j] = INT_MAX;
      }
      else if ( bound_left < 0 || bound_right > w_img ) //bound_right >= w_img )
      {
        output[i][j] = INT_MAX;
      }
      else
      if ( bound_up >= 0 && bound_down <= h_img && bound_left >= 0 && bound_right <= w_img )
      {
        output[i][j] = 0;
        for (int n = 0; n < h_blk; n++)
        {       
          for (int m = 0; m < w_blk; m++)
          {
            int acc_t = search_area[i + n][j + m] - block[n][m];
            output[i][j] += metric(acc_t);
          }
        }
        output[i][j] = (output[i][j] << _SHIFT);
      }
    } // END col
  } // END row
}

void block_match_v2(matrix search_area, int w_s_area, int h_s_area,
                           matrix block, int w_blk, int h_blk,
                           int x, int y, int w_img, int h_img, int search_range, matrix output)
{
  for (int i = 0; i < h_s_area; i++ ) // row
  {
    for (int j = 0; j < w_s_area; j++) // col
    {
        output[i][j] = 0;
        for (int n = 0; n < h_blk; n++)
        {       
          for (int m = 0; m < w_blk; m++)
          {
            int acc_t = search_area[i + n][j + m] - block[n][m];
            output[i][j] += metric(acc_t);          
          }
        }
        output[i][j] = (output[i][j] << _SHIFT);
    } // END col
  } // END row
}

void block_match_and_store(matrix search_area, int w_s_area, int h_s_area,
                           matrix block, int w_blk, int h_blk,
                           int x, int y, int w_img, int h_img, int search_range,
                           match& output)
{
  int acc_t = 0;
  int acc = 0;

  output.src_x = x;
  output.src_y = y;
  output.m_value = INT_MAX; // reset

  for (int i = 0; i < h_s_area; i++ ) // row
  {
    for (int j = 0; j < w_s_area; j++) // col
    {
      int bound_up = y + i - search_range;
      int bound_down = bound_up + h_blk;
      int bound_left = x + j - search_range;
      int bound_right = bound_left + w_blk;

      //if ( bound_up < 0 || bound_down > h_img ) //bound_down >= h_img )
      //{
      //  acc = INT_MAX;
      //}
      //else if ( bound_left < 0 || bound_right > w_img ) //bound_right >= w_img )
      //{
      //  acc = INT_MAX;
      //}
      //else
      if ( bound_up >= 0 && bound_down <= h_img && bound_left >= 0 && bound_right <= w_img )
      {
        for (int n = 0; n < h_blk; n++)
        {
          if ( n%2 == 0 && acc >= output.m_value ) break;
          for (int m = 0; m < w_blk; m++)
          {
            acc_t = search_area[i + n][j + m] - block[n][m];
            acc += metric(acc_t); 
          }
        }

        if ( acc < output.m_value )
        {
          output.m_value = acc;
          output.dest_x = x - search_range + j;
          output.dest_y = y - search_range + i;
        }
        acc = 0;
      }
    } // END col
  } // END row
}

void block_match_i_spiral(matrix ref_area, int w_ref_area, int h_ref_area,
                          matrix block, int w_blk, int h_blk,
                          int x, int y, int search_range, match& output, int spiral_size)
{
  int r_area_size = search_range*2;

  int acc_t = 0;
  int acc = 0;
  int x_curr, y_curr;

  int x_pivot = search_range;// + (output.dest_x - output.src_x);
  int y_pivot = search_range;// + (output.dest_y - output.src_y);

  output.src_x = x;
  output.src_y = y;

  // look the center position first
  for (int n = 0; n < h_blk; n++) // 0 -> h_blk - 1
  {
    for (int m = 0; m < w_blk; m++) // 0 -> w_blk - 1
    {
      acc_t = ref_area[y_pivot + n][x_pivot + m] - block[n][m];
      acc += metric(acc_t); //(acc_t * acc_t);
    }
  }
  output.m_value = acc;
  output.dest_x = x;
  output.dest_y = y;

  // for each circle
  for (int sx = 1; sx <= spiral_size; sx++ )
  {
    // for each side (4 sides!)
    for (int v = 0; v < 4; v++)
    {
      // for each elem
      for ( int idx = (1-sx) ; idx <= sx ; idx++ )
      {
        switch (v) 
        {
        case 0: y_curr = y_pivot + idx; x_curr = x_pivot + sx; break;
        case 1: y_curr = y_pivot + sx;  x_curr = x_pivot - idx; break;
        case 2: y_curr = y_pivot - idx; x_curr = x_pivot - sx; break;
        case 3: y_curr = y_pivot - sx;  x_curr = x_pivot + idx; break;
        }

        if ( x_curr >= 0 && y_curr >=0 && x_curr < r_area_size && y_curr < r_area_size )
        {
          acc = 0;
          for (int n = 0; n < h_blk; n++) // 0 -> h_blk - 1
          {
            if ( acc >= output.m_value ) break;
            for (int m = 0; m < w_blk; m++) // 0 -> w_blk - 1
            {
              acc_t = ref_area[y_curr + n][x_curr + m] - block[n][m];
              acc += metric(acc_t); //(acc_t * acc_t); //
            }
          }

          if ( acc < output.m_value )
          {
            output.m_value = acc;
            output.dest_x = x_curr + x - search_range;
            output.dest_y = y_curr + y - search_range;
          }
        }
      }
    }
  }
}


void block_match_and_store_v2(matrix search_area, int w_s_area, int h_s_area,
                           matrix block, int w_blk, int h_blk,
                           int x, int y, int w_img, int h_img, int search_range,
                           match& output)
{
  int acc = 0;

  output.src_x = x;
  output.src_y = y;
  output.m_value = INT_MAX; // reset

  for (int i = 0; i < h_s_area; i++ ) // row
  {
    for (int j = 0; j < w_s_area; j++) // col
    {
        for (int n = 0; n < h_blk; n++)
        {
          if ( acc >= output.m_value ) break;
          for (int m = 0; m < w_blk; m++)
          {
            int acc_t = search_area[i + n][j + m] - block[n][m];
            acc += metric(acc_t);
          }
        }

        if ( acc < output.m_value )
        {
          output.m_value = acc;
          output.dest_x = x - search_range + j;
          output.dest_y = y - search_range + i;
        }
        acc = 0;
     
    } // END col
  } // END row
}

void block_match_log_search_and_store(matrix search_area, int w_s_area, int h_s_area,
                                      matrix block, int w_blk, int h_blk,
                                      int x, int y, int w_img, int h_img, int search_range,
                                      match& output)
{
  int acc_t = 0;
  int acc = 0;
  int step = search_range/2 + 1;

  int x_curr, y_curr; // ???
  int bound_up, bound_down, bound_left, bound_right; // ???

  int x_pivot = search_range;
  int y_pivot = search_range;

  output.src_x = x;
  output.src_y = y;
  output.m_value = INT_MAX;

  while (1)
  {
    if (step != 1)
    {
      // step 16, 8, 4, 2
      for ( int pos = 0; pos < 5; pos++ ) // 5 positions
      {
        switch (pos)
        {
        case 0: {
          x_curr = x_pivot;
          y_curr = y_pivot;
          break;
                }
        case 1: {
          x_curr = x_pivot + step;
          y_curr = y_pivot;
          break;
                }
        case 2: {
          x_curr = x_pivot;
          y_curr = y_pivot + step;
          break;
                }
        case 3: {
          x_curr = x_pivot - step;
          y_curr = y_pivot;
          break;
                }
        case 4: {
          x_curr = x_pivot;
          y_curr = y_pivot - step;
          break;
                }
        }

        // calculate match
        if ( x_curr >= 0 && y_curr >=0 && x_curr < w_s_area && y_curr < h_s_area )
        {
          // update boundaries
          bound_up    =  y + y_curr - search_range;
          bound_down  =  bound_up + h_blk;
          bound_left  =  x + x_curr - search_range;
          bound_right =  bound_left + w_blk;

          if ( bound_up < 0 || bound_down > h_img )
          {
            acc = INT_MAX;
          }
          else if ( bound_left < 0 || bound_right > w_img )
          {
            acc = INT_MAX;
          }
          else
          {
            for (int n = 0; n < h_blk; n++)
            {
              for (int m = 0; m < w_blk; m++)
              {
                acc_t = search_area[y_curr + n][x_curr + m] - block[n][m];
                acc += metric(acc_t);
              }
            }

            if ( acc < output.m_value )
            {
              output.m_value = acc;
              output.dest_x = x_curr + x - search_range;
              output.dest_y = y_curr + y - search_range;
            }
          }

          acc = 0;
        }
      }
      // is the best maching the one in center of the cross?
      //if ( (output.dest_x == x_pivot + x - search_range) && (output.dest_y == y_pivot + y - search_range) )
      //{
      //  return;
      //}

      if ( output.m_value == 0 ) return;

      // update pivot
      x_pivot = output.dest_x - x + search_range;
      y_pivot = output.dest_y - y + search_range;
      
    }
    else
    {
      for ( int pos = 0; pos < 9; pos++ ) // 9 positions
      {
        switch (pos)
        {
        case 0: {
          x_curr = x_pivot;
          y_curr = y_pivot;
          break;
                }
        case 1: {
          x_curr = x_pivot + 1;
          y_curr = y_pivot;
          break;
                }
        case 2: {
          x_curr = x_pivot + 1;
          y_curr = y_pivot + 1;
          break;
                }
        case 3: {
          x_curr = x_pivot;
          y_curr = y_pivot + 1;
          break;
                }
        case 4: {
          x_curr = x_pivot - 1;
          y_curr = y_pivot + 1;
          break;
                }
        case 5: {
          x_curr = x_pivot - 1;
          y_curr = y_pivot;
          break;
                }
        case 6: {
          x_curr = x_pivot - 1;
          y_curr = y_pivot - 1;
          break;
                }
        case 7: {
          x_curr = x_pivot;
          y_curr = y_pivot - 1;
          break;
                }
        case 8: {
          x_curr = x_pivot + 1;
          y_curr = y_pivot - 1;
          break;
                }
        }

        // calculate match
        if ( x_curr >= 0 && y_curr >=0 && x_curr < w_s_area && y_curr < h_s_area )
        {
         // update boundaries
          bound_up    =  y + y_curr - search_range;
          bound_down  =  bound_up + h_blk;
          bound_left  =  x + x_curr - search_range;
          bound_right =  bound_left + w_blk;

          if ( bound_up < 0 || bound_down > h_img )
          {
            acc = INT_MAX;
          }
          else if ( bound_left < 0 || bound_right > w_img )
          {
            acc = INT_MAX;
          }
          else
          {
            for (int n = 0; n < h_blk; n++)
            {
              for (int m = 0; m < w_blk; m++)
              {
                int acc_t = search_area[y_curr + n][x_curr + m] - block[n][m];
                acc += metric(acc_t);
              }
            }
            if ( acc < output.m_value )
            {
              output.m_value = acc;
              output.dest_x = x_curr + x - search_range;
              output.dest_y = y_curr + y - search_range;
            }
          }

          acc = 0;
        }

      }
      return; // end of the story!
    }

    step = step/2;  // update step
    if (step != 1) step++;
  }
}

/*
void read_search_area(matrix input, int w_in, int h_in, int x, int y, matrix output, int w_out, int h_out, int search_range)
{
  for (int i = 0; i < h_out; i++) // row
    for (int j = 0; j < w_out; j++) // col
    {
      int idx_y = y + i - search_range;
      int idx_x = x + j - search_range;
      if ( idx_y < 0 || idx_x < 0)
      {
        output[i][j] = 0;
      }
      else if ( idx_y >= h_in || idx_x >= w_in )
      {
        output[i][j] = 0;
      }
      else
      {
        output[i][j] = input[idx_y][idx_x];
      }
    }
}
*/