#ifndef SCALAR_BUILTIN_SUM_H
#define SCALAR_BUILTIN_SUM_H

#include "macros.h"
#include "internal/ScalarBase.h"
#include "internal/Scalar.h"

#include <Eigen/Core>
#include <type_traits>

namespace GP {
namespace internal {

// specialize traits for ScalarBuiltinSum
template <class Lhs> struct traits<ScalarBuiltinSum<Lhs> > {
  static_assert(std::is_convertible<typename Lhs::scalar_type, double>::value,
                "cannot mix different scalar types");
  typedef double scalar_type;
  typedef double result_type;
};

// expression template for Scalar + Builtin
template <typename Lhs>
class ScalarBuiltinSum : public ScalarBase
                                <ScalarBuiltinSum<Lhs> > {
 private:
  const Lhs& lhs_;
  double rhs_;

 public:
  typedef typename traits
      <ScalarBuiltinSum<Lhs> >::scalar_type scalar_type;
  typedef typename traits
      <ScalarBuiltinSum<Lhs> >::result_type result_type;

 public:
  // constructor
  ScalarBuiltinSum(const Lhs& lhs, double rhs) : lhs_(lhs), rhs_(rhs) {}

  // actual computation
  result_type get() const { return lhs_.get() + rhs_; }

  unsigned get_version() const { return lhs_.get_version(); }
};
}
}
#endif /* SCALAR_BUILTIN_SUM_H */
