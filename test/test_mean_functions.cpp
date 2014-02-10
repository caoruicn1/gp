#include "mean_functions.h"
#include "Matrix.h"

#include <Eigen/Dense>

int main(){
    auto zf = GP::mean::zero();
    GP::VectorXd vec(Eigen::VectorXd::Random(3));
    if (zf(vec) != 0.) return 1;
    return 0;
}