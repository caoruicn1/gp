#ifndef GAUSSIAN_PROCESS_INTERPOLATION_H
#define GAUSSIAN_PROCESS_INTERPOLATION_H

#include "macros.h"
#include "univariate_functions.h"
#include "bivariate_functions.h"
#include <Eigen/Dense>
#include <Eigen/Cholesky>

using Eigen::MatrixXd;
using Eigen::VectorXd;
using Eigen::RowVectorXd;

// class GaussianProcessInterpolationRestraint;
class GaussianProcessInterpolationScoreState;

//! GaussianProcessInterpolation
/** This class provides methods to perform bayesian interpolation/smoothing of
 * data using a gaussian process prior on the function to be interpolated.
 * It takes a dataset as input (via its sufficient statistics) along with prior
 * mean and covariance functions. It outputs the value of the posterior mean and
 * covariance functions at points requested by the user.
 */
class GaussianProcessInterpolation {
   public:
    /** Constructor for the gaussian process
     * \param [in] x : a list of coordinates in N-dimensional space
     *                corresponding to the abscissa of each observation
     * \param [in] sample_mean \f$I\f$ : vector of mean observations at each of
     *                                  the previously defined coordinates
     * \param [in] sample_std \f$s\f$ : vector of sample standard deviations
     * \param [in] n_obs \f$N\f$ : sample size for mean and std
     * \param [in] mean_function \f$m\f$ : a pointer to the prior mean function
     *                                    to use.  Should be compatible with
     *                                    the size of x(i).
     * \param [in] covariance_function \f$w\f$: prior covariance function.
     * \param [in] sigma : ISD Scale (proportionality factor to S)
     * \param [in] sparse_cutoff : when to consider that a matrix entry is zero
     *
     * Computes the necessary matrices and inverses when called.
     */
    GaussianProcessInterpolation(MatrixXd x, VectorXd sample_mean,
                                 VectorXd sample_std, unsigned n_obs,
                                 UnivariateFunction *mean_function,
                                 BivariateFunction *covariance_function,
                                 kernel::Particle *sigma,
                                 double sparse_cutoff = 1e-7);

    /** Get posterior mean and covariance functions, at the points requested
     * Posterior mean is defined as
     * \f[\hat{I}(x) = m(x)
     *        + {}^t\mathbf{w}(q)
     *        (\mathbf{W}+\mathbf{S})^{-1}
     *        (\mathbf{I}-\mathbf{m}) \f]
     * Posterior covariance
     * \f[\hat{\sigma}^2(x,x') =
     *    w(x,x') - {}^t\mathbf{w}(x)
     *              (\mathbf{W} + \mathbf{S})^{-1}
     *              \mathbf{w}(x') \f]
     * where \f$\mathbf{m}\f$ is the vector built by evaluating the prior mean
     * function at the observation points; \f$\mathbf{w}(x)\f$ is the vector of
     * covariances between each observation point and the current point;
     * \f$\mathbf{W}\f$ is the prior covariance matrix built by evaluating the
     * covariance function at each of the observations; \f$\mathbf{S}\f$ is the
     * diagonal covariance matrix built from sample_std and n_obs.
     *
     * Both functions will check if the mean or covariance functions have
     *changed
     * since the last call, and will recompute
     * \f$(\mathbf{W} + \mathbf{S})^{-1}\f$ if necessary.
     */
    double get_posterior_mean(VectorXd x) const;
    double get_posterior_covariance(VectorXd x1, VectorXd x2) const;
    MatrixXd get_posterior_covariance_matrix(MatrixXd x) const;

    // derivative: d(mean(q))/(dparticle_i)
    // VectorXd get_posterior_mean_derivative(VectorXd x) const;

    // derivative: d(cov(q,q))/(dparticle_i)
    VectorXd get_posterior_covariance_derivative(VectorXd x) const;

    // hessian: d(mean(q))/(dparticle_i dparticle_j)
    // MatrixXd get_posterior_mean_hessian(VectorXd x) const;

    // hessian: d(cov(q,q))/(dparticle_i dparticle_j)
    MatrixXd get_posterior_covariance_hessian(VectorXd x) const;

    // call these if you called update() on the mean or covariance function.
    // it will force update any internal variables dependent on these functions.
    void force_mean_update();
    void force_covariance_update();

    // returns the number of particles that control m's values.
    // public for testing purposes
    unsigned get_number_of_m_particles() const;

    // returns true if a particle is optimized
    // public for testing purposes
    bool get_m_particle_is_optimized(unsigned i) const;

    // returns the number of particles that control Omega's values.
    // public for testing purposes
    unsigned get_number_of_Omega_particles() const;

    // returns true if a particle is optimized
    // public for testing purposes
    bool get_Omega_particle_is_optimized(unsigned i) const;

    // returns data
    MatrixXd get_data_abscissa() const;
    VectorXd get_data_mean() const;
    MatrixXd get_data_variance() const;

    friend class GaussianProcessInterpolationRestraint;
    friend class GaussianProcessInterpolationScoreState;

    IMP_OBJECT_METHODS(GaussianProcessInterpolation);

   protected:
    // returns updated data vector
    VectorXd get_I() const { return I_; }
    // returns updated prior mean vector
    VectorXd get_m() const;
    // returns dm/dparticle
    VectorXd get_m_derivative(unsigned particle) const;
    // returns d2m/(dparticle_1 dparticle_2)
    VectorXd get_m_second_derivative(unsigned particle1,
                                     unsigned particle2) const;
    // returns updated prior covariance vector
    void add_to_m_particle_derivative(unsigned particle, double value,
                                      DerivativeAccumulator &accum);
    // returns updated prior covariance vector
    VectorXd get_wx_vector(VectorXd xval) const;
    // returns updated data covariance matrix
    Eigen::DiagonalMatrix<double, Eigen::Dynamic> get_S() const { return S_; }
    // returns updated prior covariance matrix
    MatrixXd get_W() const;
    // returns Omega=(W+S/N)
    MatrixXd get_Omega() const;
    // returns dOmega/dparticle
    MatrixXd get_Omega_derivative(unsigned particle) const;
    // returns d2Omega/(dparticle_1 dparticle_2)
    MatrixXd get_Omega_second_derivative(unsigned particle1,
                                         unsigned particle2) const;
    // returns updated prior covariance vector
    void add_to_Omega_particle_derivative(unsigned particle, double value,
                                          DerivativeAccumulator &accum);
    // returns LDLT decomp of omega
    Eigen::LDLT<MatrixXd, Eigen::Upper> get_ldlt() const;
    // returns updated Omega^{-1}
    MatrixXd get_Omi() const;
    // returns updated Omega^{-1}(I-m)
    VectorXd get_OmiIm() const;

   private:
    // ensures the mean/covariance function has updated parameters. Signals an
    // update by changing the state flags. Returns true if the function has
    // changed. This is used by GaussianProcessInterpolationRestraint.
    void update_flags_mean();
    void update_flags_covariance();

    // compute prior covariance matrix
    void compute_W();
    // compute \f$(\mathbf{W} + \frac{\sigma}{N}\mathbf{S})^{-1}\f$.
    void compute_Omega();
    // compute LDLT decomposition of Omega
    void compute_ldlt();
    // compute \f$(\mathbf{W} + \frac{\sigma}{N}\mathbf{S})^{-1}\f$.
    void compute_Omi();
    // compute (W+sigma*S/N)^{-1} (I-m)
    void compute_OmiIm();

    // compute dw(q)/dparticle_i
    VectorXd get_wx_vector_derivative(VectorXd q, unsigned i) const;
    // compute dw(q)/(dparticle_i * dparticle_j)
    VectorXd get_wx_vector_second_derivative(VectorXd q, unsigned i,
                                             unsigned j) const;

    // compute dcov(q,q)/dw(q)
    VectorXd get_dcov_dwq(VectorXd q) const;
    // compute dcov(q,q)/dOmega
    MatrixXd get_dcov_dOm(VectorXd q) const;
    // compute d2cov(q,q)/(dw(q) dw(q)) (independent of q !)
    MatrixXd get_d2cov_dwq_dwq() const;
    // compute d2cov(q,q)/(dw(q)_m dOmega)
    MatrixXd get_d2cov_dwq_dOm(VectorXd q, unsigned m) const;
    // compute d2cov(q,q)/(dOmega dOmega_mn)
    MatrixXd get_d2cov_dOm_dOm(VectorXd q, unsigned m, unsigned n) const;

    // compute mean observations
    void compute_I(VectorXd mean);
    // compute diagonal covariance matrix of observations
    void compute_S(VectorXd std);
    // compute prior mean vector
    void compute_m();

   private:
    unsigned N_;      // number of dimensions of the abscissa
    unsigned M_;      // number of observations to learn from
    MatrixXd x_;      // abscissa
    unsigned n_obs_;  // number of observations
    // pointer to the prior mean function
    IMP::base::PointerMember<UnivariateFunction> mean_function_;
    // pointer to the prior covariance function
    IMP::base::PointerMember<BivariateFunction> covariance_function_;
    VectorXd I_, m_;
    MatrixXd W_, Omega_, Omi_;  // Omi = Omega^{-1}
    Eigen::DiagonalMatrix<double, Eigen::Dynamic> S_;
    VectorXd OmiIm_;  // Omi * (I - m)
    bool flag_m_, flag_m_gpir_, flag_Omi_, flag_OmiIm_, flag_W_, flag_Omega_,
        flag_Omega_gpir_, flag_ldlt_;
    IMP::base::Pointer<IMP::kernel::Particle> sigma_;
    double cutoff_;
    double sigma_val_;  // to know if an update is needed
    Eigen::LDLT<MatrixXd, Eigen::Upper> ldlt_;
};

#endif /* GAUSSIAN_PROCESS_INTERPOLATION_H */