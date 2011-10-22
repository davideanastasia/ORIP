/* ORIP_v2 –> Op Ref Im Proc -> Oper Refin Imag Process -> Operational Refinement of Image Processing
 *
 * Copyright Davide Anastasia and Yiannis Andreopoulos, University College London
 *
 * http://www.ee.ucl.ac.uk/~iandreop/ORIP.html
 * If you have not retrieved this source code via the above link, please contact danastas@ee.ucl.ac.uk and iandreop@ee.ucl.ac.uk
 * 
 */


#include "common_lib.h"

int _MASK = 0xFF;
int _SHIFT = 0x00;

void set_mask(int bitplanes)
{
  switch(bitplanes)
  {
  case 0:
    {
    _MASK = 0x00;
    _SHIFT = 0;
    }
    break;
  case 1:{
    _MASK = 0x80;
    _SHIFT = 7;
         }
    break;
  case 2:{
    _MASK = 192;
    _SHIFT = 6;
         }
    break;
  case 3:{
    _MASK = 224;
    _SHIFT = 5;
         }
    break;
  case 4:{
    _MASK = 240;
    _SHIFT = 4;
         }
    break;
  case 5:{
    _MASK = 248;
    _SHIFT = 3;
         }
    break;
  case 6:{
    _MASK = 252;
    _SHIFT = 2;
         }
    break;
  case 7:{
    _MASK = 254;
    _SHIFT = 1;
         }
    break;
  case 8:
  default:{
    _MASK = 0xFF; // 255
    _SHIFT = 0;
          }
    break;
  }
}

void shift_matrix(double** in, double** out, int _col, int _row, int shift)
{
  for (int row = 0; row < _row; row++) // row
  {
    for (int col = 0; col < _col; col++) // col
    {
      out[row][col] = (in[row][col] * pow(2.0, shift));
    }
  }
}

int bitcount(unsigned int n)
{
   /* works for 32-bit numbers only    */
   /* fix last line for 64-bit numbers */

   register unsigned int tmp;

   tmp = n - ((n >> 1) & 033333333333)
           - ((n >> 2) & 011111111111);
   return ((tmp + (tmp >> 3)) & 030707070707) % 63;
}

void mask_matrix(double** input, double** output, int _col, int _row)
{
  long t_;

  for (int row = 0; row < _row; row++) // row
  {
    for (int col = 0; col < _col; col++) // col
    {
      t_ = (long)(input[row][col]) & _MASK;
      output[row][col] = double(t_);
    }
  }
}

void mask_matrix_and_shift(double** input, double** output, int _col, int _row)
{
  for (int row = 0; row < _row; row++) // row
  {
    for (int col = 0; col < _col; col++) // col
    {
      output[row][col] = floor(input[row][col] / pow(2.0, _SHIFT));
    }
  }
}

void start_high_priority()
{
#if defined(_WIN32) || defined(__CYGWIN__)
  if(!SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS))
  {
    cerr << "[!] Failed to enter HIGH_PRIORITY_CLASS (" << GetLastError() <<" )" << endl;
  } 

  cout << " Current priority class is " << GetPriorityClass(GetCurrentProcess()) << endl;
#endif
}

void exit_high_priority()
{
#if defined(_WIN32) || defined(__CYGWIN__)
  if(!SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS))
  {
    cerr << "[!] Failed to exit HIGH_PRIORITY_CLASS (" << GetLastError() << ")" << endl;
  }

  //cout << " Current priority class is " << GetPriorityClass(GetCurrentProcess()) << endl;
#endif
}
