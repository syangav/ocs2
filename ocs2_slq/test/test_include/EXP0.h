/*
 * EXP0.h
 *
 *  Created on: Jan 6, 2016
 *      Author: farbod
 */

#ifndef EXP0_OCS2_H_
#define EXP0_OCS2_H_

#include <cmath>
#include <limits>

#include <ocs2_core/logic/LogicRulesBase.h>
#include <ocs2_core/dynamics/ControlledSystemBase.h>
#include <ocs2_core/dynamics/DerivativesBase.h>
#include <ocs2_core/constraint/ConstraintBase.h>
#include <ocs2_core/cost/CostFunctionBase.h>
#include <ocs2_core/misc/FindActiveIntervalIndex.h>
#include <ocs2_core/initialization/SystemOperatingPoint.h>


namespace ocs2{

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
class EXP0_LogicRules : public LogicRulesBase<2,1>
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	typedef LogicRulesBase<2,1> BASE;

	EXP0_LogicRules(const BASE::scalar_array_t& switchingTimes)
	: BASE(switchingTimes)
	{}

	~EXP0_LogicRules() {}

	void adjustController(typename BASE::controller_t& controller) const override
	{}

private:

};

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
class EXP0_Sys1 : public ControlledSystemBase<2,1,EXP0_LogicRules>
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	EXP0_Sys1() {}
	~EXP0_Sys1() {}

	void computeDerivative( const double& t, const Eigen::Vector2d& x, const Eigen::Matrix<double,1,1>& u, Eigen::Vector2d& dxdt)  {
		Eigen::Matrix2d A;
		A << 0.6, 1.2, -0.8, 3.4;
		Eigen::Vector2d B;
		B << 1, 1;

		dxdt = A*x + B*u;
	}

	EXP0_Sys1* clone() const override {
		return new EXP0_Sys1(*this);
	}
};

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
class EXP0_Sys2 : public ControlledSystemBase<2,1,EXP0_LogicRules>
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	EXP0_Sys2() {}
	~EXP0_Sys2() {}

	void computeDerivative( const double& t, const Eigen::Vector2d& x, const Eigen::Matrix<double,1,1>& u, Eigen::Vector2d& dxdt)  {
		Eigen::Matrix2d A;
		A << 4, 3, -1, 0;
		Eigen::Vector2d B;
		B << 2, -1;

		dxdt = A*x + B*u;
	}

	EXP0_Sys2* clone() const override {
		return new EXP0_Sys2(*this);
	}
};

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
class EXP0_System : public ControlledSystemBase<2,1,EXP0_LogicRules>
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	typedef ControlledSystemBase<2,1,EXP0_LogicRules> Base;

	EXP0_System()
	: activeSubsystem_(0),
	  subsystemDynamicsPtr_(2)
	{
		subsystemDynamicsPtr_[0] = std::allocate_shared<EXP0_Sys1, Eigen::aligned_allocator<EXP0_Sys1>>( Eigen::aligned_allocator<EXP0_Sys1>() );
		subsystemDynamicsPtr_[1] = std::allocate_shared<EXP0_Sys2, Eigen::aligned_allocator<EXP0_Sys2>>( Eigen::aligned_allocator<EXP0_Sys2>() );
	}

	~EXP0_System() {}

	EXP0_System(const EXP0_System& other)
	: activeSubsystem_(other.activeSubsystem_),
	  subsystemDynamicsPtr_(2)
	{
		subsystemDynamicsPtr_[0] = Base::Ptr(other.subsystemDynamicsPtr_[0]->clone());
		subsystemDynamicsPtr_[1] = Base::Ptr(other.subsystemDynamicsPtr_[1]->clone());
	}

	void initializeModel(const LogicRulesMachine<2, 1, EXP0_LogicRules>& logicRulesMachine,
			const size_t& partitionIndex, const char* algorithmName=NULL) override {

		Base::initializeModel(logicRulesMachine, partitionIndex, algorithmName);

		findActiveSubsystemFnc_ = std::move( logicRulesMachine.getHandleToFindActiveSubsystemID(partitionIndex) );
	}

	EXP0_System* clone() const override {
		return new EXP0_System(*this);
	}

	void computeDerivative(const scalar_t& t, const state_vector_t& x, const input_vector_t& u,
			state_vector_t& dxdt) override {

		activeSubsystem_ = findActiveSubsystemFnc_(t);

		subsystemDynamicsPtr_[activeSubsystem_]->computeDerivative(t, x, u, dxdt);
	}

public:
	int activeSubsystem_;
	std::function<size_t(scalar_t)> findActiveSubsystemFnc_;
	std::vector<Base::Ptr> subsystemDynamicsPtr_;
};

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
class EXP0_SysDerivative1 : public DerivativesBase<2,1,EXP0_LogicRules>
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	EXP0_SysDerivative1() {};
	~EXP0_SysDerivative1() {};

	void getDerivativeState(state_matrix_t& A)  { A << 0.6, 1.2, -0.8, 3.4; }
	void getDerivativesControl(control_gain_matrix_t& B) { B << 1, 1; }

	EXP0_SysDerivative1* clone() const override {
		return new EXP0_SysDerivative1(*this);
	}
};


/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
class EXP0_SysDerivative2 : public DerivativesBase<2,1,EXP0_LogicRules>
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	EXP0_SysDerivative2() {};
	~EXP0_SysDerivative2() {};

	void getDerivativeState(state_matrix_t& A)  { A << 4, 3, -1, 0; }
	void getDerivativesControl(control_gain_matrix_t& B) { B << 2, -1; }

	EXP0_SysDerivative2* clone() const {
		return new EXP0_SysDerivative2(*this);
	}
};


/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
class EXP0_SystemDerivative : public DerivativesBase<2,1,EXP0_LogicRules>
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	typedef DerivativesBase<2,1,EXP0_LogicRules> Base;

	EXP0_SystemDerivative()
	: activeSubsystem_(0),
	  subsystemDerivativesPtr_(2)
	{
		subsystemDerivativesPtr_[0] = std::allocate_shared<EXP0_SysDerivative1, Eigen::aligned_allocator<EXP0_SysDerivative1>>(
				Eigen::aligned_allocator<EXP0_SysDerivative1>() );
		subsystemDerivativesPtr_[1] = std::allocate_shared<EXP0_SysDerivative2, Eigen::aligned_allocator<EXP0_SysDerivative2>>(
				Eigen::aligned_allocator<EXP0_SysDerivative2>() );
	}

	~EXP0_SystemDerivative() {}

	EXP0_SystemDerivative(const EXP0_SystemDerivative& other)
	: activeSubsystem_(other.activeSubsystem_),
	  subsystemDerivativesPtr_(2)
	{
		subsystemDerivativesPtr_[0] = Base::Ptr(other.subsystemDerivativesPtr_[0]->clone());
		subsystemDerivativesPtr_[1] = Base::Ptr(other.subsystemDerivativesPtr_[1]->clone());
	}


	void initializeModel(const LogicRulesMachine<2, 1, EXP0_LogicRules>& logicRulesMachine,
			const size_t& partitionIndex, const char* algorithmName=NULL) override {

		Base::initializeModel(logicRulesMachine, partitionIndex, algorithmName);

		findActiveSubsystemFnc_ = std::move( logicRulesMachine.getHandleToFindActiveSubsystemID(partitionIndex) );
	}

	EXP0_SystemDerivative* clone() const override {
		return new EXP0_SystemDerivative(*this);
	}

	void setCurrentStateAndControl(const scalar_t& t, const state_vector_t& x, const input_vector_t& u) override {

		Base::setCurrentStateAndControl(t, x, u);
		activeSubsystem_ = findActiveSubsystemFnc_(t);
		subsystemDerivativesPtr_[activeSubsystem_]->setCurrentStateAndControl(t, x, u);
	}

	void getDerivativeState(state_matrix_t& A)  {
		subsystemDerivativesPtr_[activeSubsystem_]->getDerivativeState(A);
	}

	void getDerivativesControl(control_gain_matrix_t& B) {
		subsystemDerivativesPtr_[activeSubsystem_]->getDerivativesControl(B);
	}

public:
	int activeSubsystem_;
	std::function<size_t(scalar_t)> findActiveSubsystemFnc_;
	std::vector<Base::Ptr> subsystemDerivativesPtr_;

};

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
using EXP0_SystemConstraint = ConstraintBase<2,1,EXP0_LogicRules>;

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
class EXP0_CostFunction1 : public CostFunctionBase<2,1,EXP0_LogicRules>
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	EXP0_CostFunction1() {};
	~EXP0_CostFunction1() {};

	void evaluate(scalar_t& L) { L = 0.5*(x_(1)-2.0)*(x_(1)-2.0) + 0.5*u_(0)*u_(0); }

	void stateDerivative(state_vector_t& dLdx) { dLdx << 0.0, (x_(1)-2.0); }
	void stateSecondDerivative(state_matrix_t& dLdxx)  { dLdxx << 0.0, 0.0, 0.0, 1.0; }
	void controlDerivative(input_vector_t& dLdu)  { dLdu << u_; }
	void controlSecondDerivative(control_matrix_t& dLduu)  { dLduu << 1.0; }

	void stateControlDerivative(control_feedback_t& dLdxu) { dLdxu.setZero(); }

	void terminalCost(scalar_t& Phi) { Phi = 0; }
	void terminalCostStateDerivative(state_vector_t& dPhidx)  { dPhidx.setZero(); }
	void terminalCostStateSecondDerivative(state_matrix_t& dPhidxx)  { dPhidxx.setZero(); }

	EXP0_CostFunction1* clone() const override {
		return new EXP0_CostFunction1(*this);
	};
};



/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
class EXP0_CostFunction2 : public CostFunctionBase<2,1,EXP0_LogicRules>
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	EXP0_CostFunction2() {};
	~EXP0_CostFunction2() {};

	void evaluate(scalar_t& L) { L = 0.5*(x_(1)-2.0)*(x_(1)-2.0) + 0.5*u_(0)*u_(0); }

	void stateDerivative(state_vector_t& dLdx) { dLdx << 0.0, (x_(1)-2.0); }
	void stateSecondDerivative(state_matrix_t& dLdxx)  { dLdxx << 0.0, 0.0, 0.0, 1.0; }
	void controlDerivative(input_vector_t& dLdu)  { dLdu << u_; }
	void controlSecondDerivative(control_matrix_t& dLduu)  { dLduu << 1.0; }

	void stateControlDerivative(control_feedback_t& dLdxu) { dLdxu.setZero(); }

	void terminalCost(scalar_t& Phi) { Phi = 0.5*(x_(0)-4.0)*(x_(0)-4.0) + 0.5*(x_(1)-2.0)*(x_(1)-2.0); }
	void terminalCostStateDerivative(state_vector_t& dPhidx)  { dPhidx << (x_(0)-4.0), (x_(1)-2.0); }
	void terminalCostStateSecondDerivative(state_matrix_t& dPhidxx)  { dPhidxx.setIdentity(); }

	EXP0_CostFunction2* clone() const override {
		return new EXP0_CostFunction2(*this);
	};

};

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
class EXP0_CostFunction : public CostFunctionBase<2,1,EXP0_LogicRules>
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	typedef CostFunctionBase<2,1,EXP0_LogicRules> Base;

	EXP0_CostFunction()
	: activeSubsystem_(0),
	  subsystemCostsPtr_(2)
	{
		subsystemCostsPtr_[0] = std::allocate_shared<EXP0_CostFunction1, Eigen::aligned_allocator<EXP0_CostFunction1>>(
				Eigen::aligned_allocator<EXP0_CostFunction1>() );
		subsystemCostsPtr_[1] = std::allocate_shared<EXP0_CostFunction2, Eigen::aligned_allocator<EXP0_CostFunction2>>(
				Eigen::aligned_allocator<EXP0_CostFunction2>() );
	}

	~EXP0_CostFunction() {}

	EXP0_CostFunction(const EXP0_CostFunction& other)
	: activeSubsystem_(other.activeSubsystem_),
	  subsystemCostsPtr_(2)
	{
		subsystemCostsPtr_[0] = Base::Ptr(other.subsystemCostsPtr_[0]->clone());
		subsystemCostsPtr_[1] = Base::Ptr(other.subsystemCostsPtr_[1]->clone());
	}

	void initializeModel(const LogicRulesMachine<2, 1, EXP0_LogicRules>& logicRulesMachine,
			const size_t& partitionIndex, const char* algorithmName=NULL) override {

		Base::initializeModel(logicRulesMachine, partitionIndex, algorithmName);

		findActiveSubsystemFnc_ = std::move( logicRulesMachine.getHandleToFindActiveSubsystemID(partitionIndex) );
	}

	EXP0_CostFunction* clone() const override {
		return new EXP0_CostFunction(*this);
	}

	void setCurrentStateAndControl(const scalar_t& t, const state_vector_t& x, const input_vector_t& u) override {

		Base::setCurrentStateAndControl(t, x, u);
		activeSubsystem_ = findActiveSubsystemFnc_(t);
		subsystemCostsPtr_[activeSubsystem_]->setCurrentStateAndControl(t, x, u);
	}

	void evaluate(scalar_t& L) {
		subsystemCostsPtr_[activeSubsystem_]->evaluate(L);
	}

	void stateDerivative(state_vector_t& dLdx) {
		subsystemCostsPtr_[activeSubsystem_]->stateDerivative(dLdx);
	}
	void stateSecondDerivative(state_matrix_t& dLdxx)  {
		subsystemCostsPtr_[activeSubsystem_]->stateSecondDerivative(dLdxx);
	}
	void controlDerivative(input_vector_t& dLdu)  {
		subsystemCostsPtr_[activeSubsystem_]->controlDerivative(dLdu);
	}
	void controlSecondDerivative(control_matrix_t& dLduu)  {
		subsystemCostsPtr_[activeSubsystem_]->controlSecondDerivative(dLduu);
	}

	void stateControlDerivative(control_feedback_t& dLdxu) {
		subsystemCostsPtr_[activeSubsystem_]->stateControlDerivative(dLdxu);
	}

	void terminalCost(scalar_t& Phi) {
		subsystemCostsPtr_[activeSubsystem_]->terminalCost(Phi);
	}
	void terminalCostStateDerivative(state_vector_t& dPhidx)  {
		subsystemCostsPtr_[activeSubsystem_]->terminalCostStateDerivative(dPhidx);
	}
	void terminalCostStateSecondDerivative(state_matrix_t& dPhidxx)  {
		subsystemCostsPtr_[activeSubsystem_]->terminalCostStateSecondDerivative(dPhidxx);
	}

public:
	int activeSubsystem_;
	std::function<size_t(scalar_t)> findActiveSubsystemFnc_;
	std::vector<std::shared_ptr<CostFunctionBase<2,1,EXP0_LogicRules> > > subsystemCostsPtr_;

};

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
using EXP0_SystemOperatingTrajectories = SystemOperatingPoint<2,1,EXP0_LogicRules>;

} // namespace ocs2

#endif /* EXP0_OCS2_H_ */
