/******************************************************************************
Copyright (c) 2020, Farbod Farshidian. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

#include <ocs2_core/cost/QuadraticStateCost.h>

namespace ocs2 {

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
QuadraticStateCost::QuadraticStateCost(matrix_t Q) : Q_(std::move(Q)) {}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
QuadraticStateCost* QuadraticStateCost::clone() const {
  return new QuadraticStateCost(*this);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
scalar_t QuadraticStateCost::getValue(scalar_t time, const vector_t& state, const CostDesiredTrajectories& desiredTrajectory) const {
  const vector_t xDeviation = getStateDeviation(time, state, desiredTrajectory);
  return 0.5 * xDeviation.dot(Q_ * xDeviation);
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
ScalarFunctionQuadraticApproximation QuadraticStateCost::getQuadraticApproximation(scalar_t time, const vector_t& state,
                                                                                   const CostDesiredTrajectories& desiredTrajectory) const {
  const vector_t xDeviation = getStateDeviation(time, state, desiredTrajectory);
  const vector_t qDeviation = Q_ * xDeviation;

  ScalarFunctionQuadraticApproximation Phi;
  Phi.f = 0.5 * xDeviation.dot(qDeviation);
  Phi.dfdx = qDeviation;
  Phi.dfdxx = Q_;
  return Phi;
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
vector_t QuadraticStateCost::getStateDeviation(scalar_t time, const vector_t& state,
                                               const CostDesiredTrajectories& desiredTrajectory) const {
  return state - desiredTrajectory.getDesiredState(time);
}

}  // namespace ocs2