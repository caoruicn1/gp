#ifndef VECTOR_FROM_FUNCTION_H
#define VECTOR_FROM_FUNCTION_H

#include <Eigen/Dense>

//! map the rows of INMATRIX through UNIFUNC to a VECTOR
template <class INMATRIX, class UNIFUNC>
class VectorFromFunction {

   public:
    VectorFromFunction(INMATRIX X, UNIFUNC mu) : X_(X), mu_(mu) {}

    typedef Eigen::VectorXd result_type;
    const result_type& get() const { return build_vector(); }

   private:
    const result_type& build_vector() const {
        auto Xmat(X_.get());
        retval_ = Eigen::VectorXd(Xmat.rows());
        for (unsigned i = 0; i < Xmat.rows(); i++)
            retval_(i) = mu_.eval(Xmat.row(i));
        return retval_;
    }

   private:
    INMATRIX X_;
    UNIFUNC mu_;
    mutable result_type retval_;

};

#endif /* VECTOR_FROM_FUNCTION */
