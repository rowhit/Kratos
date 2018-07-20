//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Jordi Cotela
//

// System includes

#if defined(KRATOS_PYTHON)
// External includes
#include <pybind11/pybind11.h>


// Project includes
#include "includes/define_python.h"
#include "fluid_dynamics_application.h"
#include "custom_python/add_custom_strategies_to_python.h"
#include "custom_python/add_custom_utilities_to_python.h"
#include "custom_python/add_custom_processes_to_python.h"
#include "custom_python/add_custom_constitutive_laws_to_python.h"
#include "custom_python/add_custom_response_functions_to_python.h"


namespace Kratos
{

namespace Python
{

using namespace pybind11;


PYBIND11_MODULE(KratosFluidDynamicsApplication,m)
{

    class_<KratosFluidDynamicsApplication,
           KratosFluidDynamicsApplication::Pointer,
           KratosApplication >(m,"KratosFluidDynamicsApplication")
           .def(init<>())
           ;

    AddCustomConstitutiveLawsToPython(m);
    AddCustomProcessesToPython(m);
    AddCustomResponseFunctionsToPython(m);
    AddCustomStrategiesToPython(m);
    AddCustomUtilitiesToPython(m);

    //registering variables in python
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m,PATCH_INDEX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m,TAUONE);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m,TAUTWO);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m,PRESSURE_MASSMATRIX_COEFFICIENT);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m,Y_WALL);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m,SUBSCALE_PRESSURE);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m,C_DES);
//        KRATOS_REGISTER_IN_PYTHON_VARIABLE(m,C_SMAGORINSKY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m,CHARACTERISTIC_VELOCITY);

    // For Non-Newtonian constitutive relations
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m,REGULARIZATION_COEFFICIENT);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m,BINGHAM_SMOOTHER);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m,GEL_STRENGTH);

    KRATOS_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m,SUBSCALE_VELOCITY);
    KRATOS_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m,COARSE_VELOCITY);

    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m,FIC_BETA);

    // Embedded fluid variables
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m,EMBEDDED_IS_ACTIVE);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m,EMBEDDED_WET_PRESSURE);
    KRATOS_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m,EMBEDDED_WET_VELOCITY);

    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m,Q_VALUE);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m,VORTICITY_MAGNITUDE);
    KRATOS_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS(m,RECOVERED_PRESSURE_GRADIENT);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m,NODAL_WEIGHTS);

    // Compressible fluid variables
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m,HEAT_CAPACITY_RATIO);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m,REACTION_DENSITY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m,REACTION_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m,MACH);

    // Transient adjoint fluid variables
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m,ARTIFICIAL_DIFFUSION);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m,ADJOINT_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m,DIFFUSION_ENERGY);

    // ------------------- Testing Variables --------------------
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_1_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_2_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_3_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_4_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_5_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_6_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_7_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_8_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_9_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_10_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_11_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_12_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_13_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_14_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_15_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_16_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_17_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_18_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_19_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_20_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_21_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_22_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_23_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_24_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_25_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_26_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_27_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_28_EIGEN_MIN);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_29_EIGEN_MIN);

    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_1_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_2_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_3_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_4_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_5_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_6_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_7_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_8_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_9_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_10_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_11_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_12_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_13_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_14_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_15_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_16_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_17_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_18_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_19_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_20_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_21_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_22_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_23_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_24_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_25_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_26_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_27_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_28_EIGEN_MAX);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_29_EIGEN_MAX);

    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_1_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_2_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_3_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_4_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_5_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_6_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_7_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_8_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_9_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_10_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_11_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_12_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_13_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_14_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_15_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_16_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_17_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_18_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_19_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_20_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_21_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_22_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_23_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_24_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_25_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_26_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_27_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_28_ENERGY);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_29_ENERGY);

    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_24);
    KRATOS_REGISTER_IN_PYTHON_VARIABLE(m, STABILIZATION_ANALYSIS_MATRIX_29);

}


}  // namespace Python.

}  // namespace Kratos.

#endif // KRATOS_PYTHON defined
