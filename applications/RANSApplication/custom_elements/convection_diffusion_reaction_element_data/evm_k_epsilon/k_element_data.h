//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Suneth Warnakulasuriya (https://github.com/sunethwarna)
//

#if !defined(KRATOS_EVM_K_EPSILON_HIGH_RE_ELEMENT_DATA_K_ELEMENT_DATA_H_INCLUDED)
#define KRATOS_EVM_K_EPSILON_HIGH_RE_ELEMENT_DATA_K_ELEMENT_DATA_H_INCLUDED

// System includes

// Project includes
#include "containers/variable.h"
#include "geometries/geometry_data.h"
#include "includes/node.h"
#include "includes/process_info.h"
#include "includes/ublas_interface.h"

// Application includes
#include "custom_elements/convection_diffusion_reaction_element_data.h"

namespace Kratos
{
///@name Kratos Globals
///@{

///@}
///@name Type Definitions
///@{

///@}
///@name  Enum's
///@{

///@}
///@name  Functions
///@{

namespace EvmKEpsilonElementData
{
template <unsigned int TDim>
class KElementData : public ConvectionDiffusionReactionElementData
{
public:
    using BaseType = ConvectionDiffusionReactionElementData;
    using NodeType = Node<3>;
    using GeomtryType = BaseType::GeometryType;

    static const Variable<double>& GetScalarVariable();
    static const Variable<double>& GetScalarRateVariable();
    static const Variable<double>& GetScalarRelaxedRateVariable();

    static void Check(const GeometryType& rGeometry, const ProcessInfo& rCurrentProcessInfo);

    static GeometryData::IntegrationMethod GetIntegrationMethod();

    static const std::string GetName()
    {
        return "KEpsilonKElementData";
    }

    KElementData(const GeomtryType& rGeometry) : BaseType(rGeometry)
    {
    }

    void CalculateConstants(const ProcessInfo& rCurrentProcessInfo) override;

    void CalculateGaussPointData(const Vector& rShapeFunctions,
                                 const Matrix& rShapeFunctionDerivatives,
                                 const int Step = 0) override;

    array_1d<double, 3> CalculateEffectiveVelocity(const Vector& rShapeFunctions,
                                                   const Matrix& rShapeFunctionDerivatives) const override;

    double CalculateEffectiveKinematicViscosity(const Vector& rShapeFunctions,
                                                const Matrix& rShapeFunctionDerivatives) const override;

    double CalculateReactionTerm(const Vector& rShapeFunctions,
                                 const Matrix& rShapeFunctionDerivatives) const override;

    double CalculateSourceTerm(const Vector& rShapeFunctions,
                               const Matrix& rShapeFunctionDerivatives) const override;

protected:
    BoundedMatrix<double, TDim, TDim> mVelocityGradient;

    double mGamma;
    double mTurbulentKinematicViscosity;
    double mKinematicViscosity;
    double mVelocityDivergence;
    double mInvTkeSigma;
    double mCmu;
};

///@}

} // namespace EvmKEpsilonElementData

} // namespace Kratos

#endif