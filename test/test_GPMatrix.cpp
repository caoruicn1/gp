#include "Scalar.h"
#include "Matrix.h"

#include <Eigen/Dense>
#include <type_traits>
#include <math.h>

using namespace GP;

int main(int, char * []) {
  // traits classes
  static_assert(std::is_same<MatrixXd::result_type, Eigen::MatrixXd>::value,
                "wrong result_type");
  static_assert(std::is_same<MatrixXd::scalar_type, double>::value,
                "wrong scalar_type");

  // matrix basics
  const unsigned int szx = 10, szy = 3;
  Eigen::MatrixXd x(Eigen::MatrixXd::Constant(szx, szy, 1));
  MatrixXd vx(x);
  Eigen::MatrixXd y(Eigen::MatrixXd::Constant(szx, szy, 2));
  MatrixXd vy(y);
  if (vx.get() != x) return 1;
  // sum
  vx + vy;
  const Eigen::MatrixXd tmp((vx + vy).get());
  MatrixXd vsum(vy + vx);
  if (vsum.get() != Eigen::MatrixXd::Constant(szx, szy, 3)) return 2;
  if ((vx + vy).get() != Eigen::MatrixXd::Constant(szx, szy, 3)) return 3;
  internal::MatrixSum<MatrixXd, MatrixXd> s(vx, vy);  // type is defined
  if (s.get() != vsum.get()) return 4;  // works as expected
  // product
  if ((vx.transpose() * vx).get() != x.transpose() * x) return 5;
  // difference
  if ((vx - vy).get() != Eigen::MatrixXd::Constant(szx, szy, -1)) return 6;

  // scalar basics
  Scalar scal(3.2);
  if (scal.get() != 3.2) return 7;
  // sum
  if (std::abs((scal + scal).get() - 6.4) > 1e-7) return 8;
  // difference
  if (std::abs((scal - scal).get() - 0.) > 1e-7) return 24;
  // product
  if (std::abs((scal * scal).get() - 10.24) > 1e-7) return 25;
  // quotient
  if (std::abs((scal / scal).get() - 1.) > 1e-7) return 26;

  // builtin scalar product
  if ((scal * 3.).get() != (scal.get() * 3.)) return 11;
  if ((scal * 3).get() != (scal.get() * 3.)) return 12;
  if ((3. * scal).get() != (scal.get() * 3.)) return 13;
  if ((3 * scal).get() != (scal.get() * 3.)) return 14;
  // builtin scalar quotient
  if ((scal / 3.).get() != (scal.get() / 3.)) return 28;
  if ((3. / scal).get() != (3./scal.get())) return 29;
  // builtin scalar difference
  if ((scal - 3.).get() != (scal.get() - 3.)) return 30;
  if ((3. - scal).get() != (3.-scal.get())) return 31;
  // builtin scalar sum
  if ((scal + 3.).get() != (scal.get() + 3.)) return 32;
  if ((3. + scal).get() != (3.+scal.get())) return 33;

  // matrix scalar product
  if ((scal * vx).get() != (scal.get() * x)) return 9;
  if ((vx * scal).get() != (scal.get() * x)) return 10;
  // matrix scalar quotient
  if ((vx / scal).get() != (x / scal.get())) return 27;

  // matrix builtin product
  if ((5 * vx).get() != (5. * x)) return 15;
  if ((vx * 5.).get() != (5. * x)) return 16;
  // matrix builtin quotient
  if ((vx / 5.).get() != (x/5.)) return 28;

  // lots of products sums and divisions
  if ((vx.transpose() * (5 * vx + vy - scal * vx)).get()
      != (x.transpose() * (5 * x + y - scal.get() * x)))
    return 17;

  // transpose
  if (vx.transpose().get() != x.transpose()) return 17;
  if ((vx + vx).transpose().get() != (x + x).transpose()) return 18;
  if ((vx - vx).transpose().get() != (x - x).transpose()) return 19;
  if ((vx - scal * vx).transpose().get() != (x - scal.get() * x).transpose())
    return 20;

  // ldlt
  Eigen::MatrixXd L(Eigen::MatrixXd::Random(5, 5));
  L.diagonal() = L.diagonal().array().abs();
  L = L.triangularView<Eigen::Lower>();
  Eigen::MatrixXd sd(L * L.transpose());
  MatrixXd msd(sd);
  auto ldlt = msd.decomposition();
  ldlt.get();
  (sd * sd).ldlt();
  (msd * msd).decomposition();
  (msd * msd).decomposition().get();

  // determinant
  LogDeterminant<decltype(ldlt)> det(ldlt);
  static_assert(std::is_same<decltype(det), decltype(ldlt.logdet())>::value,
                "MatrixBase passes wrong type to LogDeterminant");
  if (std::abs(det.get() - std::log(sd.determinant())) > 1e-7) return 21;

  if (std::abs(ldlt.logdet().get() - std::log(sd.determinant())) > 1e-7)
    return 22;

  // solve AX=B
  Eigen::MatrixXd B(Eigen::MatrixXd::Random(5, 2));
  MatrixXd mB(B);
  Eigen::MatrixXd X(sd.ldlt().solve(B));
  MatrixXd mX(ldlt.solve(mB).get());
  if ((X - mX.get()).array().abs().matrix().norm() > 1e-5) return 23;
  if ((ldlt.solve(mB).get() - sd.ldlt().solve(B)).array().abs().matrix().norm()
      > 1e-5)
    return 24;

  //rows and columns
  MatrixXd dummy(Eigen::MatrixXd::Random(szx,szy));
  if (dummy.rows() != szx) return 26;
  if (dummy.cols() != szy) return 27;

  //trace
  if (dummy.trace().get() != dummy.get().trace()) return 28;

  //products of determinants should be possible
  //double tmp4((msd.decomposition().logdet()
  //            *msd.decomposition().logdet()).get());

  //multiple products should work too
  if ( (msd*3*msd).get() != (sd*3*sd) ) return 29;
  if ( (3*msd*msd).get() != (3*sd*sd) ) return 30;

  //dangling refs causing BAD_ALLOC / segfault
  scal.set(5.0);
  Eigen::MatrixXd eId(Eigen::MatrixXd::Identity(5,5));
  MatrixXd Id(eId);
  if ((scal-1.0-scal-3.0-scal-5.0).get() != 5-1-5-3-5-5) return 31;
  if ((scal/(1.0/scal)/scal).get() != 5.0/(1.0/5.0)/5.0) return 32;
  if ((MatrixXd(msd*Id)*Id).get() != sd*eId*eId) return 33;
  if (((((3 * msd) * 3) * 3).get() - (sd * 27)).array().abs().matrix().norm()
      > 1e-5) return 34;
  if (((((msd / 3) / 3) / 3).get() - (sd / 27)).array().abs().matrix().norm()
      > 1e-5) return 35;
  if (((msd-Id)-Id-(msd-Id)).get() != sd-eId-eId-(sd-eId)) return 36;
  if (((msd*Id)*Id).get() != sd*eId*eId) return 37;
  //Eigen::MatrixXd((msd*3*msd*3*msd).get());
  return 0;
}
