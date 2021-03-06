#ifndef INTERNAL_MATRIX_BASE_H
#define INTERNAL_MATRIX_BASE_H

#include "macros.h"
#include "internal/ForwardDeclarations.h"
#include "Matrix.h"

#include <functional>

namespace GP {
namespace internal {

//! Base class for any matrix
template <class Derived> class MatrixBase : public GPBase<Derived> {

 protected:
  MatrixBase() {}  // only children can instantiate it

 public:
  // allow implicit casting to Derived
  const Derived& asDerived() const {
    return static_cast<const Derived&>(*this);
  }

  // rows and columns
  unsigned rows() const { return asDerived().get().rows(); }
  unsigned cols() const { return asDerived().get().cols(); }

  // conversion of 1x1 matrix to Scalar
  ScalarFromMatrix<Derived> scalar() const { return asDerived(); }

  // trace
  UnaryExpr<op::Trace, Derived> trace() const {
    return UnaryExpr<op::Trace, Derived>(asDerived());
  }

  // transposition
  UnaryExpr<op::Transpose, Derived> transpose() const {
    return UnaryExpr<op::Transpose, Derived>(asDerived());
  }

  // decompose matrix, use Cholesky LDLT decomposition by default
  template <template <class> class Policy = LDLTPolicy>
  Decomposition<Derived, Policy> decomposition() const {
    return Decomposition<Derived, Policy>(asDerived());
  }

  //! build diagonal matrix from vector
  UnaryExpr<op::DiagonalMatrixFromVector, Derived> asDiagonal() {
    return UnaryExpr<op::DiagonalMatrixFromVector, Derived>(asDerived());
  }

  //! Yield a matrix by applying a univariate function to every row of an input
  // matrix. Will not check whether func is compatible with the rows of the
  // matrix. Will only compile if the function returns a row vector
  template <class Functor, class InMat>
  static MatrixFromUnivariateFunctor<Functor, InMat> Apply(const Functor& func,
                                                           const InMat& mat) {
    return MatrixFromUnivariateFunctor<Functor, InMat>(func, mat);
  }

  //! Yield a matrix by applying a bivariate function to every row of two input
  // matrices. Will not check whether func is compatible with the rows of the
  // matrix. Will only compile if the function returns a scalar, or 1x1 matrix.
  template <class Functor, class InMat1, class InMat2>
  static MatrixFromBivariateFunctor<Functor, InMat1, InMat2> Apply(
      const Functor& func, const InMat1& mat1, const InMat2& mat2) {
    return MatrixFromBivariateFunctor
        <Functor, InMat1, InMat2>(func, mat1, mat2);
  }

  //! Yield a matrix by applying a bivariate function to every pair of rows of
  // an input matrix.
  // will not check whether func is compatible with the rows of the matrix
  // will only compile if the function returns a double or a 1x1 matrix
  template <class Functor, class InMat>
  static SymmetricMatrixFromBivariateFunctor<Functor, InMat> SymmetricApply(
      const Functor& func, const InMat& mat) {
    return SymmetricMatrixFromBivariateFunctor<Functor, InMat>(func, mat);
  }

  //! Broadcast a constant Scalar expression to a matrix/vector
  //\see declaration in
  template <class ScalarExpr>
  static MatrixFromScalar<ScalarExpr, Derived> Broadcast(const ScalarBase
                                                         <ScalarExpr>& scal,
                                                         unsigned nrows,
                                                         unsigned ncols) {
    return MatrixFromScalar
        <ScalarExpr, Derived>(scal.asDerived(), nrows, ncols);
  }
};

// sums
//  double Matrix
template <class MatrixExpression>
const BinaryExpr<op::Sum, ConstScalar, ScalarFromMatrix<MatrixExpression> >
operator+(double lhs, const MatrixBase<MatrixExpression>& rhs) {
  return BinaryExpr<op::Sum, ConstScalar, ScalarFromMatrix<MatrixExpression> >(
      lhs, rhs.asDerived());
}
//  Matrix double
template <class MatrixExpression>
const BinaryExpr<op::Sum, ScalarFromMatrix<MatrixExpression>, ConstScalar>
operator+(const MatrixBase<MatrixExpression>& lhs, double rhs) {
  return BinaryExpr<op::Sum, ScalarFromMatrix<MatrixExpression>, ConstScalar>(
      lhs.asDerived(), rhs);
}

// differences
//  double Matrix
template <class MatrixExpression>
const BinaryExpr
    <op::Difference, ConstScalar, ScalarFromMatrix<MatrixExpression> >
operator-(double lhs, const MatrixBase<MatrixExpression>& rhs) {
  return BinaryExpr
      <op::Difference, ConstScalar, ScalarFromMatrix<MatrixExpression> >(
          lhs, rhs.asDerived());
}
//  Matrix double
template <class MatrixExpression>
const BinaryExpr
    <op::Difference, ScalarFromMatrix<MatrixExpression>, ConstScalar>
operator-(const MatrixBase<MatrixExpression>& lhs, double rhs) {
  return BinaryExpr
      <op::Difference, ScalarFromMatrix<MatrixExpression>, ConstScalar>(
          lhs.asDerived(), rhs);
}

// products
//  Matrix Matrix
template <class Lhs, class Rhs>
const BinaryExpr<op::MatrixProduct, Lhs, Rhs> operator*(const MatrixBase
                                                        <Lhs>& lhs,
                                                        const MatrixBase
                                                        <Rhs>& rhs) {
  return BinaryExpr
      <op::MatrixProduct, Lhs, Rhs>(lhs.asDerived(), rhs.asDerived());
}
//  double Matrix
template <class Rhs>
const BinaryExpr<op::Product, ConstScalar, Rhs> operator*(double lhs,
                                                          const MatrixBase
                                                          <Rhs>& rhs) {
  return BinaryExpr<op::Product, ConstScalar, Rhs>(lhs, rhs.asDerived());
}
//  Matrix double
template <class Lhs>
const BinaryExpr<op::Product, Lhs, ConstScalar> operator*(const MatrixBase
                                                          <Lhs>& lhs,
                                                          double rhs) {
  return BinaryExpr<op::Product, Lhs, ConstScalar>(lhs.asDerived(), rhs);
}

//    Matrix / double
template <class Lhs>
const BinaryExpr<op::Quotient, Lhs, ConstScalar> operator/(const MatrixBase
                                                           <Lhs>& lhs,
                                                           double rhs) {
  return BinaryExpr<op::Quotient, Lhs, ConstScalar>(lhs.asDerived(), rhs);
}
}
}
#endif /* INTERNAL_MATRIX_BASE_H */
