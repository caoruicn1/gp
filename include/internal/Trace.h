#ifndef INTERNAL_TRACE_H
#define INTERNAL_TRACE_H

#include "macros.h"
#include "internal/ScalarBase.h"
#include "internal/Matrix.h"

#include <Eigen/Core>
#include <type_traits>
#include <typeinfo>

namespace GP {
namespace internal {

template <class Derived> struct traits<Trace<Derived> > {
  typedef typename Derived::scalar_type scalar_type;
  typedef typename Derived::scalar_type result_type;
};

template <typename Derived>
class Trace : public ScalarBase<Trace<Derived> > {
 public:
  // typedefs
  typedef typename traits<Trace>::scalar_type scalar_type;
  typedef typename traits<Trace>::result_type result_type;

 private:
  Derived data_;

 public:
  // constructor
  Trace(const Derived& data) : data_(data) {}

  // actual computation
  result_type get() const { return data_.get().trace(); }

  unsigned get_version() const { return data_.get_version(); }
};
}
}
#endif /* INTERNAL_TRACE_H */
