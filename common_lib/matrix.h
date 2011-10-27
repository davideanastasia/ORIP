/*
 *  matrix.h
 *  ORIP
 *
 *  Created by Davide Anastasia on 22/10/2011.
 *  Copyright 2011 Davide Anastasia. All rights reserved.
 *
 */

#ifndef ORIP_MATRIX
#define ORIP_MATRIX

#include <new>

namespace ORIP
{
  template <typename Type>
  class Matrix {
    //friend std::ostream& operator<<(std::ostream& output, const Matrix& p);
  private:
    int m_rows;
    int m_cols;
    int m_elems;

    Type* m_data;

  public:
    Matrix(int rows, int cols);
    ~Matrix();
    
    Type& operator()(int pos);
    const Type& operator()(int pos) const;
    
    Type& operator()(int row, int col);
    const Type& operator()(int row, int col) const;
    
    int get_rows() const;
    int get_cols() const;
    int get_elems() const;
  };
  
  template<typename Type>
  Matrix<Type>::Matrix(int rows, int cols):
  m_rows(rows), m_cols(cols), m_elems(rows*cols), m_data(new Type[rows*cols])
  {}
  
  template<typename Type>
  Matrix<Type>::~Matrix()
  {
    delete [] m_data;
  }
  
  template<typename Type>
  inline int Matrix<Type>::get_rows() const
  {
    return m_rows;
  }
  
  template<typename Type>
  inline int Matrix<Type>::get_cols() const
  {
    return m_cols;
  }
  
  template<typename Type>
  inline int Matrix<Type>::get_elems() const
  {
    return m_elems;
  }
  
  template<typename Type>
  inline Type& Matrix<Type>::operator()(int pos)
  {
    return m_data[ pos ];
  }

  template<typename Type>
  inline const Type& Matrix<Type>::operator()(int pos) const
  {
    return m_data[ pos ];
  }
  
  template<typename Type>
  inline Type& Matrix<Type>::operator()(int row, int col)
  {
    return m_data[ row*m_cols + col ];
  }

  template<typename Type>
  inline const Type& Matrix<Type>::operator()(int row, int col) const
  {
    return m_data[ row*m_cols + col ];
  }

#include <iomanip>

  template<typename T>
  std::ostream &operator<<( std::ostream &out, const ORIP::Matrix<T>& M )
  {
    using namespace std;
    const std::streamsize _width = 7;

    out.precision(3);
    cout.fill(' ');
    out.setf(ios::fixed);

    out << "[";
    for (int i = 0; i < M.get_rows()-1; i++)
    {
        for (int j=0; j < M.get_cols()-1; j++)
        {
            out << setw(_width) << M(i, j) << ",";
        }
        out << setw(_width) << M(i, M.get_cols()-1) << ";";
        out << "\n";
    }

    for (int j=0; j < M.get_cols()-1; j++)
    {
        out << setw(_width) << M(M.get_rows()-1, j) << ",";
    }
    out << setw(_width) << M(M.get_rows()-1, M.get_cols()-1);
    out << "]" << endl;

    out.unsetf(ios::floatfield);
    return out;
  }

};



#endif // ORIP_MATRIX
