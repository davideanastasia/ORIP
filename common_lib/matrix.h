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
  template <typename Type>
  class Matrix {
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
};
