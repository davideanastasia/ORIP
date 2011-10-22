/*
 *  matrix.h
 *  ORIP
 *
 *  Created by Davide Anastasia on 22/10/2011.
 *  Copyright 2011 Davide Anastasia. All rights reserved.
 *
 */

#include <new>

namespace ORIP
{
  template <class Type>
  class Matrix {
  private:
    Type* m_data;
    int m_elems;
    int m_rows;
    int m_cols;
    
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
  
  template<class Type>
  Matrix<Type>::Matrix(int rows, int cols):
  m_rows(rows), m_cols(cols), m_elems(rows*cols)
  {
    m_data = new Type[m_elems];
  }
  
  template<class Type>
  Matrix<Type>::~Matrix()
  {
    delete [] m_data;
  }
  
  template<class Type>
  inline int Matrix<Type>::get_rows() const
  {
    return m_rows;
  }
  
  template<class Type>
  inline int Matrix<Type>::get_cols() const
  {
    return m_cols;
  }
  
  template<class Type>
  inline int Matrix<Type>::get_elems() const
  {
    return m_elems;
  }
  
  template<class Type>
  inline Type& Matrix<Type>::operator()(int pos)
  {
    return m_data[ pos ];
  }

  template<class Type>
  inline const Type& Matrix<Type>::operator()(int pos) const
  {
    return m_data[ pos ];
  }
  
  template<class Type>
  inline Type& Matrix<Type>::operator()(int row, int col)
  {
    return m_data[ row*m_cols + col ];
  }

  template<class Type>
  inline const Type& Matrix<Type>::operator()(int row, int col) const
  {
    return m_data[ row*m_cols + col ];
  }
};
