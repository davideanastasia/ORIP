/* ORIP_v2 –> Op Ref Im Proc -> Oper Refin Imag Process -> Operational Refinement of Image Processing
 *
 * Copyright Davide Anastasia and Yiannis Andreopoulos, University College London
 *
 * http://www.ee.ucl.ac.uk/~iandreop/ORIP.html
 * If you have not retrieved this source code via the above link, please contact danastas@ee.ucl.ac.uk and iandreop@ee.ucl.ac.uk
 * 
 */


// [row][col]

//#define EARLY_TERMINATION (1)

#include "common_lib.h"
#include "library.h"
#include <iomanip>

using namespace std;

// -- packing ---
long    __max_value;
int    __M;
double __eps;
int    __inv_eps;
int    __num_pack;
// --------------

void setup_env(int b_col, int b_row, int bit4bitplane)
{
  // calculate __max_value dinamically
  double max_sample_value = pow(2.0, bit4bitplane) - 1;
  __max_value = long (2 * pow(max_sample_value * (b_col * b_row), 2.0));

  double pow_2 = log10(double(__max_value))/log10(2.0);
  __M = int(ceil(pow_2) + 1);

  __inv_eps = int(pow(2.0, (__M)));
  __eps = pow(2.0, -(__M));

  __num_pack = 2; //__num_pack = int(floor(LIMIT/__M) + 1);

  while ( exp_error(__max_value) > 0.5 ) __num_pack--;
}

void show_env_status()
{
  cout << endl;
  cout << " Max Value = " << __max_value << endl;
  cout << " M         = " << __M << endl;
  cout << " EPS       = " << __eps << endl;
  cout << " 1/EPS     = " << __inv_eps << endl;
  cout << " Packing   = " << __num_pack << endl;
  cout << " Max Expect Error = " << pow(2.0, -(MAX_PACKING))*__max_value/pow(2.0, (-(__num_pack-1)*__M)) << endl;
  cout << endl;
}

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

int block_match_d(double**** ref_area, int w_ref_area, int h_ref_area,
                  double** block_ref, int w_blk, int h_blk,
                  int x, int y, int search_range, match& output)
{
  int early_termination=0;
  int curr_grid_ref,grid_j,progress_grid;
  int curr_SSD_grid1,curr_SSD_grid2;
  double curr_SSD_grid=0,tmp;

  output.m_value = INT_MAX;
  for (int i=0; i<2*search_range; i++)
  {
    for (int j=0; j<2*search_range; j++)
    {
      if ((i%2)==0 && (j%2)==0)
      {
        curr_grid_ref=0;
        grid_j=j/2;
        progress_grid=0;
      }
      if ((i%2)==1 && (j%2)==0)
      {
        curr_grid_ref=1;
        grid_j=j/2;
        progress_grid=0;
      }
      if ((i%2)==0 && (j%2)==1)
      {
        curr_grid_ref=1;
        grid_j=(j-1)/2;
        progress_grid=1;
      }
      if ((i%2)==1 && (j%2)==1)
      {
        curr_grid_ref=0;
        grid_j=(j-1)/2;
        progress_grid=1;
      }

      curr_SSD_grid = 0;
      early_termination = 0;

      for (int block_i=1; block_i<=h_blk; block_i++)
      {
        for (int block_j=1; block_j<=w_blk/2; block_j++)
        {
          tmp = (block_ref[block_i][block_j] - ref_area[progress_grid][curr_grid_ref][i+block_i][grid_j+block_j]);
          curr_SSD_grid += (tmp*tmp);
        }
#ifdef EARLY_TERMINATION
        if ((block_i%2)==0)
        {
          double R0,R1, R2;
          int U1;

          R0 = curr_SSD_grid;
          curr_SSD_grid1 = full_round(R0);
          R1 = (R0-curr_SSD_grid1)/__eps;
          U1 = full_round(R1);
          R2 = (R1-U1)/__eps;
          curr_SSD_grid2 = full_round(R2);

          if ( curr_SSD_grid1 + curr_SSD_grid2 > output.m_value )
          {
            // the unpacked SSDs exceed the best--> terminate
            early_termination=1;
            break;
          }
        }
#endif
      }

#ifdef EARLY_TERMINATION
      if ( early_termination == 0 )
      {
        output.m_value = curr_SSD_grid1 + curr_SSD_grid2;
        output.dest_x = x + j - search_range;
        output.dest_y = y + i - search_range;
      }
#else
      double R0,R1, R2;
      int U1;

      R0 = curr_SSD_grid;
      curr_SSD_grid1 = full_round(R0);
      R1 = (R0-curr_SSD_grid1)/__eps;
      U1 = full_round(R1);
      R2 = (R1-U1)/__eps;
      curr_SSD_grid2 = full_round(R2);

      if ( curr_SSD_grid1 + curr_SSD_grid2 < output.m_value )
      {
        output.m_value=curr_SSD_grid1+curr_SSD_grid2;
        output.dest_x = x + j - search_range;
        output.dest_y = y + i - search_range;
      }
#endif
    }
  }

  return 0;
}

int block_match_d_log_search(double**** ref_area, int w_ref_area, int h_ref_area,
                             double** block_ref, int w_blk, int h_blk,
                             int x, int y, int search_range, match& output, int spiral_size)

{
  int r_area_size = search_range*2;
  int early_termination = 0;
  int curr_grid_ref;
  int grid_j;
  int progress_grid;
  int curr_SSD_grid1;
  int curr_SSD_grid2;
  double curr_SSD_grid = 0;
  double tmp;

  output.m_value = INT_MAX;

  int acc = 0;
  int acc_d = 0;
  int x_curr, y_curr;

  int x_pivot = search_range; // central point of the reference area
  int y_pivot = search_range;

  // step 16, 8, 4, 2
  int step = search_range/2 + 1;
  while (1)
  {
    if ( step != 1)
    {
      for ( int pos = 0; pos < 5; pos++ ) // 5 positions
      {
        switch (pos)
        {
        case 0: x_curr = x_pivot; y_curr = y_pivot; break;
        case 1: x_curr = x_pivot + step; y_curr = y_pivot; break;
        case 2: x_curr = x_pivot; y_curr = y_pivot + step; break;
        case 3: x_curr = x_pivot - step; y_curr = y_pivot; break;
        case 4: x_curr = x_pivot; y_curr = y_pivot - step; break;
        } // END switch

        // calculate match
        if ( x_curr >= 0 && y_curr >=0 && x_curr < r_area_size && y_curr < r_area_size )
        {
          if ((y_curr%2)==0 && (x_curr%2)==0)
          {
            curr_grid_ref=0;
            grid_j=x_curr/2;
            progress_grid=0;
          }
          if ((y_curr%2)==1 && (x_curr%2)==0)
          {
            curr_grid_ref=1;
            grid_j=x_curr/2;
            progress_grid=0;
          }
          if ((y_curr%2)==0 && (x_curr%2)==1)
          {
            curr_grid_ref=1;
            grid_j=(x_curr-1)/2;
            progress_grid=1;
          }
          if ((y_curr%2)==1 && (x_curr%2)==1)
          {
            curr_grid_ref=0;
            grid_j=(x_curr-1)/2;
            progress_grid=1;
          }

          curr_SSD_grid = 0;
#ifdef EARLY_TERMINATION
          early_termination = 0;
#endif

          for (int block_i=1; block_i<=h_blk; block_i++)
          {
            for (int block_j=1; block_j<=w_blk/2; block_j++)
            {
              tmp = (block_ref[block_i][block_j] - ref_area[progress_grid][curr_grid_ref][y_curr + block_i][grid_j + block_j]);
              curr_SSD_grid += (tmp*tmp);
            }
#ifdef EARLY_TERMINATION
            if ((block_i%2)==0)
            {
              double R0,R1, R2;
              int U1;

              R0 = curr_SSD_grid;
              curr_SSD_grid1 = full_round(R0);
              R1 = (R0-curr_SSD_grid1)/__eps;
              U1 = full_round(R1);
              R2 = (R1-U1)/__eps;
              curr_SSD_grid2 = full_round(R2);

              if ( curr_SSD_grid1 + curr_SSD_grid2 > output.m_value )
              {
                // the unpacked SSDs exceed the best--> terminate
                early_termination=1;
                break;
              }
            }
#endif
          }

#ifdef EARLY_TERMINATION
          if ( early_termination == 0 )
          {
            output.m_value = curr_SSD_grid1 + curr_SSD_grid2;
            output.dest_x = x + x_curr - search_range;
            output.dest_y = y + y_curr - search_range;
          }
#else
          double R0,R1, R2;
          int U1;

          R0 = curr_SSD_grid;
          curr_SSD_grid1 = full_round(R0);
          R1 = (R0-curr_SSD_grid1)/__eps;
          U1 = full_round(R1);
          R2 = (R1-U1)/__eps;
          curr_SSD_grid2 = full_round(R2);

          if ( curr_SSD_grid1 + curr_SSD_grid2 < output.m_value )
          {
            output.m_value = curr_SSD_grid1 + curr_SSD_grid2;
            output.dest_x = x + x_curr - search_range;
            output.dest_y = y + y_curr - search_range;
          }
#endif
        } // if ( x_curr >= 0 && y_curr >=0 && x_curr < w_s_area && y_curr < h_s_area )
      } // for ( int pos = 0; pos < 5; pos++ )

      step = step/2;  // update step
      // if (step == 0) break;
      if (step != 1) step++;
      // update pivot
      x_pivot = search_range + (output.dest_x - output.src_x);
      y_pivot = search_range + (output.dest_y - output.src_y);
    }
    else
    {
      //  STEP is 1, so I do a spiral search around the already found best match
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
              if ((y_curr%2)==0 && (x_curr%2)==0)
              {
                curr_grid_ref=0;
                grid_j=x_curr/2;
                progress_grid=0;
              }
              if ((y_curr%2)==1 && (x_curr%2)==0)
              {
                curr_grid_ref=1;
                grid_j=x_curr/2;
                progress_grid=0;
              }
              if ((y_curr%2)==0 && (x_curr%2)==1)
              {
                curr_grid_ref=1;
                grid_j=(x_curr-1)/2;
                progress_grid=1;
              }
              if ((y_curr%2)==1 && (x_curr%2)==1)
              {
                curr_grid_ref=0;
                grid_j=(x_curr-1)/2;
                progress_grid=1;
              }

              curr_SSD_grid = 0;
#ifdef EARLY_TERMINATION
              early_termination = 0;
#endif

              for (int block_i=1; block_i<=h_blk; block_i++)
              {
                for (int block_j=1; block_j<=w_blk/2; block_j++)
                {
                  tmp = (block_ref[block_i][block_j] - ref_area[progress_grid][curr_grid_ref][y_curr + block_i][grid_j + block_j]);
                  curr_SSD_grid += (tmp*tmp);
                }
#ifdef EARLY_TERMINATION
                if ((block_i%2)==0)
                {
                  double R0,R1, R2;
                  int U1;

                  R0 = curr_SSD_grid;
                  curr_SSD_grid1 = full_round(R0);
                  R1 = (R0-curr_SSD_grid1)/__eps;
                  U1 = full_round(R1);
                  R2 = (R1-U1)/__eps;
                  curr_SSD_grid2 = full_round(R2);

                  if ( curr_SSD_grid1 + curr_SSD_grid2 > output.m_value )
                  {
                    // the unpacked SSDs exceed the best--> terminate
                    early_termination=1;
                    break;
                  }
                }
#endif
              }

#ifdef EARLY_TERMINATION
              if ( early_termination == 0 )
              {
                output.m_value = curr_SSD_grid1 + curr_SSD_grid2;
                output.dest_x = x + x_curr - search_range;
                output.dest_y = y + y_curr - search_range;
              }
#else
              double R0,R1, R2;
              int U1;

              R0 = curr_SSD_grid;
              curr_SSD_grid1 = full_round(R0);
              R1 = (R0-curr_SSD_grid1)/__eps;
              U1 = full_round(R1);
              R2 = (R1-U1)/__eps;
              curr_SSD_grid2 = full_round(R2);

              if ( curr_SSD_grid1 + curr_SSD_grid2 < output.m_value )
              {
                output.m_value = curr_SSD_grid1 + curr_SSD_grid2;
                output.dest_x = x + x_curr - search_range;
                output.dest_y = y + y_curr - search_range;
              }
#endif
            }
          }
        }
      }


  return 0;
    } // END IF step == 1;
  }   // while (1)
}

void block_match_d_spiral(matrix ref_area, int w_ref_area, int h_ref_area,
                          matrix block, int w_blk, int h_blk,
                          int x, int y, int search_range, match& output, int spiral_size)
{
  int r_area_size = search_range*2;

  int acc_t = 0;
  int acc = 0;
  int x_curr, y_curr;

  int x_pivot = search_range + (output.dest_x - output.src_x);
  int y_pivot = search_range + (output.dest_y - output.src_y);

  // look the center position first
  for (int n = 1; n <= h_blk; n++) // 0 -> h_blk - 1
  {
    for (int m = 1; m <= w_blk; m++) // 0 -> w_blk - 1
    {
      acc_t = ref_area[y_pivot + n][x_pivot + m] - block[n][m];
      acc += (acc_t * acc_t);
    }
  }
  output.m_value = acc;

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
          for (int n = 1; n <= h_blk; n++) // 0 -> h_blk - 1
          {
            if ( acc >= output.m_value ) break;
            for (int m = 1; m <= w_blk; m++) // 0 -> w_blk - 1
            {
              acc_t = ref_area[y_curr + n][x_curr + m] - block[n][m];
              acc += (acc_t * acc_t);
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


/*
void pick_best_and_store(matrix input, match& output, int x, int y, int search_range)
{
int mat_dim = (search_range*2)+1;

output.src_x = x;
output.src_y = y;
output.m_value = INT_MAX;

for (int i = 0; i < mat_dim; i++ ) // row
{
for (int j = 0; j < mat_dim; j++) // col
{
if ( input[i][j] < output.m_value )
{
output.m_value = input[i][j];
output.dest_x = x - search_range + j;
output.dest_y = y - search_range + i;
}
}
}
}
*/

/*
void pick_best_and_store_bounded(matrix input, match& output, int x, int y, int search_range, int w_img, int h_img, int blk_size)
{
int mat_dim = (search_range*2)+1;

output.src_x = x;
output.src_y = y;
output.m_value = INT_MAX;

int x_base = x - search_range;
int y_base = y - search_range;

for (int i = 0; i < mat_dim; i++ ) // row
{
for (int j = 0; j < mat_dim; j++) // col
{
if ( input[i][j] < output.m_value )
{
// min border
if (y_base + i >= 0) 
if (x_base + j >= 0)
if (x_base + j + blk_size <= w_img)
if (y_base + i + blk_size <= h_img)
{
output.m_value = input[i][j];
output.dest_x = x_base + j;
output.dest_y = y_base + i;
}
}
}
}
}
*/

void pack_ref_area(matrix input, int elem, double**** output, int width, int height)
{
  int jout;
  // [prog_i][grid_i][i][j]
  for (int i=1; i<=height; i++)
  {
    jout=1;
    for (int j=(2-(i%2)); j<=width; j+=2)
    {
      // 1,1 in Matlab
      output[0][0][i][jout]=input[i][j];
      output[0][1][i][jout]=input[i][j]*__eps;
      jout++;
    }
    jout=1;
    for (int j=((i%2)+1); j<=width; j+=2)
    {
      // 2,1 in Matlab
      output[0][0][i][jout]+=input[i][j]*__eps;
      output[0][1][i][jout]+=input[i][j];
      jout++;
    }
    jout=1;
    for (int j=(2+(i%2)); j<=width; j+=2)
    {
      // 1,2 in Matlab
      output[1][0][i][jout]=input[i][j];
      output[1][1][i][jout]=input[i][j]*__eps;
      jout++;
    }
    jout=1;
    for (int j=(3-(i%2)); j<=width; j+=2)
    {
      // 2,2 in Matlab
      output[1][0][i][jout]+=input[i][j]*__eps;
      output[1][1][i][jout]+=input[i][j];
      jout++;
    }
  }
}

void pack_block(matrix block, int elem, double** output, int width, int height)
{
  int jout;
  for (int i=1; i<=height; i++)  // row
  {
    jout=1;
    for (int j=(2-(i%2)); j<=width; j+=2)
    {
      output[i][jout]=block[i][j];
      jout++;
    }
    jout=1;
    for (int j=((i%2)+1); j<=width; j+=2)
    {
      output[i][jout]+=block[i][j]*__eps;
      jout++;
    }


    //if ( i%2 == 0 ) {
    //  output[i][j]= blocks[i][j] + eps*block[i][j+1];
    //} else {
    //  output[i][j]= blocks[i][j+1] + eps*block[i][j];     
    //}

  }
}

void unpack_matrix_and_store_d(double** input, matrix* output, int b_idx, int elem, int width, int height, int bitplane, int bits)
{
  int tmp = 0;
  double ax = 0.0;
  int bx = 0;
  int shift = (bitplane - bits + 1)*2;

  for (int row = 0; row < height; row++) // row
  {
    for (int col = 0; col < width; col++) // col
    {
      if ( input[row][col] == MARKER )
      {
        // it never happens with the new code
        output[b_idx][row][col] = INT_MAX;
        output[b_idx + 1][row][col] = INT_MAX;
      }
      else
      {
        ax = input[row][col];
        bx = full_round(ax);
        output[b_idx][row][col] += (bx << shift);

        ax = (ax - bx)/(__eps);
        bx = full_round(ax);
        //output[b_idx + i][row][col] += (bx << shift);

        ax = (ax - bx)/(__eps);
        bx = full_round(ax);
        output[b_idx + 1][row][col] += (bx << shift);
      }
    }
  }
}

void write_block(matrix input, int width_b, int height_b, int x, int y, matrix output, int width, int height)
{
  for (int i = 0; i < height_b; i++)
    for (int j = 0; j < width_b; j++)
      output[i+y][j+x] = input[i+1][j+1];
}

void block_index_2_x_y(int blk_idx, int &x, int &y, int n_h_blocks, int block_size)
{
  y = block_size*((blk_idx)/n_h_blocks);
  x = block_size*((blk_idx)%n_h_blocks);
}