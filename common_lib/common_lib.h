/* ORIP_v2 –> Op Ref Im Proc -> Oper Refin Imag Process -> Operational Refinement of Image Processing
 *
 * Copyright Davide Anastasia and Yiannis Andreopoulos, University College London
 *
 * http://www.ee.ucl.ac.uk/~iandreop/ORIP.html
 * If you have not retrieved this source code via the above link, please contact danastas@ee.ucl.ac.uk and iandreop@ee.ucl.ac.uk
 * 
 */
#ifndef __COMMON_LIB_H_
#define __COMMON_LIB_H_

#define _CRT_SECURE_NO_DEPRECATE
#define TAB                   "\t"
#define BITMASK(in,b)         ((in >> b) & 1)  
#define STOREBITPLANE_R(in,b) ((FROUND(in) << b))  
#define STOREBITPLANE(in,b)   ((in << b))
#define ZERO_DOT_FIVE         (0.5)
#define PRINT                 (8)

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <limits.h>

#if defined(_WIN32) || defined(__CYGWIN__)
#include <windows.h>
#endif

using namespace std;

extern int _MASK;
extern int _SHIFT;

// the third parameter of from_string() should be 
// one of std::hex, std::dec or std::oct
template <class T>
bool from_string(T& t, 
                 const std::string& s, 
                 std::ios_base& (*f)(std::ios_base&))
{
  std::istringstream iss(s);
  return !(iss >> f >> t).fail();
}

// --- DECLARATION ---
void test_integer();
void set_mask(int bitplanes);
int bitcount(unsigned int n);

void shift_matrix(double** in, double** out, int _col, int _row, int shift);
void mask_matrix(double** input, double** output, int _col, int _row);  // template for int types

void start_high_priority();
void exit_high_priority();
// -------------------

// --- INLINE FUNCTIONS ---
// round for positive number only
//#define PROUND(i) ( int(i + 0.5) ) 
// round for any number (heavier than PROUND?)
//#define FROUND(i) ( (i >= 0) ? int(i + 0.5) : int(i - 0.5) )

inline long fast_round(double i)
{
  return (long)(i + 0.5);
}

inline long full_round(double i)
{
  return ((i >= 0) ? (long)(i + ZERO_DOT_FIVE) : (long)(i - ZERO_DOT_FIVE));
}
// -------------------------

// --- TEMPLATE FUNCTIONS ---
template <class T>
void fill_matrix(T** matrix, int _col, int _row, FILE* ifile)
{
  // read YUV
  // read Y
  unsigned char temp;

  for (int row = 0; row < _row; ++row) // row
  {
    for (int col = 0; col < _col; ++col) // col
    {
				fread(&temp, sizeof(unsigned char), 1, ifile);
 				matrix[row][col] = ((T)temp);
    }
  }

  // read UV ... and throw it away! :)
  char *uv_frame_buffer = new char[_col*(_row/2)];
  fread(uv_frame_buffer, sizeof(char), _col*(_row/2), ifile);
  delete uv_frame_buffer;

  cout << "." << flush;
}

template <class T>
void fill_matrix_T(T** matrix, int _col, int _row, FILE* ifile)
{
  // read YUV
  // read Y
  //T temp;

  for (int row = 0; row < _row; ++row) // row
  {
    for (int col = 0; col < _col; ++col) // col
    {
				fread(&matrix[row][col], sizeof(T), 1, ifile);
    }
  }

  // read UV ... and throw it away! :)
  T* uv_frame_buffer = new T[_col*(_row/2)];
  fread(uv_frame_buffer, sizeof(T), _col*(_row/2), ifile);
  delete uv_frame_buffer;

  cout << "." << flush;
}

template <class T>
void fill_matrix(T** matrix, int _col, int _row, ifstream& ifile)
{
  // read YUV
  // read Y
  unsigned char temp;

  for (int row = 0; row < _row; ++row) // row
  {
    for (int col = 0; col < _col; ++col) // col
    {
      ifile >> temp;
      matrix[row][col] = (T)temp;
    }
  }

  // read UV ... and throw it away! :)
  char* uv_frame_buffer = new char[_col*(_row/2)];
  ifile.read(uv_frame_buffer, _col*(_row/2));
  delete uv_frame_buffer;

  cout << "." << flush;
}

template <class T>
void write_matrix(T** matrix, int _col, int _row, FILE* ifile)
{
  // read YUV
  // read Y
  unsigned char temp;

  for (int row = 0; row < _row; ++row) // row
  {
    for (int col = 0; col < _col; ++col) // col
    {
        temp = ((unsigned char)matrix[row][col]);
				fwrite(&temp, sizeof(unsigned char), 1, ifile);
    }
  }

  // read UV ... and throw it away! :)
  char *uv_frame_buffer = new char[_col*(_row/2)];
  for (int i=0; i < _col*(_row/2); i++) uv_frame_buffer[i] = 0; // put zeros everywhere!
  fwrite(uv_frame_buffer, sizeof(char), _col*(_row/2), ifile);
  delete uv_frame_buffer;
}

/*
 * May 07, 2009: move everything to long, double too!
 */
template <class T>
void write_matrix_full(T** matrix, int _col, int _row, FILE* ifile)
{
  // write Y
  T temp;
  for (int row = 0; row < _row; ++row) // row
  {
    for (int col = 0; col < _col; ++col) // col
    {
        temp = ((T)matrix[row][col]);
				fwrite(&temp, sizeof(T), 1, ifile);
    }
  }

  // write UV ... all zeros!
  static long *uv_frame_buffer = new long[_col*(_row/2)];
  for (int i=0; i < _col*(_row/2); i++) uv_frame_buffer[i] = 0; // put zeros everywhere!
  fwrite(uv_frame_buffer, sizeof(long), _col*(_row/2), ifile);

  //delete uv_frame_buffer;
  //delete temp;
  //fflush(ifile);
}

template <class T>
void transpose_matrix(T** in, T** out, int size)
{
  // in and out are two square matrix of size * size
  for (int row = 0; row < size; row++) // row
  {
    for (int col = 0; col < size; col++) // col
    {
      out[col][row] = in[row][col];
    }
  }
}

template <class T>
T** create_matrix(int _col, int _row)
{
  // init memory
  T** mem = new T*[_row];
  for (int k=0; k < _row; k++)
  {
    mem[k] = new T[_col];
  }

  // reset memory
  for (int i = 0; i < _row; i++)
    for (int j = 0; j < _col; j++)
      mem[i][j] = 0;

  return mem;
}

template <class T>
void free_matrix(T** in, int _col, int _row)
{
  for (int k = 0; k < _row; k++)
  {
    delete in[k];
  }

  delete in;
}


template <class T>
void print_matrix(T** matrix, int _col, int _row)
{
  cout << endl;
  for (int i = 0 ; i < _row; i++)
  {
    for ( int j = 0 ; j < _col; j++)
    {
      if ( matrix[i][j] == INT_MAX )
        cout << setw(6) << ".";
      else
        cout << setw(6) << matrix[i][j];
    }
    cout << endl;
  }
}

template <class T>
void print_matrix_csv(T** matrix, int _col, int _row)
{
  //cerr << endl;
  for (int i = 0 ; i < _row; i++)
  {
    for ( int j = 0 ; j < _col; j++)
    {
        cerr << setiosflags(ios::fixed) << setprecision(10) << matrix[i][j];
        if (j != _col-1) cerr << ",";
    }
    cerr << endl;
  }
}


template <class T>
void print_matrix(T** matrix, int c_size, int r_size, int print_col, int print_row)
{
  for (int i = r_size - print_row ; i < r_size; i++)
  {
    for (int j = c_size - print_col ; j < c_size; j++)
    {
      cout << setw(6) << matrix[i][j];
    }
    cout << endl;
  }
}

template <class T>
void reset_matrix(T** mat, int _col, int _row)
{
  // reset memory
  for (int i = 0; i < _row; i++)
    for (int j = 0; j < _col; j++)
      mat[i][j] = 0;
}

template <class T>
bool check_matrix_int(T** matrix1, T** matrix2, int _col, int _row)
{
  bool result = true;

  int t_matrix1 = 0;
  int t_matrix2 = 0;

  for (int i = 0; i < _row; i++)
  {
    for (int j = 0; j < _col; j++)
    {
      t_matrix1 = (my_round(matrix1[i][j]));
      t_matrix2 = (my_round(matrix2[i][j]));

      if ( t_matrix1 != t_matrix2 ) 
      {
        result = false;
      }
    }
  }

  return result;
}

template <class T>
bool check_matrix(T** matrix1, T** matrix2, int _col, int _row)
{
  bool result = true;
  for (int i = 0; i < _row; i++)
    for (int j = 0; j < _col; j++)
      if ( matrix1[i][j] != matrix2[i][j] ) result = false;

  return result;
}

template <class T>
void copy_matrix(T** in, T** out, int _col, int _row)
{
  // in and out are two square matrix of size * size
  for (int row = 0; row < _row; row++) // row
  {
    for (int col = 0; col < _col; col++) // col
    {
      out[row][col] = in[row][col];
    }
  }
}


template <class T>
void shift_matrix(T** input, T** output, int _col, int _row, int shift)
{
  for (int row = 0; row < _row; row++) // row
  {
    for (int col = 0; col < _col; col++) // col
    {
      output[row][col] = (input[row][col] << shift);
    }
  }
}

template <class T, class O>
void extract_bitplane_matrix_i(T** input, O** output, int _col, int _row, int bitplane, int bits)
{
  int shift = bitplane - bits + 1;
  long mask = long( pow(2.0, bits) - 1 );

  for (int row = 0; row < _row; row++) // row
  {
    for (int col = 0; col < _col; col++) // col
    {
      output[row][col] = ( input[row][col] >> shift ) & mask;
    }
  }
}

template <class T, class O>
void extract_bitplane_matrix(T** input, O** output, int _col, int _row, int bitplane)
{
  for (int row = 0; row < _row; row++) // row
  {
    for (int col = 0; col < _col; col++) // col
    {
      output[row][col] = BITMASK(input[row][col], bitplane);
    }
  }
}

template <class T>
void mask_matrix(T** input, T** output, int _col, int _row)
{
  for (int row = 0; row < _row; row++) // row
  {
    for (int col = 0; col < _col; col++) // col
    {
      output[row][col] = input[row][col] & _MASK;
    }
  }
}

/* This fuction is allowed only for double matrixes */
//template <class T>
void mask_matrix_and_shift(double** input, double** output, int _col, int _row);
//{
//  for (int row = 0; row < _row; row++) // row
//  {
//    for (int col = 0; col < _col; col++) // col
//    {
//      output[row][col] = floor(input[row][col] / pow(2.0, _SHIFT));
//    }
//  }
//}

template <class T>
void mask_matrix_and_shift(T** input, T** output, int _col, int _row)
{

  for (int row = 0; row < _row; row++) // row
  {
    for (int col = 0; col < _col; col++) // col
    {
      output[row][col] = (input[row][col] & _MASK) >> _SHIFT;
    }
  }
}

template <class T>
T get_max_matrix(T** input, int _col, int _row)
{
  T _max = INT_MIN;
  for (int row = 0; row < _row; row++) // row
  {
    for (int col = 0; col < _col; col++) // col
    {
      if ( input[row][col] > _max ) _max = input[row][col];
    }
  }

  return _max;
}

template <class T>
T get_min_matrix(T** input, int _col, int _row)
{
  T _min = INT_MAX;
  for (int row = 0; row < _row; row++) // row
  {
    for (int col = 0; col < _col; col++) // col
    {
      if ( input[row][col] < _min ) _min = input[row][col];
    }
  }

  return _min;
}

template <class T>
void pswap(T* pa, T* pb)
{
  T pt = *pa;
  *pa = *pb;
  *pb = pt;
}

template <class T>
//void convert2int(double **mat, int rows, int cols, int n_shift, T **gauss)
void convert2int(double **mat, int rows, int cols, int n_shift, T **gauss)
{
  int i,j;

  for (i=0; i<rows; i++)
  {
    for (j=0; j<cols; j++)
    {
      gauss[i][j] = (T)floor( mat[i][j] * pow((double)2,(double)n_shift)+0.49999 );
    }
  }
}

// take a file name and return a full matrix of the correct size with the content of the file
template<class T>
T** read_matrix_from_file(string filename, int &col, int &row )
{
  ifstream k_file;
  string this_line;
  int k_row = 0;
  int k_col = 0;

  // open the matrix file
  k_file.open(filename.c_str());
  if ( !k_file.is_open() )
  {
    return (NULL);  // not able to read the file
  }

  getline( k_file, this_line, '\n' );
  // read matrix dimension
  string::size_type lastPos = this_line.find_first_not_of(",", 0);    // Skip delimiters at beginning
  string::size_type pos     = this_line.find_first_of(",", lastPos);  // Find first "non-delimiter"

  // read ROW first
  from_string<int>(k_col, this_line.substr(lastPos, pos - lastPos), std::dec ); // Found a token, add it to the vector.
  col = k_col;
  
  // NEXT
  lastPos = this_line.find_first_not_of(",", pos);    // Skip delimiters.  Note the "not_of"
  pos = this_line.find_first_of(",", lastPos);        // Find next "non-delimiter"

  // read ROW first
  from_string<int>(k_row, this_line.substr(lastPos, pos - lastPos), std::dec ); // Found a token, add it to the vector.
  row = k_row;

  // create matrix
  T** matrix = new T*[k_row];
  for (int k=0; k < k_row; k++)
  {
    matrix[k] = new T[k_col];
  }

  // set everything to zero
  for (int i = 0; i < k_row; i++)
    for (int j = 0; j < k_col; j++)
      matrix[i][j] = 0;

  // read line by line and store
  int i=0;
  int j=0;

  while ( !k_file.eof() )
  {
    getline( k_file, this_line, '\n' );

    lastPos = this_line.find_first_not_of(",", 0);    // Skip delimiters.  Note the "not_of"
    pos = this_line.find_first_of(",", lastPos);        // Find next "non-delimiter"

    while (string::npos != pos || string::npos != lastPos)
    {
      from_string<T>(matrix[i][j], this_line.substr(lastPos, pos - lastPos), std::dec ); // Found a token, add it to the vector.

      lastPos = this_line.find_first_not_of(",", pos);    // Skip delimiters.  Note the "not_of"
      pos = this_line.find_first_of(",", lastPos);        // Find next "non-delimiter"

      j++;
    }

    i++; j = 0;
  }

  // close file
  k_file.close();

  // return matrix
  return matrix;
}


#endif  //__COMMON_LIB_H_

