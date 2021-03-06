#ifndef MPC_H
#define MPC_H

#include <vector>
#include <cppad/cppad.hpp>
#include <cppad/ipopt/solve.hpp>
#include "Eigen-3.3/Eigen/Core"
#include <chrono>

using namespace std;
// Return controls, and predicted x and y values
using MPC_OUTPUT = tuple<double, double, vector<double>, vector<double>>;
typedef CPPAD_TESTVECTOR(double) Dvector;

// Evaluate a polynomial (using CPPAD).
template<typename scalar_t>
scalar_t polyeval_cppad(Eigen::VectorXd coeffs, scalar_t x) {
  scalar_t result = 0.0;
  for (int i = 0; i < coeffs.size(); i++) {
    result += coeffs[i] * CppAD::pow(x, i);
  }
  return result;
}

// Evaluate a polynomial slope (using CPPAD).
template<typename scalar_t>
scalar_t polyeval_slope_cppad(Eigen::VectorXd coeffs, scalar_t x) {
  scalar_t result = 0.0;
  for (int i = 1; i < coeffs.size(); i++) {
    result += i*coeffs[i] * CppAD::pow(x, i-1);
  }
  return result;
}






/**
* Convert global co-ordinate to local Co-ordinate system 
* @
*/

template <typename scalar_t , typename vector_t>
tuple<scalar_t,scalar_t> globalTolocal_Transformation (tuple<scalar_t,scalar_t> input , vector_t vehicle)
{

	scalar_t x;
	scalar_t y;
	scalar_t x0 	= vehicle[0] ;
	scalar_t y0 	= vehicle[1] ;
	scalar_t theta0 = vehicle[2] ;
	std::tie(x,y)	= input ;

	x = x - x0 ; 
	y = y - y0 ;

	 return std::make_tuple(x * CppAD::cos(theta0) + y * CppAD::sin(theta0),
                        -x * CppAD::sin(theta0) + y * CppAD::cos(theta0));

}


struct MPC_configuration
{
	double v_max ;
	double ref_v;

	double ref_epsi;
	double ref_cte;

	double w_cte;
	double w_epsi;
	double w_v;


	double w_delta;
	double w_a;

	double w_deltadot;
	double w_adot;


	size_t solver_N ;
	double solver_dt;
	double solver_timeout;


	double p_lag;
	double p_steering_limit;
};

class MPC {
 public:
  MPC(MPC_configuration& config);

  virtual ~MPC();

   void Init(Eigen::VectorXd x0);

  // Solve the model given an initial state and polynomial coefficients.
  // Return the first actuatotions.
  MPC_OUTPUT Solve(Eigen::VectorXd state, Eigen::VectorXd coeffs);

  private :
  MPC_configuration& config_;
  bool is_initialized_ = false;

  CppAD::ipopt::solve_result<Dvector> last_sol_;
  int time_ctr = -1;
  int N_;
  double dt_;

  Dvector last_control_;

  // ipopt options
  std::string options_;

  Dvector vars_;
  Dvector constraints_lowerbound_;
  Dvector constraints_upperbound_;
  Dvector vars_lowerbound_;
  Dvector vars_upperbound_;

};

#endif /* MPC_H */
