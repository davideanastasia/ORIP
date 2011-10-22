/* ORIP_v2 �> Op Ref Im Proc -> Oper Refin Imag Process -> Operational Refinement of Image Processing
 *
 * Copyright Davide Anastasia and Yiannis Andreopoulos, University College London
 *
 * http://www.ee.ucl.ac.uk/~iandreop/ORIP.html
 * If you have not retrieved this source code via the above link, please contact danastas@ee.ucl.ac.uk and iandreop@ee.ucl.ac.uk
 * 
 */


// [row][col]

#define PRECISION 8
#define PRECISION_6 6

#include "common_lib.h"
#include "library.h"

// boolean for threading cooperation
#ifdef _OPENMP
bool flag_int = false;
bool flag_scheduler = true;
#endif
// ---

using namespace std;

void conv2(matrix mat1, int rows1, int cols1, matrix mat2, int rows2, int cols2, matrix res)
{
  my_t conv_t;
  int i, j, ic, jc;
  int offset_rows, offset_cols;

  static matrix mat1_t = create_matrix<my_t>(cols1 + cols2 - 1, rows1 + rows2 - 1);

  if ( rows2%2 == 0 ) offset_rows = rows2/2-1; else offset_rows = rows2/2;
  if ( cols2%2 == 0 ) offset_cols = cols2/2-1; else offset_cols = cols2/2;

  for ( i = 0; i < rows1; i++ )
  {
    for ( j = 0; j < cols1; j++ )
    {
      mat1_t[offset_rows + i][offset_cols + j] = mat1[i][j];
      res[i][j] = 0;
    }
  }

  for (i = 0; i < rows1; i++)
  {

#ifdef _OPENMP
    if (flag_int == true) return;     // stop function execution
#endif

    for (j = 0; j < cols1; j++)
    {
      for (ic = 0; ic < rows2; ic++)
      {
        for (jc = 0; jc < cols2; jc++)
        {
          conv_t = ( mat1_t[i+ic][j+jc] * mat2[rows2-1-ic][cols2-1-jc] );
          res[i][j] += conv_t; // res[i][j] = res[i][j] + conv_t;
        }
      }
    }
  }
}

// it works on symmetric kernels
void conv2_symmetric(matrix mat1, int rows1, int cols1, matrix mat2, int rows2, int cols2, matrix res)
{
  my_t conv_t = 0;
  int i, j, ic, jc;
  int offset_rows, offset_cols;

  static matrix mat1_t = create_matrix<my_t>(cols1 + cols2 - 1, rows1 + rows2 - 1);

  if ( rows2%2 == 0 ) offset_rows = rows2/2-1; else offset_rows = rows2/2;
  if ( cols2%2 == 0 ) offset_cols = cols2/2-1; else offset_cols = cols2/2;

  for ( i = 0; i < rows1; i++ )
  {
    for ( j = 0; j < cols1; j++ )
    {
      mat1_t[offset_rows+i][offset_cols+j] = mat1[i][j];
      res[i][j] = 0;
    }
  }

  int rows2_t = rows2/2;  // if the kernel matrix is symmetric, rows2 is pair
  int cols2_t = cols2/2;   // same as above

  int max_ic = rows2 - 1;
  int max_jc = cols2 - 1;

  my_t elem_t = 0;

  for (i = 0; i < rows1; i++)
  {

#ifdef _OPENMP
    if (flag_int == true) return;
#endif

    for (j = 0; j < cols1; j++)
    {
      for (ic = 0; ic < rows2_t; ic++)
      {
        for (jc = 0; jc < cols2_t; jc++)
        {
          elem_t = mat1_t[i+ic][j+jc] + mat1_t[i+(max_ic - ic)][j + (max_jc - jc)]
          +mat1_t[i + ic][j + (max_jc - jc)] +mat1_t[i+(max_ic - ic)][j + jc];

          conv_t = elem_t * mat2[rows2-1-ic][cols2-1-jc];
          res[i][j] += conv_t; // res[i][j] = res[i][j] + conv_t;
        }
      }
    }
  }
}

void setup_conv_matrix(matrix input, int col, int row, int max_val)
{
  srand ( 0 );

  for (int i=0; i < row; i++)
  {
    for ( int j=0; j < col; j++)
    {
      input[i][j] = (rand() % max_val); // - (max_val/2);
    }
  }
}

void setup_conv_gaussian_6x6(matrix input)
{
  double g6[6][6]={ 0.00030833651364,   0.00227831579665,   0.00619310442954,   0.00619310442954,   0.00227831579665,   0.00030833651364,
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
