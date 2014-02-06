#ifndef INTERNAL_MATRIX_SUM_H
#define INTERNAL_MATRIX_SUM_H

#include "macros.h"
#include "internal/MatrixBase.h"

#include <Eigen/Core>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace GP {
namespace internal {

template <class Lhs, class Rhs> struct traits<MatrixSum<Lhs, Rhs> > {
  static_assert(std::is_same
                <typename Lhs::scalar_type, typename Rhs::scalar_type>::value,
                "cannot mix matrices of different scalar types");
  static_assert(Lhs::RowsAtCompileTime == Eigen::Dynamic
                || Rhs::RowsAtCompileTime == Eigen::Dynamic
                || Lhs::RowsAtCompileTime == Rhs::RowsAtCompileTime,
                "Matrices must have same number of rows!");
  static_assert(Lhs::ColsAtCompileTime == Eigen::Dynamic
                || Rhs::ColsAtCompileTime == Eigen::Dynamic
                || Lhs::ColsAtCompileTime == Rhs::ColsAtCompileTime,
                "Matrices must have same number of columns!");
  typedef typename Lhs::scalar_type scalar_type;
  typedef decltype(std::declval<typename Lhs::result_type>() + std::declval
                   <typename Rhs::result_type>()) result_type;
  enum {
    RowsAtCompileTime = Lhs::RowsAtCompileTime,
    ColsAtCompileTime = Lhs::ColsAtCompileTime
  };
};

template <typename Lhs, typename Rhs>
class MatrixSum : public MatrixBase<MatrixSum<Lhs, Rhs> > {
 public:
  // typedefs
  typedef typename traits<MatrixSum>::scalar_type scalar_type;
  typedef typename traits<MatrixSum>::result_type result_type;
  enum {
    RowsAtCompileTime = traits<MatrixSum>::RowsAtCompileTime,
    ColsAtCompileTime = traits<MatrixSum>::ColsAtCompileTime
  };

 private:
  Lhs lhs_;
  Rhs rhs_;

 public:
  // constructor
  MatrixSum(const Lhs& lhs, const Rhs& rhs) : lhs_(lhs), rhs_(rhs) {}

  // actual computation
  result_type get() const { return lhs_.get() + rhs_.get(); }

  unsigned get_version() const {
    return lhs_.get_version() + rhs_.get_version();
  }
};
}
}
#endif /* INTERNAL_MATRIX_SUM_H */
