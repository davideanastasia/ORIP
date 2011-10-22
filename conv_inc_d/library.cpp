/* ORIP_v2 –> Op Ref Im Proc -> Oper Refin Imag Process -> Operational Refinement of Image Processing
 *
 * Copyright Davide Anastasia and Yiannis Andreopoulos, University College London
 *
 * http://www.ee.ucl.ac.uk/~iandreop/ORIP.html
 * If you have not retrieved this source code via the above link, please contact danastas@ee.ucl.ac.uk and iandreop@ee.ucl.ac.uk
 * 
 */


// to index matrixes, use [row][col]

#define PRECISION     (8)
#define PRECISION_6   (6)

#include "common_lib.h"
#include "library.h"

using namespace std;

// -- packing ---
long   __max_value;
int    __M;
double __eps;
int    __inv_eps;
int    __num_pack;
// --------------

// boolean for threading cooperation
#ifdef _OPENMP
bool flag_int = false;
bool flag_scheduler = true;
#endif
// ---

void setup_env(matrix coeff_matrix, int _col, int _row, int bit4bitplane)
{
  my_t counter = 0;
  for ( int i = 0 ; i < _row; i++ )
    for ( int j = 0 ; j < _col; j++ )
      if ( coeff_matrix[i][j] >= 0) counter += coeff_matrix[i][j];

  // calculate __max_value dinamically
  __max_value = long((pow(2.0, bit4bitplane)- 1)*counter);

  double pow_2 = log10(double(__max_value))/log10(2.0);
  __M = int(ceil(pow_2) + 1);   // + 1 Safety Bit 
  __inv_eps = int(pow(2.0, (__M)));
  __eps = pow(2.0, -(__M));
  __num_pack = int(floor(LIMIT/__M) + 1);   // + 1 because there's 2^0

  while ( exp_error(__max_value) > 0.5 )
  {
    cout << endl << " Max Expect Error is " << exp_error(__max_value) << " packing " << __num_pack << endl;
    __num_pack--;
  }

  //__num_pack = 1;
}

void show_env_status()
{
  cout << endl;
  cout << " Max Value = " << __max_value << endl;
  cout << " M         = " << __M << endl;
  cout << " EPS       = " << __eps << endl;
  cout << " 1/EPS     = " << __inv_eps << endl;
  cout << " Packing   = " << __num_pack << endl;
  cout << " Max Expect Error = " << exp_error(__max_value) << endl;
  cout << endl;
}

void conv2(double** mat1, int rows1, int cols1, matrix mat2, int rows2, int cols2, double** res, int n_shift)
{
	int i, j, ic, jc;
	int offset_rows, offset_cols;

  double tmp_conv;
  static double** copymat1 = create_matrix<double>(cols1 + cols2 - 1, rows1 + rows2 - 1);

	if ( rows2%2 == 0 ) offset_rows = rows2/2-1; else offset_rows = rows2/2;
	if ( cols2%2 == 0 ) offset_cols = cols2/2-1; else offset_cols = cols2/2;

	for ( i = 0; i < rows1; i++ )
	{
		for ( j = 0; j < cols1; j++ )
		{
			copymat1[offset_rows+i][offset_cols+j] = mat1[i][j];
			res[i][j] = 0.0;
		}
	}

	for (i = 0; i < rows1; i++)
	{
		for (j = 0; j < cols1; j++)
		{

#ifdef _OPENMP
       // stop function execution
      if (flag_int == true) return;
#endif

			for (ic = 0; ic < rows2; ic++)
			{
				for (jc = 0; jc < cols2; jc++)
				{
					tmp_conv = ( copymat1[i+ic][j+jc] * mat2[rows2-1-ic][cols2-1-jc] );
					res[i][j] += tmp_conv;
				}
			}
		}
	}
}

// it works on symmetric kernels
void conv2_symmetric(double** in_packed, int rows1, int cols1, matrix kernel, int rows2, int cols2, double** res)
{
  //double conv_t = 0;
	int i, j, ic, jc;
	int offset_rows, offset_cols;

  static double** in_packed_copy = create_matrix<double>(cols1 + cols2 - 1, rows1 + rows2 - 1);

	if ( rows2%2 == 0 ) offset_rows = rows2/2-1; else offset_rows = rows2/2;
	if ( cols2%2 == 0 ) offset_cols = cols2/2-1; else offset_cols = cols2/2;

	for ( i = 0; i < rows1; i++ )
	{
		for ( j = 0; j < cols1; j++ )
		{
			in_packed_copy[offset_rows + i][offset_cols + j] = in_packed[i][j];
			res[i][j] = 0;
		}
	}

  int rows2_t = rows2/2;   // if the kernel matrix is symmetric, rows2 is pair
  int cols2_t = cols2/2;   // same as above

  int max_ic = rows2 - 1;
  int max_jc = cols2 - 1;

  double elem_t = 0;

	for (i = 0; i < rows1; i++)
	{
		for (j = 0; j < cols1; j++)
		{

#ifdef _OPENMP
      // stop function execution
      if (flag_int == true) return;
#endif

      // res[i][j] = 0.0; // already done in the upper loop
			for (ic = 0; ic < rows2_t; ic++)
			{
				for (jc = 0; jc < cols2_t; jc++)
				{
          elem_t  = in_packed_copy[i+ic][j+jc]
                  + in_packed_copy[i+(max_ic - ic)][j + (max_jc - jc)]
                  + in_packed_copy[i+ic][j+(max_jc - jc)]
                  + in_packed_copy[i+(max_ic - ic)][j + jc];

					res[i][j] += elem_t * kernel[rows2-1-ic][cols2-1-jc];
				}
			}
      //res[i][j] = conv_t;
		}
	}
}

// it works on symmetric kernels
void conv2_unpack_store_sym(double** in_packed, int i_height, int i_width, matrix kernel, int k_height, int k_width,
                            matrix output, int o_width, int o_height, int elem, int bitplane, int bits)
{
  int shift = bitplane - bits + 1;

  int i, j, ic, jc;
	int offset_rows, offset_cols;

	if ( k_height%2 == 0 ) offset_rows = k_height/2-1; else offset_rows = k_height/2;
	if ( k_width%2 == 0 ) offset_cols = k_width/2-1; else offset_cols = k_width/2;

  // --- putting zeros around the kernel
  static double** in_packed_copy = create_matrix<double>(i_width + k_width - 1, i_height + k_height - 1);

	for ( i = 0; i < i_height; i++ )
	{
		for ( j = 0; j < i_width; j++ )
		{
			in_packed_copy[offset_rows + i][offset_cols + j] = in_packed[i][j];
		}
	}
  // ---

  // --- convolution
  double elem_t = 0;
  double curr_elem = 0.0;

  int k_height_t = k_height/2;    // if the kernel matrix is symmetric, k_height is pair
  int k_width_t = k_width/2;      // same as above

  int max_ic = k_height - 1;
  int max_jc = k_width - 1;
  // ---

  // --- unpacking
  double ax = 0.0;
  long bx = 0;
  int b_idx = o_height / elem;  // it should be an integer

  int offset_top = (i_height - b_idx)/2;
  int offset_bottom = offset_top;
  if ( (i_height - b_idx)%2 == 1 ) offset_bottom++;
  
  register int temp_idx;
  // ---

	for (i = 0; i < i_height; i++)
	{
		for (j = 0; j < i_width; j++)
		{

#ifdef _OPENMP
      // stop function execution
      if (flag_int == true) return;
#endif

      if ( (i >= offset_top) && (i < i_height - offset_bottom) )
      {
        // --- convolution
        curr_elem = 0.0;
        for (ic = 0; ic < k_height_t; ic++)
        {
          for (jc = 0; jc < k_width_t; jc++)
          {
            elem_t  = in_packed_copy[i+ic][j+jc]
            + in_packed_copy[i+(max_ic - ic)][j + (max_jc - jc)]
            + in_packed_copy[i+ic][j+(max_jc - jc)]
            + in_packed_copy[i+(max_ic - ic)][j + jc];

            curr_elem += elem_t * kernel[k_height-1-ic][k_width-1-jc];
          }
        }
        // ---

        // --- unpacking
        for (int e = 0; e < elem; e++)  // elem packed
        {
          if ( e == 0 )
            ax = curr_elem;
          else
            ax = (ax - bx)/(__eps);
          bx = full_round(ax);

          temp_idx = (b_idx * e) + i - offset_top;
          output[temp_idx][j] += (bx << shift);
        }
        // ---
      }

    }
	}
}

// it works on symmetric kernels
void conv2_unpack_store(double** in_packed, int i_height, int i_width, matrix kernel, int k_height, int k_width,
                            matrix output, int o_width, int o_height, int elem, int bitplane, int bits)
{
  int shift = bitplane - bits + 1;

  int i, j, ic, jc;
	int offset_rows, offset_cols;

	if ( k_height%2 == 0 ) offset_rows = k_height/2-1; else offset_rows = k_height/2;
	if ( k_width%2 == 0 ) offset_cols = k_width/2-1; else offset_cols = k_width/2;

  // --- putting zeros around the kernel
  static double** in_packed_copy = create_matrix<double>(i_width + k_width - 1, i_height + k_height - 1);

	for ( i = 0; i < i_height; i++ )
	{
		for ( j = 0; j < i_width; j++ )
		{
			in_packed_copy[offset_rows + i][offset_cols + j] = in_packed[i][j];
		}
	}
  // ---

  // --- convolution
  double elem_t = 0;
  double curr_elem = 0.0;

  int max_ic = k_height - 1;
  int max_jc = k_width - 1;
  // ---

  // --- unpacking
  double ax = 0.0;
  long bx = 0;
  int b_idx = o_height / elem;  // it should be an integer

  int offset_top = (i_height - b_idx)/2;
  int offset_bottom = offset_top;
  if ( (i_height - b_idx)%2 == 1 ) offset_bottom++;
  
  register int temp_idx;
  // ---
	for (i = 0; i < i_height; i++)
	{
		for (j = 0; j < i_width; j++)
		{

#ifdef _OPENMP
      // stop function execution
      if (flag_int == true) return;
#endif
    
      if ( (i >= offset_top) && (i < i_height - offset_bottom) )
      {
        // --- convolution
        curr_elem = 0.0;
        for (ic = 0; ic < k_height; ic++)
        {
          for (jc = 0; jc < k_width; jc++)
          {
            curr_elem += ( in_packed_copy[i+ic][j+jc] * kernel[k_height-1-ic][k_width-1-jc] );
          }
        }
        // ---

        // --- unpacking
        for (int e = 0; e < elem; e++)  // elem packed
        {
          if ( e == 0 )
            ax = curr_elem;
          else
            ax = (ax - bx)/(__eps);
          bx = full_round(ax);

          temp_idx = (b_idx * e) + i - offset_top;
          output[temp_idx][j] += (bx << shift);
        } 
        // ---
      }

    }
	}
}

void setup_conv_matrix(matrix input, int col, int row, int max_value)
{
  srand ( 0 );

  for (int i = 0; i < row; i++)
  {
    for ( int j = 0; j < col; j++)
    {
      input[i][j] = (rand() % max_value); // - (max_value/2);
    }
  }
}

void pack_matrix(matrix input, int i_width, int i_height, double** output, int o_width, int o_height, int elem)
{
  int b_idx = i_height / elem;  // it should be an integer
  int offset = (o_height - b_idx)/2;
  
  //int *idx = new int[elem];
  //for ( int i = 0; i < elem; i++ ) idx[i] = b_idx*i;

  // V.1
  /*
  // first pack
  for (int j = 0; j < o_height; j++)
  {
    int temp_idx = j - offset;
    for (int k = 0; k < o_width; k++)
    {
      //int temp_idx = j - offset; // int temp_idx = idx[0] - offset + j;
      int value = 0;
      if ( temp_idx >= 0 && temp_idx < i_height ) value = input[temp_idx][k];

      output[j][k] = value;
    }
  }

  // 2nd to elem-th pack
  double temp_eps = __eps;
  for (int e = 1; e < elem; e++)
  {
    int base_idx = b_idx*e;
    for (int j = 0; j < o_height; j++)
    {
      int temp_idx = idx[e] - offset + j;
      for (int k = 0; k < o_width; k++)
      {
        // int temp_idx = base_idx - offset + j;
        int value = 0;
        if ( temp_idx >= 0 && temp_idx < i_height ) value = input[temp_idx][k];

        output[j][k] += temp_eps * value;
      }
    }
    temp_eps *= __eps;
  }
  */
  
  // V.2
  // first pack
  for (int j = 0; j < o_height; j++)
  {
    int temp_idx = j - offset;
    if ( temp_idx >= 0 && temp_idx < i_height ) {
      for (int k = 0; k < o_width; k++)
      {
        output[j][k] = input[temp_idx][k];
      }
    } else {
      for (int k = 0; k < o_width; k++)
      {
        output[j][k] = 0;
      }
    }
  }

  // 2nd to elem-th pack
  double temp_eps = __eps;
  for (int e = 1; e < elem; e++)
  {
    int base_idx = b_idx*e;
    for (int j = 0; j < o_height; j++)
    {
      int temp_idx = base_idx - offset + j;
      if ( temp_idx >= 0 && temp_idx < i_height )
      {
        for (int k = 0; k < o_width; k++)
        {
          output[j][k] += temp_eps * input[temp_idx][k];
        }
      } // else do nothing
    }
    temp_eps *= __eps;
  }
}

void unpack_matrix_and_store_i(double** input, int i_width, int i_height, matrix output, int o_width, int o_height, int elem, int bitplane, int bits)
{
  int shift = bitplane - bits + 1;

  int b_idx = o_height / elem;  // it should be an integer

  int offset_top = (i_height - b_idx)/2;
  int offset_bottom = offset_top;
  if ( (i_height - b_idx)%2 == 1 ) offset_bottom++;

  // V.1
  double ax = 0.0;
  my_t bx = 0;
  for (int j = offset_top; j < (i_height - offset_bottom); j++)  // row
  {
    for (int k = 0; k < i_width; k++) // col
    {
      for (int e = 0; e < elem; e++)  // elem packed
      {
        if ( e == 0 )
          ax = input[j][k];
        else
          ax = (ax - bx)/(__eps);
        bx = full_round(ax);

        int temp_idx = b_idx*e - offset_top + j;
        output[temp_idx][k] += (bx << shift);
      }
    }
  }


  // V.2
  /*
  static double** ax = create_matrix<double>(i_width, i_height);
  static my_t** bx = create_matrix<my_t>(i_width, i_height);

  for (int j = offset_top; j < (i_height - offset_bottom); j++)  // row
  {
    int temp_idx = j - offset_top;
    for (int k = 0; k < i_width; k++) // col
    {
      ax[j][k] = input[j][k];
      bx[j][k] = full_round(ax[j][k]);

      output[temp_idx][k] += (bx[j][k] << shift);
    }
  }

  for (int e = 1; e < elem; e++)  // elem packed
  {
    for (int j = offset_top; j < (i_height - offset_bottom); j++)  // row
    {
      int temp_idx = idx[e] - offset_top + j;
      for (int k = 0; k < i_width; k++) // col
      {
        ax[j][k] = (ax[j][k] - bx[j][k])/(__eps);
        bx[j][k] = full_round(ax[j][k]);

        output[temp_idx][k] += (bx[j][k] << shift);
      }
    }
  }
  */
}

void setup_conv_gaussian_6x6(matrix input)
{
double g6[6][6]={               0.00030833651364,   0.00227831579665,   0.00619310442954,   0.00619310442954,   0.00227831579665,   0.00030833651364,
                                               0.00227831579665,   0.01683460323256,   0.04576119605639,   0.04576119605639,   0.01683460323256,   0.00227831579665,
                                               0.00619310442954,   0.04576119605639,   0.12439182768863,   0.12439182768863,   0.04576119605639,   0.00619310442954,
                                               0.00619310442954,   0.04576119605639,   0.12439182768863,   0.12439182768863,   0.04576119605639,   0.00619310442954,
                                               0.00227831579665,   0.01683460323256,   0.04576119605639,   0.04576119605639,   0.01683460323256,   0.00227831579665,
                                               0.00030833651364,   0.00227831579665,   0.00619310442954,   0.00619310442954,   0.00227831579665,   0.00030833651364};


  double** _g6 = create_matrix<double>(6,6);

  for (int i = 0; i < 6; i++)
    for (int j = 0; j < 6; j++)
      _g6[i][j] = g6[i][j];

  convert2int(_g6, 6, 6, PRECISION_6, input); // in, size, size, n_shift, out
  free_matrix(_g6, 6, 6);
}

void setup_conv_gaussian_12x12(matrix input)
{
  double g12[12][12] = {	0.00002077426027,   0.00007250929304,   0.00019710069366,   0.00041726217174,   0.00068794901801,   0.00088334402452,   0.00088334402452,   0.00068794901801,   0.00041726217174,	0.00019710069366,   0.00007250929304,   0.00002077426027,
    0.00007250929304,   0.00025308230030,   0.00068794901801,   0.00145638808256,   0.00240117801012,   0.00308317359498,   0.00308317359498,   0.00240117801012,   0.00145638808256,	0.00068794901801,   0.00025308230030,   0.00007250929304,
    0.00019710069366,   0.00068794901801,   0.00187003931457,   0.00395887326002,   0.00652707855180,   0.00838093475723,   0.00838093475723,   0.00652707855180,   0.00395887326002,	0.00187003931457,   0.00068794901801,   0.00019710069366,
    0.00041726217174,   0.00145638808256,   0.00395887326002,   0.00838093475723,   0.01381782540259,   0.01774243902029,   0.01774243902029,   0.01381782540259,   0.00838093475723,	0.00395887326002,   0.00145638808256,   0.00041726217174,	
    0.00068794901801,   0.00240117801012,   0.00652707855180,   0.01381782540259,   0.02278174265608,   0.02925233660684,   0.02925233660684,   0.02278174265608,   0.01381782540259,	0.00652707855180,   0.00240117801012,   0.00068794901801,
    0.00088334402452,   0.00308317359498,   0.00838093475723,   0.01774243902029,   0.02925233660684,   0.03756074370069,   0.03756074370069,   0.02925233660684,   0.01774243902029,	0.00838093475723,   0.00308317359498,   0.00088334402452,
    0.00088334402452,   0.00308317359498,   0.00838093475723,   0.01774243902029,   0.02925233660684,   0.03756074370069,   0.03756074370069,   0.02925233660684,   0.01774243902029,	0.00838093475723,   0.00308317359498,   0.00088334402452,
    0.00068794901801,   0.00240117801012,   0.00652707855180,   0.01381782540259,   0.02278174265608,   0.02925233660684,   0.02925233660684,   0.02278174265608,   0.01381782540259,	0.00652707855180,   0.00240117801012,   0.00068794901801,
    0.00041726217174,   0.00145638808256,   0.00395887326002,   0.00838093475723,   0.01381782540259,   0.01774243902029,   0.01774243902029,   0.01381782540259,   0.00838093475723,	0.00395887326002,   0.00145638808256,   0.00041726217174,
    0.00019710069366,   0.00068794901801,   0.00187003931457,   0.00395887326002,   0.00652707855180,   0.00838093475723,   0.00838093475723,   0.00652707855180,   0.00395887326002,	0.00187003931457,   0.00068794901801,   0.00019710069366,
    0.00007250929304,   0.00025308230030,   0.00068794901801,   0.00145638808256,   0.00240117801012,   0.00308317359498,   0.00308317359498,   0.00240117801012,   0.00145638808256,	0.00068794901801,   0.00025308230030,   0.00007250929304,
    0.00002077426027,   0.00007250929304,   0.00019710069366,   0.00041726217174,   0.00068794901801,   0.00088334402452,   0.00088334402452,   0.00068794901801,   0.00041726217174,	0.00019710069366,   0.00007250929304,   0.00002077426027};

  double** _g12 = create_matrix<double>(12,12);

  for (int i = 0; i < 12; i++)
    for (int j = 0; j < 12; j++)
      _g12[i][j] = g12[i][j];

  convert2int(_g12, 12, 12, PRECISION, input); // in, size, size, n_shift, out
  free_matrix(_g12, 12, 12);
}

void setup_conv_gaussian_18x18(matrix input)
{
  double g18[18][18]={	0.00000579964060,   0.00001410719343,   0.00003070613946,   0.00005980739310,   0.00010423884362,   0.00016257334987,   0.00022688938707,   0.00028335055443,   0.00031664964772,	0.00031664964772,   0.00028335055443,   0.00022688938707,   0.00016257334987,   0.00010423884362,   0.00005980739310,   0.00003070613946,   0.00001410719343,   0.00000579964060,
    0.00001410719343,   0.00003431469638,   0.00007469039523,   0.00014547702533,   0.00025355321655,   0.00039544755442,   0.00055189152042,   0.00068922910109,   0.00077022666320,	0.00077022666320,   0.00068922910109,   0.00055189152042,   0.00039544755442,   0.00025355321655,   0.00014547702533,   0.00007469039523,   0.00003431469638,   0.00001410719343,
    0.00003070613946,   0.00007469039523,   0.00016257334987,   0.00031664964772,   0.00055189152042,   0.00086074298338,   0.00120126360241,   0.00150019669125,   0.00167649840933,	0.00167649840933,   0.00150019669125,   0.00120126360241,   0.00086074298338,   0.00055189152042,   0.00031664964772,   0.00016257334987,   0.00007469039523,   0.00003070613946,
    0.00005980739310,   0.00014547702533,   0.00031664964772,   0.00061674929795,   0.00107493790130,   0.00167649840933,   0.00233974201070,   0.00292198416382,   0.00326537302163,	0.00326537302163,   0.00292198416382,   0.00233974201070,   0.00167649840933,   0.00107493790130,   0.00061674929795,   0.00031664964772,   0.00014547702533,   0.00005980739310,
    0.00010423884362,   0.00025355321655,   0.00055189152042,   0.00107493790130,   0.00187351894114,   0.00292198416382,   0.00407795740493,   0.00509275249299,   0.00569124802330,	0.00569124802330,   0.00509275249299,   0.00407795740493,   0.00292198416382,   0.00187351894114,   0.00107493790130,   0.00055189152042,   0.00025355321655,   0.00010423884362,
    0.00016257334987,   0.00039544755442,   0.00086074298338,   0.00167649840933,   0.00292198416382,   0.00455719516153,   0.00636007819097,   0.00794277645556,   0.00887620414784,	0.00887620414784,   0.00794277645556,   0.00636007819097,   0.00455719516153,   0.00292198416382,   0.00167649840933,   0.00086074298338,   0.00039544755442,   0.00016257334987,
    0.00022688938707,   0.00055189152042,   0.00120126360241,   0.00233974201070,   0.00407795740493,   0.00636007819097,   0.00887620414784,   0.01108503751106,   0.01238774079633,	0.01238774079633,   0.01108503751106,   0.00887620414784,   0.00636007819097,   0.00407795740493,   0.00233974201070,   0.00120126360241,   0.00055189152042,   0.00022688938707,
    0.00028335055443,   0.00068922910109,   0.00150019669125,   0.00292198416382,   0.00509275249299,   0.00794277645556,   0.01108503751106,   0.01384353655853,   0.01547041608298,	0.01547041608298,   0.01384353655853,   0.01108503751106,   0.00794277645556,   0.00509275249299,   0.00292198416382,   0.00150019669125,   0.00068922910109,   0.00028335055443,
    0.00031664964772,   0.00077022666320,   0.00167649840933,   0.00326537302163,   0.00569124802330,   0.00887620414784,   0.01238774079633,   0.01547041608298,   0.01728848497410,	0.01728848497410,   0.01547041608298,   0.01238774079633,   0.00887620414784,   0.00569124802330,   0.00326537302163,   0.00167649840933,   0.00077022666320,   0.00031664964772,
    0.00031664964772,   0.00077022666320,   0.00167649840933,   0.00326537302163,   0.00569124802330,   0.00887620414784,   0.01238774079633,   0.01547041608298,   0.01728848497410,	0.01728848497410,   0.01547041608298,   0.01238774079633,   0.00887620414784,   0.00569124802330,   0.00326537302163,   0.00167649840933,   0.00077022666320,   0.00031664964772,
    0.00028335055443,   0.00068922910109,   0.00150019669125,   0.00292198416382,   0.00509275249299,   0.00794277645556,   0.01108503751106,   0.01384353655853,   0.01547041608298,   0.01547041608298,   0.01384353655853,   0.01108503751106,   0.00794277645556,   0.00509275249299,   0.00292198416382,   0.00150019669125,   0.00068922910109,   0.00028335055443,
    0.00022688938707,   0.00055189152042,   0.00120126360241,   0.00233974201070,   0.00407795740493,   0.00636007819097,   0.00887620414784,   0.01108503751106,   0.01238774079633,   0.01238774079633,   0.01108503751106,   0.00887620414784,   0.00636007819097,   0.00407795740493,   0.00233974201070,   0.00120126360241,   0.00055189152042,   0.00022688938707,
    0.00016257334987,   0.00039544755442,   0.00086074298338,   0.00167649840933,   0.00292198416382,   0.00455719516153,   0.00636007819097,   0.00794277645556,   0.00887620414784,   0.00887620414784,   0.00794277645556,   0.00636007819097,   0.00455719516153,   0.00292198416382,   0.00167649840933,   0.00086074298338,   0.00039544755442,   0.00016257334987,
    0.00010423884362,   0.00025355321655,   0.00055189152042,   0.00107493790130,   0.00187351894114,   0.00292198416382,   0.00407795740493,   0.00509275249299,   0.00569124802330,   0.00569124802330,   0.00509275249299,   0.00407795740493,   0.00292198416382,   0.00187351894114,   0.00107493790130,   0.00055189152042,   0.00025355321655,   0.00010423884362,
    0.00005980739310,   0.00014547702533,   0.00031664964772,   0.00061674929795,   0.00107493790130,   0.00167649840933,   0.00233974201070,   0.00292198416382,   0.00326537302163,   0.00326537302163,   0.00292198416382,   0.00233974201070,   0.00167649840933,   0.00107493790130,   0.00061674929795,   0.00031664964772,   0.00014547702533,   0.00005980739310,
    0.00003070613946,   0.00007469039523,   0.00016257334987,   0.00031664964772,   0.00055189152042,   0.00086074298338,   0.00120126360241,   0.00150019669125,   0.00167649840933,   0.00167649840933,   0.00150019669125,   0.00120126360241,   0.00086074298338,   0.00055189152042,   0.00031664964772,   0.00016257334987,   0.00007469039523,   0.00003070613946,
    0.00001410719343,   0.00003431469638,   0.00007469039523,   0.00014547702533,   0.00025355321655,   0.00039544755442,   0.00055189152042,   0.00068922910109,   0.00077022666320,   0.00077022666320,   0.00068922910109,   0.00055189152042,   0.00039544755442,   0.00025355321655,   0.00014547702533,   0.00007469039523,   0.00003431469638,   0.00001410719343,
    0.00000579964060,   0.00001410719343,   0.00003070613946,   0.00005980739310,   0.00010423884362,   0.00016257334987,   0.00022688938707,   0.00028335055443,   0.00031664964772,   0.00031664964772,   0.00028335055443,   0.00022688938707,   0.00016257334987,   0.00010423884362,   0.00005980739310,   0.00003070613946,   0.00001410719343,   0.00000579964060};


  double** _g18 = create_matrix<double>(18,18);

  for (int i = 0; i < 18; i++)
    for (int j = 0; j < 18; j++)
      _g18[i][j] = g18[i][j];

  convert2int(_g18, 18, 18, PRECISION, input); // in, size, size, n_shift, out
  free_matrix(_g18, 18, 18);
}

