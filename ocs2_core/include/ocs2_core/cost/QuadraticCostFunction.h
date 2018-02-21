/*
 * QuadraticCostFunction.h
 *
 *  Created on: Jan 3, 2016
 *      Author: farbod
 */

#ifndef QUADRATICCOSTFUNCTION_OCS2_H_
#define QUADRATICCOSTFUNCTION_OCS2_H_

#include "ocs2_core/cost/CostFunctionBase.h"

namespace ocs2{

/**
 * Quadratic Cost Function.
 *
 * @tparam STATE_DIM: Dimension of the state space.
 * @tparam INPUT_DIM: Dimension of the control input space.
 * @tparam LOGIC_RULES_T: Logic Rules type (default NullLogicRules).
 */
template <size_t STATE_DIM, size_t INPUT_DIM, class LOGIC_RULES_T=NullLogicRules>
class QuadraticCostFunction : public CostFunctionBase< STATE_DIM, INPUT_DIM, LOGIC_RULES_T>
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	typedef CostFunctionBase<STATE_DIM, INPUT_DIM, LOGIC_RULES_T> Base;

	typedef Dimensions<STATE_DIM, INPUT_DIM> DIMENSIONS;
	typedef typename DIMENSIONS::scalar_t scalar_t;
	typedef typename DIMENSIONS::state_vector_t state_vector_t;
	typedef typename DIMENSIONS::state_matrix_t state_matrix_t;
	typedef typename DIMENSIONS::control_vector_t input_vector_t;
	typedef typename DIMENSIONS::control_matrix_t control_matrix_t;
	typedef typename DIMENSIONS::control_feedback_t control_feedback_t;

	/**
	 * Constructor for the running and final cost function defined as the following:
	 * - \f$ L = 0.5(x-x_{nominal})' Q (x-x_{nominal}) + 0.5(u-u_{nominal})' R (u-u_{nominal}) \f$
	 * - \f$ \Phi = 0.5(x-x_{final})' Q_{final} (x-x_{final}) \f$.
	 * @param [in] Q: \f$ Q \f$
	 * @param [in] R: \f$ R \f$
	 * @param [in] x_nominal: \f$ x_{nominal}\f$
	 * @param [in] u_nominal: \f$ u_{nominal}\f$
	 * @param [in] x_final: \f$ x_{final}\f$
	 * @param [in] Q_final: \f$ Q_{final}\f$
	 */
	QuadraticCostFunction(const state_matrix_t& Q,
			const control_matrix_t& R,
	    const state_vector_t& x_nominal,
	    const input_vector_t& u_nominal,
	    const state_vector_t& x_final,
	    const state_matrix_t Q_final)
		: Q_(Q),
		  R_(R),
		  x_nominal_(x_nominal),
		  u_nominal_(u_nominal),
		  x_final_(x_final),
		  Q_final_(Q_final)
	{}

	virtual ~QuadraticCostFunction() {}

	/**
	 * Initializes the quadratic cost function.
	 *
	 * @param [in] logicRulesMachine: A class which contains and parse the logic rules e.g
	 * method findActiveSubsystemHandle returns a Lambda expression which can be used to
	 * find the ID of the current active subsystem.
	 * @param [in] partitionIndex: index of the time partition.
	 * @param [in] algorithmName: The algorithm that class this class (default not defined).
	 */
	virtual void initializeModel(const LogicRulesMachine<STATE_DIM, INPUT_DIM, LOGIC_RULES_T>& logicRulesMachine,
			const size_t& partitionIndex, const char* algorithmName=NULL) override {

		Base::initializeModel(logicRulesMachine, partitionIndex, algorithmName);
	}

	/**
	 * Sets the current time, state, and control input
	 *
	 * @param [in] t: Current time
	 * @param [in] x: Current state vector
	 * @param [in] u: Current input vector
	 */
	virtual void setCurrentStateAndControl(const scalar_t& t, const state_vector_t& x, const input_vector_t& u) {

		Base::setCurrentStateAndControl(t, x, u);
		x_deviation_ = x - x_nominal_;
		u_deviation_ = u - u_nominal_;
	}

    /**
     * Evaluates the cost.
     *
     * @param [out] cost: The cost value.
     */
	void evaluate(scalar_t& cost) 	{

		scalar_t costQ = 0.5 * x_deviation_.transpose() * Q_ * x_deviation_;
		scalar_t costR = 0.5 * u_deviation_.transpose() * R_ * u_deviation_;
		cost = costQ + costR;
	}

    /**
     * Gets the state derivative.
     *
     * @param [out] dLdx: First order cost derivative with respect to state vector.
     */
	void stateDerivative(state_vector_t& dLdx)  {
		dLdx =  Q_ * x_deviation_;
	}

    /**
     * Gets state second order derivative.
     *
     * @param [out] dLdxx: Second order cost derivative with respect to state vector.
     */
	void stateSecondDerivative(state_matrix_t& dLdxx)  {
		dLdxx = Q_;
	}

    /**
     * Gets control derivative.
     *
     * @param [out] dLdu: First order cost derivative with respect to input vector.
     */
	void controlDerivative(input_vector_t& dLdu)	{
		dLdu = R_ * u_deviation_;
	}

    /**
     * Gets control second derivative.
     *
     * @param [out] dLduu: Second order cost derivative with respect to input vector.
     */
	void controlSecondDerivative(control_matrix_t& dLduu)	{
		dLduu = R_;
	}

    /**
     * Gets the state, input derivative.
     *
     * @param [out] dLdxu: Second order cost derivative with respect to state and input vector.
     */
	void stateControlDerivative(control_feedback_t& dLdxu)	{
		dLdxu = control_feedback_t::Zero();
	}

    /**
     * Gets the terminal cost.
     *
     * @param [out] cost: The final cost value
     */
	void terminalCost(scalar_t& cost)	{

		state_vector_t x_deviation_final = this->x_ - x_final_;
		cost = 0.5 * x_deviation_final.transpose() * Q_final_ * x_deviation_final;
	}

    /**
     * Gets the terminal cost state derivative.
     *
     * @param [out] dPhidx: First order final cost derivative with respect to state vector.
     */
	void terminalCostStateDerivative(state_vector_t& dPhidx)	{

		state_vector_t x_deviation_final = this->x_ - x_final_;
		dPhidx = Q_final_ * x_deviation_final;
	}

    /**
     * Gets the terminal cost state second derivative
     *
     * @param [out] dPhidxx: Second order final cost derivative with respect to state vector.
     */
	void terminalCostStateSecondDerivative(state_matrix_t& dPhidxx)	{
		dPhidxx = Q_final_;
	}

    /**
     * Returns pointer to the class.
     *
     * @return A raw pointer to the class.
     */
	virtual QuadraticCostFunction<STATE_DIM, INPUT_DIM, LOGIC_RULES_T>* clone() const override {
		return new QuadraticCostFunction<STATE_DIM, INPUT_DIM, LOGIC_RULES_T>(*this);
	}

protected:
	state_vector_t x_deviation_;
	state_vector_t x_nominal_;
	state_matrix_t Q_;

	input_vector_t u_deviation_;
	input_vector_t u_nominal_;
	control_matrix_t R_;

	state_vector_t x_final_;
	state_matrix_t Q_final_;

};

} // namespace ocs2

#endif /* QUADRATICCOSTFUNCTION_H_ */
