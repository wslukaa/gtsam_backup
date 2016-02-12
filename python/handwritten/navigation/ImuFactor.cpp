/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation,
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @brief wraps ConstantTwistScenario class to python
 * @author Frank Dellaert
 **/

#include <boost/python.hpp>

#define NO_IMPORT_ARRAY
#include <numpy_eigen/NumpyEigenConverter.hpp>

#include "gtsam/navigation/ImuFactor.h"

using namespace boost::python;
using namespace gtsam;

typedef gtsam::OptionalJacobian<3, 9> OptionalJacobian39;
typedef gtsam::OptionalJacobian<9, 6> OptionalJacobian96;
typedef gtsam::OptionalJacobian<9, 9> OptionalJacobian9;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(print_overloads, print, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(attitude_overloads, attitude, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(position_overloads, position, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(velocity_overloads, velocity, 0, 1)

void exportImuFactor() {
  class_<OptionalJacobian39>("OptionalJacobian39", init<>());
  class_<OptionalJacobian96>("OptionalJacobian96", init<>());
  class_<OptionalJacobian9>("OptionalJacobian9", init<>());

  class_<NavState>("NavState", init<>())
      // TODO(frank): overload with jacobians
      .def("print", &NavState::print, print_overloads())
      .def("attitude", &NavState::attitude,
           attitude_overloads()[return_value_policy<copy_const_reference>()])
      .def("position", &NavState::position,
           position_overloads()[return_value_policy<copy_const_reference>()])
      .def("velocity", &NavState::velocity,
           velocity_overloads()[return_value_policy<copy_const_reference>()])
      .def(repr(self))
      .def("pose", &NavState::pose);

  class_<imuBias::ConstantBias>("ConstantBias", init<>())
      .def(init<const Vector3&, const Vector3&>())
      .def(repr(self));

  class_<PreintegrationParams, boost::shared_ptr<PreintegrationParams>>(
      "PreintegrationParams", init<const Vector3&>())
      .def_readwrite("gyroscopeCovariance",
                     &PreintegrationParams::gyroscopeCovariance)
      .def_readwrite("omegaCoriolis", &PreintegrationParams::omegaCoriolis)
      .def_readwrite("body_P_sensor", &PreintegrationParams::body_P_sensor)
      .def_readwrite("accelerometerCovariance",
                     &PreintegrationParams::accelerometerCovariance)
      .def_readwrite("integrationCovariance",
                     &PreintegrationParams::integrationCovariance)
      .def_readwrite("use2ndOrderCoriolis",
                     &PreintegrationParams::use2ndOrderCoriolis)
      .def_readwrite("n_gravity", &PreintegrationParams::n_gravity)

      .def("MakeSharedD", &PreintegrationParams::MakeSharedD)
      .staticmethod("MakeSharedD")
      .def("MakeSharedU", &PreintegrationParams::MakeSharedU)
      .staticmethod("MakeSharedU");

  class_<PreintegratedImuMeasurements>(
      "PreintegratedImuMeasurements",
      init<const boost::shared_ptr<PreintegrationParams>&,
           const imuBias::ConstantBias&>())
      .def(repr(self))
      .def("predict", &PreintegratedImuMeasurements::predict)
      .def("computeError", &PreintegratedImuMeasurements::computeError)
      .def("resetIntegration", &PreintegratedImuMeasurements::resetIntegration)
      .def("integrateMeasurement",
           &PreintegratedImuMeasurements::integrateMeasurement)
      .def("preintMeasCov", &PreintegratedImuMeasurements::preintMeasCov);

  // NOTE(frank): Abstract classes need boost::noncopyable
  class_<ImuFactor, bases<NonlinearFactor>, boost::shared_ptr<ImuFactor>>(
      "ImuFactor")
      .def("error", &ImuFactor::error)
      .def(init<Key, Key, Key, Key, Key, const PreintegratedImuMeasurements&>())
      .def(repr(self));
}