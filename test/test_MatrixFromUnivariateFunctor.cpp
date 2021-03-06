#include "Scalar.h"
#include "Matrix.h"
#include "Functor.h"

#include <Eigen/Dense>
#include <iostream>

using namespace GP;

int main(int, char*[]){
    //Functor taking row vector as input
    RowVectorXd x(Eigen::RowVectorXd::LinSpaced(5,0,1));
    Scalar y(2.0);
    auto f1 = make_functor(x*y, x);
    MatrixXd inmat(Eigen::MatrixXd::Random(3,5));
    auto m1 = MatrixXd::Apply(f1, inmat);
    if ((m1.get() - y.get()*inmat.get()).norm() >1e-5) return 1;
    //Functor taking scalar as input
    auto f2 = make_functor(y,y);
    VectorXd z(Eigen::VectorXd::LinSpaced(5,0,1));
    auto m2 = VectorXd::Apply(f2, z);
    if (m2.get() != z.get()) return 2;
    if ((m2*3).get() != z.get()*3) return 3;
    return 0;
}
