//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:
//

// System includes

// External includes

// Project includes
#include "containers/model.h"
#include "testing/testing.h"

// Application includes
#include "custom_utilities/test_utilities.h"
#include "rans_application_variables.h"
#include "test_utilities.h"

namespace Kratos
{
namespace Testing
{
namespace
{
ModelPart& RansEvmKEpsilonKAFC2D3N_SetUp(Model& rModel)
{
    ModelPart& r_model_part = EvmKEpsilonTestUtilities::RansEvmKEpsilonK2D3N_SetUp(
        rModel, "RansEvmKEpsilonKAFC2D3N");

    return r_model_part;
}

ModelPart& RansEvmKEpsilonEpsilonAFC2D3N_SetUp(Model& rModel)
{
    ModelPart& r_model_part =
        EvmKEpsilonTestUtilities::RansEvmKEpsilonEpsilon2D3N_SetUp(
            rModel, "RansEvmKEpsilonEpsilonAFC2D3N");

    return r_model_part;
}

} // namespace

KRATOS_TEST_CASE_IN_SUITE(RansEvmKEpsilonKAFC2D3N_EquationIdVector, KratosRansFastSuite)
{
    // Setup:
    Model model;
    auto& r_model_part = RansEvmKEpsilonKAFC2D3N_SetUp(model);

    // Test:
    RansApplicationTestUtilities::TestEquationIdVector<ModelPart::ElementsContainerType>(r_model_part);
}

KRATOS_TEST_CASE_IN_SUITE(RansEvmKEpsilonKAFC2D3N_GetDofList, KratosRansFastSuite)
{
    // Setup:
    Model model;
    auto& r_model_part = RansEvmKEpsilonKAFC2D3N_SetUp(model);

    // Test:
    RansApplicationTestUtilities::TestGetDofList<ModelPart::ElementsContainerType>(r_model_part, TURBULENT_KINETIC_ENERGY);
}

KRATOS_TEST_CASE_IN_SUITE(RansEvmKEpsilonKAFC2D3N_CalculateLocalSystem, KratosRansFastSuite)
{
    // Setup:
    Model model;
    auto& r_model_part = RansEvmKEpsilonKAFC2D3N_SetUp(model);

    // Test:
    Matrix LHS, ref_LHS;
    Vector RHS, ref_RHS(3);
    auto& r_element = r_model_part.Elements().front();
    r_element.CalculateLocalSystem(LHS, RHS, r_model_part.GetProcessInfo());

    // setting reference values
    ref_RHS[0] = 2.67829732581686741355e+00;
    ref_RHS[1] = 2.21576728463429173388e+00;
    ref_RHS[2] = 1.85563938248039850265e+00;
    ref_LHS = ZeroMatrix(3, 3);

    std::cout << std::scientific << std::setprecision(20) << std::endl
              << RHS[0] << std::endl
              << RHS[1] << std::endl
              << RHS[2] << std::endl;

    KRATOS_CHECK_VECTOR_NEAR(RHS, ref_RHS, 1e-12);
    KRATOS_CHECK_MATRIX_NEAR(LHS, ref_LHS, 1e-12);
}

KRATOS_TEST_CASE_IN_SUITE(RansEvmKEpsilonKAFC2D3N_CalculateRightHandSide, KratosRansFastSuite)
{
    // Setup:
    Model model;
    auto& r_model_part = RansEvmKEpsilonKAFC2D3N_SetUp(model);

    // Test:
    Vector RHS, ref_RHS(3);
    auto& r_element = r_model_part.Elements().front();
    r_element.CalculateRightHandSide(RHS, r_model_part.GetProcessInfo());

    // setting reference values
    ref_RHS[0] = 2.67829732581686741355e+00;
    ref_RHS[1] = 2.21576728463429173388e+00;
    ref_RHS[2] = 1.85563938248039850265e+00;

    KRATOS_CHECK_VECTOR_NEAR(RHS, ref_RHS, 1e-12);
}

KRATOS_TEST_CASE_IN_SUITE(RansEvmKEpsilonKAFC2D3N_CalculateLocalVelocityContribution,
                          KratosRansFastSuite)
{
    // Setup:
    Model model;
    auto& r_model_part = RansEvmKEpsilonKAFC2D3N_SetUp(model);

    // Test:
    Matrix LHS, ref_LHS(3, 3);
    Vector RHS, ref_RHS;
    auto& r_element = r_model_part.Elements().front();
    r_element.CalculateLocalVelocityContribution(LHS, RHS, r_model_part.GetProcessInfo());

    // setting reference values
    ref_LHS(0, 0) = 1.22769754240541288937e+02;
    ref_LHS(0, 1) = 7.87068967410592108536e+01;
    ref_LHS(0, 2) = 8.08415536207580629480e+01;
    ref_LHS(1, 0) = 7.98696041610999429849e+01;
    ref_LHS(1, 1) = 1.89393678950818809881e+02;
    ref_LHS(1, 2) = 9.45264332693715516598e+01;
    ref_LHS(2, 0) = 8.11121632251401223357e+01;
    ref_LHS(2, 1) = 9.43911767926530984596e+01;
    ref_LHS(2, 2) = 1.98076344376050968776e+02;

    std::cout << std::scientific << std::setprecision(20) << std::endl
              << LHS(0, 0) << std::endl
              << LHS(0, 1) << std::endl
              << LHS(0, 2) << std::endl
              << LHS(1, 0) << std::endl
              << LHS(1, 1) << std::endl
              << LHS(1, 2) << std::endl
              << LHS(2, 0) << std::endl
              << LHS(2, 1) << std::endl
              << LHS(2, 2) << std::endl;

    KRATOS_CHECK_VECTOR_NEAR(RHS, ref_RHS, 1e-12);
    KRATOS_CHECK_MATRIX_NEAR(LHS, ref_LHS, 1e-12);
}

KRATOS_TEST_CASE_IN_SUITE(RansEvmKEpsilonKAFC2D3N_CalculateMassMatrix, KratosRansFastSuite)
{
    // Setup:
    Model model;
    auto& r_model_part = RansEvmKEpsilonKAFC2D3N_SetUp(model);

    // Test:
    Matrix M, ref_M;
    auto& r_element = r_model_part.Elements().front();
    r_element.CalculateMassMatrix(M, r_model_part.GetProcessInfo());

    // setting reference values
    ref_M = ZeroMatrix(3, 3);
    ref_M(0, 0) = 1.66666666666666657415e-01;
    ref_M(1, 1) = 1.66666666666666657415e-01;
    ref_M(2, 2) = 1.66666666666666657415e-01;

    std::cout << std::scientific << std::setprecision(20) << std::endl
              << M(0, 0) << std::endl
              << M(1, 1) << std::endl
              << M(2, 2) << std::endl;

    KRATOS_CHECK_MATRIX_NEAR(M, ref_M, 1e-12);
}

KRATOS_TEST_CASE_IN_SUITE(RansEvmKEpsilonKAFC2D3N_CalculateDampingMatrix, KratosRansFastSuite)
{
    // Setup:
    Model model;
    auto& r_model_part = RansEvmKEpsilonKAFC2D3N_SetUp(model);

    // Test:
    Matrix D, ref_D(3, 3);
    auto& r_element = r_model_part.Elements().front();
    r_element.CalculateDampingMatrix(D, r_model_part.GetProcessInfo());

    // setting reference values
    ref_D(0, 0) = 1.22769754240541288937e+02;
    ref_D(0, 1) = 7.87068967410592108536e+01;
    ref_D(0, 2) = 8.08415536207580629480e+01;
    ref_D(1, 0) = 7.98696041610999429849e+01;
    ref_D(1, 1) = 1.89393678950818809881e+02;
    ref_D(1, 2) = 9.45264332693715516598e+01;
    ref_D(2, 0) = 8.11121632251401223357e+01;
    ref_D(2, 1) = 9.43911767926530984596e+01;
    ref_D(2, 2) = 1.98076344376050968776e+02;

    KRATOS_CHECK_MATRIX_NEAR(D, ref_D, 1e-12);
}

KRATOS_TEST_CASE_IN_SUITE(RansEvmKEpsilonEpsilonAFC2D3N_EquationIdVector, KratosRansFastSuite)
{
    // Setup:
    Model model;
    auto& r_model_part = RansEvmKEpsilonEpsilonAFC2D3N_SetUp(model);

    // Test:
    RansApplicationTestUtilities::TestEquationIdVector<ModelPart::ElementsContainerType>(r_model_part);
}

KRATOS_TEST_CASE_IN_SUITE(RansEvmKEpsilonEpsilonAFC2D3N_GetDofList, KratosRansFastSuite)
{
    // Setup:
    Model model;
    auto& r_model_part = RansEvmKEpsilonEpsilonAFC2D3N_SetUp(model);

    // Test:
    RansApplicationTestUtilities::TestGetDofList<ModelPart::ElementsContainerType>(r_model_part, TURBULENT_ENERGY_DISSIPATION_RATE);
}

KRATOS_TEST_CASE_IN_SUITE(RansEvmKEpsilonEpsilonAFC2D3N_CalculateLocalSystem, KratosRansFastSuite)
{
    // Setup:
    Model model;
    auto& r_model_part = RansEvmKEpsilonEpsilonAFC2D3N_SetUp(model);

    // Test:
    Matrix LHS, ref_LHS;
    Vector RHS, ref_RHS(3);
    auto& r_element = r_model_part.Elements().front();
    r_element.CalculateLocalSystem(LHS, RHS, r_model_part.GetProcessInfo());

    // setting reference values
    ref_RHS[0] = 6.86298908682792080072e+03;
    ref_RHS[1] = 7.58397111019992553338e+03;
    ref_RHS[2] = 6.36469340548209856934e+03;
    ref_LHS = ZeroMatrix(3, 3);

    std::cout << std::scientific << std::setprecision(20) << std::endl
              << RHS[0] << std::endl
              << RHS[1] << std::endl
              << RHS[2] << std::endl;

    KRATOS_CHECK_VECTOR_NEAR(RHS, ref_RHS, 1e-12);
    KRATOS_CHECK_MATRIX_NEAR(LHS, ref_LHS, 1e-12);
}

KRATOS_TEST_CASE_IN_SUITE(RansEvmKEpsilonEpsilonAFC2D3N_CalculateRightHandSide, KratosRansFastSuite)
{
    // Setup:
    Model model;
    auto& r_model_part = RansEvmKEpsilonEpsilonAFC2D3N_SetUp(model);

    // Test:
    Vector RHS, ref_RHS(3);
    auto& r_element = r_model_part.Elements().front();
    r_element.CalculateRightHandSide(RHS, r_model_part.GetProcessInfo());

    // setting reference values
    ref_RHS[0] = 6.86298908682792080072e+03;
    ref_RHS[1] = 7.58397111019992553338e+03;
    ref_RHS[2] = 6.36469340548209856934e+03;

    KRATOS_CHECK_VECTOR_NEAR(RHS, ref_RHS, 1e-12);
}

KRATOS_TEST_CASE_IN_SUITE(RansEvmKEpsilonEpsilonAFC2D3N_CalculateLocalVelocityContribution,
                          KratosRansFastSuite)
{
    // Setup:
    Model model;
    auto& r_model_part = RansEvmKEpsilonEpsilonAFC2D3N_SetUp(model);

    // Test:
    Matrix LHS, ref_LHS(3, 3);
    Vector RHS, ref_RHS;
    auto& r_element = r_model_part.Elements().front();
    r_element.CalculateLocalVelocityContribution(LHS, RHS, r_model_part.GetProcessInfo());

    // setting reference values
    ref_LHS(0, 0) = 2.69302623925202681221e+02;
    ref_LHS(0, 1) = 1.74071444769769726690e+02;
    ref_LHS(0, 2) = 1.78133398955370807926e+02;
    ref_LHS(1, 0) = 1.75234152189810458822e+02;
    ref_LHS(1, 1) = 4.17367364117803390400e+02;
    ref_LHS(1, 2) = 2.08143277256379178652e+02;
    ref_LHS(2, 0) = 1.78404008559752867313e+02;
    ref_LHS(2, 1) = 2.08008020779660739663e+02;
    ref_LHS(2, 2) = 4.35870693852198144214e+02;

    std::cout << std::scientific << std::setprecision(20) << std::endl
              << LHS(0, 0) << std::endl
              << LHS(0, 1) << std::endl
              << LHS(0, 2) << std::endl
              << LHS(1, 0) << std::endl
              << LHS(1, 1) << std::endl
              << LHS(1, 2) << std::endl
              << LHS(2, 0) << std::endl
              << LHS(2, 1) << std::endl
              << LHS(2, 2) << std::endl;

    KRATOS_CHECK_VECTOR_NEAR(RHS, ref_RHS, 1e-12);
    KRATOS_CHECK_MATRIX_NEAR(LHS, ref_LHS, 1e-12);
}

KRATOS_TEST_CASE_IN_SUITE(RansEvmKEpsilonEpsilonAFC2D3N_CalculateMassMatrix, KratosRansFastSuite)
{
    // Setup:
    Model model;
    auto& r_model_part = RansEvmKEpsilonEpsilonAFC2D3N_SetUp(model);

    // Test:
    Matrix M, ref_M;
    auto& r_element = r_model_part.Elements().front();
    r_element.CalculateMassMatrix(M, r_model_part.GetProcessInfo());

    // setting reference values
    ref_M = ZeroMatrix(3, 3);
    ref_M(0, 0) = 1.66666666666666657415e-01;
    ref_M(1, 1) = 1.66666666666666657415e-01;
    ref_M(2, 2) = 1.66666666666666657415e-01;

    std::cout << std::scientific << std::setprecision(20) << std::endl
              << M(0, 0) << std::endl
              << M(1, 1) << std::endl
              << M(2, 2) << std::endl;

    KRATOS_CHECK_MATRIX_NEAR(M, ref_M, 1e-12);
}

KRATOS_TEST_CASE_IN_SUITE(RansEvmKEpsilonEpsilonAFC2D3N_CalculateDampingMatrix, KratosRansFastSuite)
{
    // Setup:
    Model model;
    auto& r_model_part = RansEvmKEpsilonEpsilonAFC2D3N_SetUp(model);

    // Test:
    Matrix D, ref_D(3, 3);
    auto& r_element = r_model_part.Elements().front();
    r_element.CalculateDampingMatrix(D, r_model_part.GetProcessInfo());

    // setting reference values
    ref_D(0, 0) = 2.69302623925202681221e+02;
    ref_D(0, 1) = 1.74071444769769726690e+02;
    ref_D(0, 2) = 1.78133398955370807926e+02;
    ref_D(1, 0) = 1.75234152189810458822e+02;
    ref_D(1, 1) = 4.17367364117803390400e+02;
    ref_D(1, 2) = 2.08143277256379178652e+02;
    ref_D(2, 0) = 1.78404008559752867313e+02;
    ref_D(2, 1) = 2.08008020779660739663e+02;
    ref_D(2, 2) = 4.35870693852198144214e+02;

    KRATOS_CHECK_MATRIX_NEAR(D, ref_D, 1e-12);
}

} // namespace Testing
} // namespace Kratos.