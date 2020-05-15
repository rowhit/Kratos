//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:     Eloisa Baez Jones, Inigo Lopez and Riccardo Rossi
//

#include "transonic_perturbation_potential_flow_element.h"
#include "compressible_potential_flow_application_variables.h"
#include "includes/cfd_variables.h"
#include "includes/kratos_flags.h"
#include "fluid_dynamics_application_variables.h"
#include "custom_utilities/potential_flow_utilities.h"
#include "utilities/geometry_utilities.h"
#include "utilities/enrichment_utilities.h"

namespace Kratos
{
///////////////////////////////////////////////////////////////////////////////////////////////////
// Public Operations

template <int TDim, int TNumNodes>
Element::Pointer TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::Create(
    IndexType NewId,
    NodesArrayType const& ThisNodes,
    PropertiesType::Pointer pProperties) const
{
    KRATOS_TRY
    return Kratos::make_intrusive<TransonicPerturbationPotentialFlowElement>(
        NewId, GetGeometry().Create(ThisNodes), pProperties);
    KRATOS_CATCH("");
}

template <int TDim, int TNumNodes>
Element::Pointer TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::Create(
    IndexType NewId,
    GeometryType::Pointer pGeom,
    PropertiesType::Pointer pProperties) const
{
    KRATOS_TRY
    return Kratos::make_intrusive<TransonicPerturbationPotentialFlowElement>(NewId, pGeom, pProperties);
    KRATOS_CATCH("");
}

template <int TDim, int TNumNodes>
Element::Pointer TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::Clone(
    IndexType NewId,
    NodesArrayType const& ThisNodes) const
{
    KRATOS_TRY
    return Kratos::make_intrusive<TransonicPerturbationPotentialFlowElement>(
        NewId, GetGeometry().Create(ThisNodes), pGetProperties());
    KRATOS_CATCH("");
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::Initialize(const ProcessInfo& rCurrentProcessInfo)
{
    FindUpwindElement(rCurrentProcessInfo);
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::CalculateLocalSystem(
    MatrixType& rLeftHandSideMatrix,
    VectorType& rRightHandSideVector,
    ProcessInfo& rCurrentProcessInfo)
{
    CalculateRightHandSide(rRightHandSideVector,rCurrentProcessInfo);
    CalculateLeftHandSide(rLeftHandSideMatrix,rCurrentProcessInfo);
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::CalculateRightHandSide(
    VectorType& rRightHandSideVector,
    ProcessInfo& rCurrentProcessInfo)
{
    const TransonicPerturbationPotentialFlowElement& r_this = *this;
    const int wake = r_this.GetValue(WAKE);

    if (wake == 0) // Normal element (non-wake) - eventually an embedded
    {
        if (this->Is(INLET))
        {
            CalculateRightHandSideInletElement(rRightHandSideVector, rCurrentProcessInfo);
        }
        else
        {
            CalculateRightHandSideNormalElement(rRightHandSideVector, rCurrentProcessInfo);
        }
    }
    else // Wake element
    {
        CalculateRightHandSideWakeElement(rRightHandSideVector, rCurrentProcessInfo);

    }
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::CalculateLeftHandSide(
    MatrixType& rLeftHandSideMatrix,
    ProcessInfo& rCurrentProcessInfo)
{
    const TransonicPerturbationPotentialFlowElement& r_this = *this;
    const int wake = r_this.GetValue(WAKE);

    if (wake == 0) // Normal element (non-wake) - eventually an embedded
    {
        if (this->Is(INLET))
        {
            CalculateLeftHandSideInletElement(rLeftHandSideMatrix, rCurrentProcessInfo);
        }
        else
        {
            CalculateLeftHandSideNormalElement(rLeftHandSideMatrix, rCurrentProcessInfo);
        }
    }
    else // Wake element
    {
        CalculateLeftHandSideWakeElement(rLeftHandSideMatrix, rCurrentProcessInfo);
    }
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::EquationIdVector(
    EquationIdVectorType& rResult,
    ProcessInfo& CurrentProcessInfo)
{
    const TransonicPerturbationPotentialFlowElement& r_this = *this;
    const int wake = r_this.GetValue(WAKE);
    const int number_of_nodes = 4;

    std::cout << "equation id vector\n";
    if (wake == 0) // Normal element
    {
        if (rResult.size() != TNumNodes)
        {
             rResult.resize(TNumNodes, false);
        }

        const int kutta = r_this.GetValue(KUTTA);

        if (kutta == 0 && this->Is(INLET))
        {
            std::cout << "GetEquationIdVectorInletElement\n";
            GetEquationIdVectorInletElement(rResult);
        }
        else if (kutta == 0 && this->IsNot(INLET))
        {
            if (rResult.size() != number_of_nodes)
            {
                rResult.resize(number_of_nodes, false);
            }

            std::cout << "GetEquationIdVectorNormalElement\n";
            GetEquationIdVectorNormalElement(rResult);
        }
        else
        {
            std::cout << "GetEquationIdVectorKuttaElement\n";
            GetEquationIdVectorKuttaElement(rResult);
        }
    }
    else // Wake element
    {
        if (rResult.size() != 2 * TNumNodes)
        {
            rResult.resize(2 * TNumNodes, false);
        }
        std::cout << "GetEquationIdVectorWakeElement\n";
        GetEquationIdVectorWakeElement(rResult);
    }
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::GetDofList(DofsVectorType& rElementalDofList,
                                                                 ProcessInfo& CurrentProcessInfo)
{
    const TransonicPerturbationPotentialFlowElement& r_this = *this;
    const int wake = r_this.GetValue(WAKE);

    if (wake == 0) // Normal element
    {
        if (rElementalDofList.size() != TNumNodes)
        {
            rElementalDofList.resize(TNumNodes);
        }

        const int kutta = r_this.GetValue(KUTTA);

        if (kutta == 0)
        {
            GetDofListNormalElement(rElementalDofList);

        }
        else
        {
            GetDofListKuttaElement(rElementalDofList);
        }
    }
    else // wake element
    {
        if (rElementalDofList.size() != 2 * TNumNodes)
        {
            rElementalDofList.resize(2 * TNumNodes);
        }

        GetDofListWakeElement(rElementalDofList);
    }
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::FinalizeSolutionStep(ProcessInfo& rCurrentProcessInfo)
{
    const bool active = Is(ACTIVE);

    const TransonicPerturbationPotentialFlowElement& r_const_this = *this;
    const int wake = r_const_this.GetValue(WAKE);

    if (wake != 0 && active == true)
    {
        ComputePotentialJump(rCurrentProcessInfo);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Inquiry

template <int TDim, int TNumNodes>
int TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::Check(const ProcessInfo& rCurrentProcessInfo)
{
    KRATOS_TRY

    // Generic geometry check
    int out = Element::Check(rCurrentProcessInfo);
    if (out != 0)
    {
        return out;
    }

    KRATOS_ERROR_IF(GetGeometry().Area() <= 0.0)
        << this->Id() << "Area cannot be less than or equal to 0" << std::endl;

    for (unsigned int i = 0; i < this->GetGeometry().size(); i++)
    {
        KRATOS_CHECK_VARIABLE_IN_NODAL_DATA(VELOCITY_POTENTIAL, this->GetGeometry()[i]);
    }

    return out;

    KRATOS_CATCH("");
}

///////////////////////////////////////////////////////////////////////////////////////////////////

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::GetValueOnIntegrationPoints(
    const Variable<double>& rVariable,
    std::vector<double>& rValues,
    const ProcessInfo& rCurrentProcessInfo)
{
    if (rValues.size() != 1)
    {
        rValues.resize(1);
    }
    if (rVariable == PRESSURE_COEFFICIENT)
    {
        rValues[0] = PotentialFlowUtilities::ComputePerturbationCompressiblePressureCoefficient<TDim, TNumNodes>(*this, rCurrentProcessInfo);
    }
    else if (rVariable == DENSITY)
    {
        rValues[0] = ComputeDensity(rCurrentProcessInfo);
    }
    else if (rVariable == MACH)
    {
        rValues[0] = PotentialFlowUtilities::ComputePerturbationLocalMachNumber<TDim, TNumNodes>(*this, rCurrentProcessInfo);
    }
    else if (rVariable == SOUND_VELOCITY)
    {
        rValues[0] = PotentialFlowUtilities::ComputePerturbationLocalSpeedOfSound<TDim, TNumNodes>(*this, rCurrentProcessInfo);
    }
    else if (rVariable == WAKE)
    {
        const TransonicPerturbationPotentialFlowElement& r_this = *this;
        rValues[0] = r_this.GetValue(WAKE);
    }
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::GetValueOnIntegrationPoints(
    const Variable<int>& rVariable,
    std::vector<int>& rValues,
    const ProcessInfo& rCurrentProcessInfo)
{
    if (rValues.size() != 1)
    {
        rValues.resize(1);
    }
    if (rVariable == TRAILING_EDGE)
    {
        rValues[0] = this->GetValue(TRAILING_EDGE);
    }
    else if (rVariable == KUTTA)
    {
        rValues[0] = this->GetValue(KUTTA);
    }
    else if (rVariable == WAKE)
    {
        rValues[0] = this->GetValue(WAKE);
    }
    else if (rVariable == ZERO_VELOCITY_CONDITION)
    {
        rValues[0] = this->GetValue(ZERO_VELOCITY_CONDITION);
    }
    else if (rVariable == TRAILING_EDGE_ELEMENT)
    {
        rValues[0] = this->GetValue(TRAILING_EDGE_ELEMENT);
    }
    else if (rVariable == DECOUPLED_TRAILING_EDGE_ELEMENT)
    {
        rValues[0] = this->GetValue(DECOUPLED_TRAILING_EDGE_ELEMENT);
    }
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::GetValueOnIntegrationPoints(
    const Variable<array_1d<double, 3>>& rVariable,
    std::vector<array_1d<double, 3>>& rValues,
    const ProcessInfo& rCurrentProcessInfo)
{
    if (rValues.size() != 1)
    {
        rValues.resize(1);
    }
    if (rVariable == VELOCITY)
    {
        const array_1d<double, 3> free_stream_velocity = rCurrentProcessInfo[FREE_STREAM_VELOCITY];
        array_1d<double, 3> v(3, 0.0);
        array_1d<double, TDim> vaux = PotentialFlowUtilities::ComputeVelocity<TDim, TNumNodes>(*this);
        for (unsigned int k = 0; k < TDim; k++)
        {
            v[k] = vaux[k] + free_stream_velocity[k];
        }
        rValues[0] = v;
    }
    else if (rVariable == PERTURBATION_VELOCITY)
    {
        array_1d<double, 3> v(3, 0.0);
        array_1d<double, TDim> vaux = PotentialFlowUtilities::ComputeVelocity<TDim,TNumNodes>(*this);
        for (unsigned int k = 0; k < TDim; k++)
        {
            v[k] = vaux[k];
        }
        rValues[0] = v;
    }
    else if (rVariable == VECTOR_TO_UPWIND_ELEMENT)
    {
        // rValues[0] = this->GetValue(VECTOR_TO_UPWIND_ELEMENT);
        rValues[0] = pGetUpwindElement()->GetGeometry().Center() - this->GetGeometry().Center();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Input and output

template <int TDim, int TNumNodes>
std::string TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::Info() const
{
    std::stringstream buffer;
    buffer << "TransonicPerturbationPotentialFlowElement #" << Id();
    return buffer.str();
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::PrintInfo(std::ostream& rOStream) const
{
    rOStream << "TransonicPerturbationPotentialFlowElement #" << Id();
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::PrintData(std::ostream& rOStream) const
{
    pGetGeometry()->PrintData(rOStream);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Private functions
///////////////////////////////////////////////////////////////////////////////////////////////////

template <int TDim, int TNumNodes>
inline GlobalPointer<Element> TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::pGetUpwindElement() const
{
    KRATOS_ERROR_IF(mpUpwindElement.get() == nullptr)
        << "No upwind element found for element #" << this->Id() << std::endl;
    return mpUpwindElement;
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::GetWakeDistances(
    array_1d<double,
    TNumNodes>& distances) const
{
    noalias(distances) = GetValue(WAKE_ELEMENTAL_DISTANCES);
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::GetEquationIdVectorNormalElement(
    EquationIdVectorType& rResult) const
{
    const int additional_node_id = GetAdditionalNode();

    KRATOS_WATCH(additional_node_id);
    KRATOS_WATCH(this->Id());

    const auto& r_geometry = this->GetGeometry();
    for (unsigned int i = 0; i < TNumNodes; i++)
    {
        rResult[i] = r_geometry[i].GetDof(VELOCITY_POTENTIAL).EquationId();
    }
    std::cout << "after for loop\n";
    const auto& r_upwind_geometry = mpUpwindElement->GetGeometry();
    KRATOS_WATCH(rResult.size());
    KRATOS_WATCH(r_upwind_geometry[additional_node_id]);
    rResult[TNumNodes + 1] = r_upwind_geometry[additional_node_id].GetDof(VELOCITY_POTENTIAL).EquationId();
    std::cout << "after rResult assignment\n";
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::GetEquationIdVectorInletElement(
    EquationIdVectorType& rResult) const
{
    for (unsigned int i = 0; i < TNumNodes; i++)
    {
        rResult[i] = GetGeometry()[i].GetDof(VELOCITY_POTENTIAL).EquationId();
    }
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::GetEquationIdVectorKuttaElement(
    EquationIdVectorType& rResult) const
{
    const auto& r_geometry = this->GetGeometry();
    // Kutta elements have only negative part
    for (unsigned int i = 0; i < TNumNodes; i++)
    {
        if (!r_geometry[i].GetValue(TRAILING_EDGE))
        {
            rResult[i] = r_geometry[i].GetDof(VELOCITY_POTENTIAL).EquationId();
        }
        else
        {
            rResult[i] = r_geometry[i].GetDof(AUXILIARY_VELOCITY_POTENTIAL).EquationId();
        }
    }
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::GetEquationIdVectorWakeElement(
    EquationIdVectorType& rResult) const
{
    array_1d<double, TNumNodes> distances;
    GetWakeDistances(distances);

    // Positive part
    for (unsigned int i = 0; i < TNumNodes; i++)
    {
        if (distances[i] > 0.0)
        {
            rResult[i] = GetGeometry()[i].GetDof(VELOCITY_POTENTIAL).EquationId();
        }
        else
        {
            rResult[i] =
                GetGeometry()[i].GetDof(AUXILIARY_VELOCITY_POTENTIAL, 0).EquationId();
        }
    }

    // Negative part - sign is opposite to the previous case
    for (unsigned int i = 0; i < TNumNodes; i++)
    {
        if (distances[i] < 0.0)
        {
            rResult[TNumNodes + i + 1] =
                GetGeometry()[i].GetDof(VELOCITY_POTENTIAL).EquationId();
        }
        else
        {
            rResult[TNumNodes + i + 1] =
                GetGeometry()[i].GetDof(AUXILIARY_VELOCITY_POTENTIAL).EquationId();
        }
    }

}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::GetDofListNormalElement(DofsVectorType& rElementalDofList) const
{
    for (unsigned int i = 0; i < TNumNodes; i++)
    {
        rElementalDofList[i] = GetGeometry()[i].pGetDof(VELOCITY_POTENTIAL);
    }
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::GetDofListKuttaElement(DofsVectorType& rElementalDofList) const
{
    const auto& r_geometry = this->GetGeometry();
    // Kutta elements have only negative part
    for (unsigned int i = 0; i < TNumNodes; i++)
    {
        if (!r_geometry[i].GetValue(TRAILING_EDGE))
        {
                rElementalDofList[i] = r_geometry[i].pGetDof(VELOCITY_POTENTIAL);
        }
        else
        {
                rElementalDofList[i] = r_geometry[i].pGetDof(AUXILIARY_VELOCITY_POTENTIAL);
        }
    }
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::GetDofListWakeElement(DofsVectorType& rElementalDofList) const
{
    array_1d<double, TNumNodes> distances;
    GetWakeDistances(distances);

    // Positive part
    for (unsigned int i = 0; i < TNumNodes; i++)
    {
        if (distances[i] > 0)
        {
            rElementalDofList[i] = GetGeometry()[i].pGetDof(VELOCITY_POTENTIAL);
        }
        else
        {
            rElementalDofList[i] = GetGeometry()[i].pGetDof(AUXILIARY_VELOCITY_POTENTIAL);
        }
    }

    // Negative part - sign is opposite to the previous case
    for (unsigned int i = 0; i < TNumNodes; i++)
    {
        if (distances[i] < 0)
        {
            rElementalDofList[TNumNodes + i] = GetGeometry()[i].pGetDof(VELOCITY_POTENTIAL);
        }
        else
        {
            rElementalDofList[TNumNodes + i] =
                GetGeometry()[i].pGetDof(AUXILIARY_VELOCITY_POTENTIAL);
        }
    }
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::CalculateLeftHandSideInletElement(
    MatrixType& rLeftHandSideMatrix,
    const ProcessInfo& rCurrentProcessInfo)
{
    if (rLeftHandSideMatrix.size1() != TNumNodes || rLeftHandSideMatrix.size2() != TNumNodes)
    {
        rLeftHandSideMatrix.resize(TNumNodes, TNumNodes, false);
    }
    rLeftHandSideMatrix.clear();

    ElementalData<TNumNodes, TDim> data;

    // Calculate shape functions
    GeometryUtils::CalculateGeometryData(GetGeometry(), data.DN_DX, data.N, data.vol);

    const double density = ComputeDensity(rCurrentProcessInfo);
    const double DrhoDu2 = ComputeDensityDerivative(density, rCurrentProcessInfo);

    // Computing local velocity
    const array_1d<double, 3> free_stream_velocity = rCurrentProcessInfo[FREE_STREAM_VELOCITY];
    array_1d<double, TDim> velocity = PotentialFlowUtilities::ComputeVelocity<TDim,TNumNodes>(*this);
    for (unsigned int i = 0; i < TDim; i++)
    {
        velocity[i] += free_stream_velocity[i];
    }

    const BoundedVector<double, TNumNodes> DNV = prod(data.DN_DX, velocity);

    noalias(rLeftHandSideMatrix) +=
        data.vol * density * prod(data.DN_DX, trans(data.DN_DX));
    noalias(rLeftHandSideMatrix) += data.vol * 2 * DrhoDu2 * outer_prod(DNV, trans(DNV));

}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::CalculateLeftHandSideNormalElement(
    MatrixType& rLeftHandSideMatrix,
    const ProcessInfo& rCurrentProcessInfo)
{
    if (rLeftHandSideMatrix.size1() != TNumNodes + 1 || rLeftHandSideMatrix.size2() != TNumNodes + 1)
    {
        rLeftHandSideMatrix.resize(TNumNodes + 1, TNumNodes + 1, false);
    }
    rLeftHandSideMatrix.clear();
    std::cout << "calculate left hand side matrix\n";
    ElementalData<TNumNodes, TDim> data;

    // Calculate shape functions
    GeometryUtils::CalculateGeometryData(GetGeometry(), data.DN_DX, data.N, data.vol);
    std::cout << "LHS CalculateGeometryData\n";

    const double density = ComputeDensity(rCurrentProcessInfo);
    const double DrhoDu2 = ComputeDensityDerivative(density, rCurrentProcessInfo);

    // Computing local velocity
    const array_1d<double, 3> free_stream_velocity = rCurrentProcessInfo[FREE_STREAM_VELOCITY];
    array_1d<double, TDim> velocity = PotentialFlowUtilities::ComputeVelocity<TDim,TNumNodes>(*this);
    for (unsigned int i = 0; i < TDim; i++)
    {
        velocity[i] += free_stream_velocity[i];
    }
    std::cout << "LHS ComputeVelocity\n";
    const BoundedVector<double, TNumNodes> DNV = prod(data.DN_DX, velocity);
    std::cout << "LHS DNV\n";

    KRATOS_WATCH(prod(data.DN_DX, trans(data.DN_DX)));
    KRATOS_WATCH(outer_prod(DNV, trans(DNV)));

    if (rLeftHandSideMatrix.size1() != TNumNodes|| rLeftHandSideMatrix.size2() != TNumNodes)
    {
        rLeftHandSideMatrix.resize(TNumNodes, TNumNodes, false);
    }

    noalias(rLeftHandSideMatrix) +=
        data.vol * density * prod(data.DN_DX, trans(data.DN_DX));
    noalias(rLeftHandSideMatrix) += data.vol * 2 * DrhoDu2 * outer_prod(DNV, trans(DNV));
    std::cout << "LHS end\n";
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::CalculateRightHandSideInletElement(
    VectorType& rRightHandSideVector,
    const ProcessInfo& rCurrentProcessInfo)
{
    if (rRightHandSideVector.size() != TNumNodes)
    {
        rRightHandSideVector.resize(TNumNodes, false);
    }

    rRightHandSideVector.clear();

    ElementalData<TNumNodes, TDim> data;

    // Calculate shape functions
    GeometryUtils::CalculateGeometryData(GetGeometry(), data.DN_DX, data.N, data.vol);

    const double density = ComputeDensity(rCurrentProcessInfo);

    // Computing local velocity
    const array_1d<double, 3> free_stream_velocity = rCurrentProcessInfo[FREE_STREAM_VELOCITY];
    array_1d<double, TDim> velocity = PotentialFlowUtilities::ComputeVelocity<TDim,TNumNodes>(*this);
    for (unsigned int i = 0; i < TDim; i++)
    {
        velocity[i] += free_stream_velocity[i];
    }

    noalias(rRightHandSideVector) = - data.vol * density * prod(data.DN_DX, velocity);
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::CalculateRightHandSideNormalElement(
    VectorType& rRightHandSideVector,
    const ProcessInfo& rCurrentProcessInfo)
{
    if (rRightHandSideVector.size() != TNumNodes + 1)
    {
        rRightHandSideVector.resize(TNumNodes + 1, false);
    }
    rRightHandSideVector.clear();

    ElementalData<TNumNodes, TDim> data;

    // Calculate shape functions
    GeometryUtils::CalculateGeometryData(GetGeometry(), data.DN_DX, data.N, data.vol);
        std::cout << "RHS CalculateGeometryData\n";

    const double density = ComputeDensity(rCurrentProcessInfo);
    std::cout << "RHS ComputeDensity\n";

    // Computing local velocity
    const array_1d<double, 3> free_stream_velocity = rCurrentProcessInfo[FREE_STREAM_VELOCITY];
    array_1d<double, TDim> velocity = PotentialFlowUtilities::ComputeVelocity<TDim,TNumNodes>(*this);
    for (unsigned int i = 0; i < TDim; i++)
    {
        velocity[i] += free_stream_velocity[i];
    }
    std::cout << "RHS ComputeVelocity\n";

    KRATOS_WATCH(data.vol);
    KRATOS_WATCH(data.DN_DX);
    KRATOS_WATCH(velocity);
    KRATOS_WATCH(prod(data.DN_DX, velocity));

    if (rRightHandSideVector.size() != TNumNodes)
    {
        rRightHandSideVector.resize(TNumNodes, false);
    }
    
    noalias(rRightHandSideVector) = - data.vol * density * prod(data.DN_DX, velocity);
    std::cout << "end calculate right hand side vector\n";
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::CalculateLeftHandSideWakeElement(
    MatrixType& rLeftHandSideMatrix,
    const ProcessInfo& rCurrentProcessInfo)
{
    // Note that the lhs and rhs have double the size
    if (rLeftHandSideMatrix.size1() != 2 * TNumNodes ||
        rLeftHandSideMatrix.size2() != 2 * TNumNodes)
    {
        rLeftHandSideMatrix.resize(2 * TNumNodes, 2 * TNumNodes, false);
    }
    rLeftHandSideMatrix.clear();

    ElementalData<TNumNodes, TDim> data;

    // Calculate shape functions
    GeometryUtils::CalculateGeometryData(GetGeometry(), data.DN_DX, data.N, data.vol);
    GetWakeDistances(data.distances);

    const double density = ComputeDensity(rCurrentProcessInfo);
    const double DrhoDu2 = ComputeDensityDerivative(density, rCurrentProcessInfo);

    // Computing local velocity
    const array_1d<double, 3> free_stream_velocity = rCurrentProcessInfo[FREE_STREAM_VELOCITY];
    array_1d<double, TDim> velocity = PotentialFlowUtilities::ComputeVelocity<TDim,TNumNodes>(*this);
    for (unsigned int i = 0; i < TDim; i++)
    {
        velocity[i] += free_stream_velocity[i];
    }

    const BoundedVector<double, TNumNodes> DNV = prod(data.DN_DX, velocity);

    const BoundedMatrix<double, TNumNodes, TNumNodes> lhs_total =
        data.vol * density * prod(data.DN_DX, trans(data.DN_DX)) +
        data.vol * 2 * DrhoDu2 * outer_prod(DNV, trans(DNV));

    if (this->Is(STRUCTURE))
    {
        Matrix lhs_positive = ZeroMatrix(TNumNodes, TNumNodes);
        Matrix lhs_negative = ZeroMatrix(TNumNodes, TNumNodes);

        CalculateLeftHandSideSubdividedElement(lhs_positive, lhs_negative, rCurrentProcessInfo);
        AssignLeftHandSideSubdividedElement(rLeftHandSideMatrix, lhs_positive,
                                            lhs_negative, lhs_total, data);
    }
    else
    {
        AssignLeftHandSideWakeElement(rLeftHandSideMatrix, lhs_total, data);
    }
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::CalculateRightHandSideWakeElement(
    VectorType& rRightHandSideVector,
    const ProcessInfo& rCurrentProcessInfo)
{
    // Note that the rhs has double the size
    if (rRightHandSideVector.size() != 2 * TNumNodes)
    {
        rRightHandSideVector.resize(2 * TNumNodes, false);
    }
    rRightHandSideVector.clear();

    ElementalData<TNumNodes, TDim> data;

    // Calculate shape functions
    const auto& r_geometry = this->GetGeometry();
    GeometryUtils::CalculateGeometryData(r_geometry, data.DN_DX, data.N, data.vol);
    GetWakeDistances(data.distances);

    const double density = ComputeDensity(rCurrentProcessInfo);

    const array_1d<double, 3> free_stream_velocity = rCurrentProcessInfo[FREE_STREAM_VELOCITY];
    array_1d<double, TDim> upper_velocity = PotentialFlowUtilities::ComputeVelocityUpperWakeElement<TDim,TNumNodes>(*this);
    array_1d<double, TDim> lower_velocity = PotentialFlowUtilities::ComputeVelocityLowerWakeElement<TDim,TNumNodes>(*this);

    for (unsigned int i = 0; i < TDim; i++)
    {
        upper_velocity[i] += free_stream_velocity[i];
        lower_velocity[i] += free_stream_velocity[i];
    }
    const array_1d<double, TDim> diff_velocity = upper_velocity - lower_velocity;

    const BoundedVector<double, TNumNodes> upper_rhs = - data.vol * density * prod(data.DN_DX, upper_velocity);
    const BoundedVector<double, TNumNodes> lower_rhs = - data.vol * density * prod(data.DN_DX, lower_velocity);
    const BoundedVector<double, TNumNodes> wake_rhs = - data.vol * density * prod(data.DN_DX, diff_velocity);

    if (this->Is(STRUCTURE))
    {
        double upper_vol = 0.0;
        double lower_vol = 0.0;

        CalculateVolumesSubdividedElement(upper_vol, lower_vol, rCurrentProcessInfo);
        for (unsigned int i = 0; i < TNumNodes; ++i)
        {
            if (r_geometry[i].GetValue(TRAILING_EDGE))
            {
                rRightHandSideVector[i] = upper_rhs(i)*upper_vol/data.vol;
                rRightHandSideVector[i + TNumNodes] = lower_rhs(i)*lower_vol/data.vol;
            }
            else
            {
                AssignRightHandSideWakeNode(rRightHandSideVector, upper_rhs, lower_rhs, wake_rhs, data, i);
            }
        }
    }
    else
    {
        for (unsigned int i = 0; i < TNumNodes; ++i)
        {
            AssignRightHandSideWakeNode(rRightHandSideVector, upper_rhs, lower_rhs, wake_rhs, data, i);
        }
    }
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::CalculateLeftHandSideSubdividedElement(
    Matrix& lhs_positive,
    Matrix& lhs_negative,
    const ProcessInfo& rCurrentProcessInfo)
{
    ElementalData<TNumNodes, TDim> data;

    // Calculate shape functions
    GeometryUtils::CalculateGeometryData(GetGeometry(), data.DN_DX, data.N, data.vol);

    GetWakeDistances(data.distances);

    // Subdivide the element
    constexpr unsigned int nvolumes = 3 * (TDim - 1);
    BoundedMatrix<double, TNumNodes, TDim> Points;
    array_1d<double, nvolumes> partitions_sign;
    BoundedMatrix<double, nvolumes, TNumNodes> gp_shape_function_values;
    array_1d<double, nvolumes> Volumes;
    std::vector<Matrix> GradientsValue(nvolumes);
    BoundedMatrix<double, nvolumes, 2> N_enriched;
    for (unsigned int i = 0; i < GradientsValue.size(); ++i)
    {
        GradientsValue[i].resize(2, TDim, false);
    }
    for (unsigned int i = 0; i < TNumNodes; ++i)
    {
        const auto& r_coords = GetGeometry()[i].Coordinates();
        for (unsigned int k = 0; k < TDim; ++k)
        {
            Points(i, k) = r_coords[k];
        }
    }

    const unsigned int nsubdivisions = EnrichmentUtilities::CalculateEnrichedShapeFuncions(
        Points, data.DN_DX, data.distances, Volumes, gp_shape_function_values,
        partitions_sign, GradientsValue, N_enriched);

    const double density = ComputeDensity(rCurrentProcessInfo);
    const double DrhoDu2 = ComputeDensityDerivative(density, rCurrentProcessInfo);

    // Computing local velocity
    const array_1d<double, 3> free_stream_velocity = rCurrentProcessInfo[FREE_STREAM_VELOCITY];
    array_1d<double, TDim> velocity = PotentialFlowUtilities::ComputeVelocity<TDim,TNumNodes>(*this);
    for (unsigned int i = 0; i < TDim; i++)
    {
        velocity[i] += free_stream_velocity[i];
    }

    const BoundedVector<double, TNumNodes> DNV = prod(data.DN_DX, velocity);

    // Compute the lhs and rhs that would correspond to it being divided
    for (unsigned int i = 0; i < nsubdivisions; ++i)
    {
        if (partitions_sign[i] > 0)
        {
            noalias(lhs_positive) +=
                Volumes[i] * density * prod(data.DN_DX, trans(data.DN_DX));
            noalias(lhs_positive) +=
                Volumes[i] * 2 * DrhoDu2 * outer_prod(DNV, trans(DNV));
        }
        else
        {
            noalias(lhs_negative) +=
                Volumes[i] * density * prod(data.DN_DX, trans(data.DN_DX));
            noalias(lhs_negative) +=
                Volumes[i] * 2 * DrhoDu2 * outer_prod(DNV, trans(DNV));
        }
    }
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::CalculateVolumesSubdividedElement(
    double& rUpper_vol,
    double& rLower_vol,
    const ProcessInfo& rCurrentProcessInfo)
{
    ElementalData<TNumNodes, TDim> data;

    // Calculate shape functions
    GeometryUtils::CalculateGeometryData(GetGeometry(), data.DN_DX, data.N, data.vol);

    GetWakeDistances(data.distances);

    // Subdivide the element
    constexpr unsigned int nvolumes = 3 * (TDim - 1);
    BoundedMatrix<double, TNumNodes, TDim> Points;
    array_1d<double, nvolumes> PartitionsSign;
    BoundedMatrix<double, nvolumes, TNumNodes> GPShapeFunctionValues;
    array_1d<double, nvolumes> Volumes;
    std::vector<Matrix> GradientsValue(nvolumes);
    BoundedMatrix<double, nvolumes, 2> NEnriched;
    for (unsigned int i = 0; i < GradientsValue.size(); ++i)
    {
        GradientsValue[i].resize(2, TDim, false);
    }
    for (unsigned int i = 0; i < TNumNodes; ++i)
    {
        const array_1d<double, 3>& coords = GetGeometry()[i].Coordinates();
        for (unsigned int k = 0; k < TDim; ++k)
        {
            Points(i, k) = coords[k];
        }
    }

    const unsigned int nsubdivisions = EnrichmentUtilities::CalculateEnrichedShapeFuncions(
        Points, data.DN_DX, data.distances, Volumes, GPShapeFunctionValues,
        PartitionsSign, GradientsValue, NEnriched);

    // Compute the volumes that would correspond to it being divided
    for (unsigned int i = 0; i < nsubdivisions; ++i)
    {
        if (PartitionsSign[i] > 0)
        {
            rUpper_vol += Volumes[i];
        }
        else
        {
            rLower_vol += Volumes[i];
        }
    }
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::ComputeLHSGaussPointContribution(
    const double weight,
    Matrix& lhs,
    const ElementalData<TNumNodes, TDim>& data) const
{
    noalias(lhs) += weight * prod(data.DN_DX, trans(data.DN_DX));
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::AssignLeftHandSideSubdividedElement(
    Matrix& rLeftHandSideMatrix,
    Matrix& lhs_positive,
    Matrix& lhs_negative,
    const BoundedMatrix<double, TNumNodes, TNumNodes>& lhs_total,
    const ElementalData<TNumNodes, TDim>& data) const
{
    const auto& r_geometry = this->GetGeometry();
    for (unsigned int i = 0; i < TNumNodes; ++i)
    {
        // The TE node takes the contribution of the subdivided element and
        // we do not apply the wake condition on the TE node
        if (r_geometry[i].GetValue(TRAILING_EDGE))
        {
            for (unsigned int j = 0; j < TNumNodes; ++j)
            {
                rLeftHandSideMatrix(i, j) = lhs_positive(i, j);
                rLeftHandSideMatrix(i + TNumNodes, j + TNumNodes) = lhs_negative(i, j);
            }
        }
        else
        {
            AssignLeftHandSideWakeNode(rLeftHandSideMatrix, lhs_total, data, i);
        }
    }
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::AssignLeftHandSideWakeElement(
    MatrixType& rLeftHandSideMatrix,
    const BoundedMatrix<double, TNumNodes, TNumNodes>& lhs_total,
    const ElementalData<TNumNodes, TDim>& data) const
{
    for (unsigned int row = 0; row < TNumNodes; ++row)
    {
        AssignLeftHandSideWakeNode(rLeftHandSideMatrix, lhs_total, data, row);
    }
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::AssignLeftHandSideWakeNode(
    MatrixType& rLeftHandSideMatrix,
    const BoundedMatrix<double, TNumNodes, TNumNodes>& lhs_total,
    const ElementalData<TNumNodes, TDim>& data,
    unsigned int& row) const
{
    // Filling the diagonal blocks (i.e. decoupling upper and lower dofs)
    for (unsigned int column = 0; column < TNumNodes; ++column)
    {
        rLeftHandSideMatrix(row, column) = lhs_total(row, column);
        rLeftHandSideMatrix(row + TNumNodes, column + TNumNodes) = lhs_total(row, column);
    }

    // Applying wake condition on the AUXILIARY_VELOCITY_POTENTIAL dofs
    if (data.distances[row] < 0.0)
    {
        for (unsigned int column = 0; column < TNumNodes; ++column)
        {
            rLeftHandSideMatrix(row, column + TNumNodes) = -lhs_total(row, column); // Side 1
        }
    }
    else if (data.distances[row] > 0.0)
    {
        for (unsigned int column = 0; column < TNumNodes; ++column)
        {
            rLeftHandSideMatrix(row + TNumNodes, column) = -lhs_total(row, column); // Side 2
        }
    }
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::AssignRightHandSideWakeNode(
    VectorType& rRightHandSideVector,
    const BoundedVector<double, TNumNodes>& rUpper_rhs,
    const BoundedVector<double, TNumNodes>& rLower_rhs,
    const BoundedVector<double, TNumNodes>& rWake_rhs,
    const ElementalData<TNumNodes, TDim>& rData,
    unsigned int& rRow) const
{
    if (rData.distances[rRow] > 0.0)
    {
        rRightHandSideVector[rRow] = rUpper_rhs(rRow);
        rRightHandSideVector[rRow + TNumNodes] = rWake_rhs(rRow);
    }
    else
    {
        rRightHandSideVector[rRow] = rWake_rhs(rRow);
        rRightHandSideVector[rRow + TNumNodes] = rLower_rhs(rRow);
    }
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::ComputePotentialJump(const ProcessInfo& rCurrentProcessInfo)
{
    const array_1d<double, 3>& vinfinity = rCurrentProcessInfo[FREE_STREAM_VELOCITY];
    const double v_infinity_norm = sqrt(inner_prod(vinfinity, vinfinity));

    array_1d<double, TNumNodes> distances;
    GetWakeDistances(distances);

    for (unsigned int i = 0; i < TNumNodes; i++)
    {
        double aux_potential =
            GetGeometry()[i].FastGetSolutionStepValue(AUXILIARY_VELOCITY_POTENTIAL);
        double potential = GetGeometry()[i].FastGetSolutionStepValue(VELOCITY_POTENTIAL);
        double potential_jump = aux_potential - potential;
        if (distances[i] > 0)
        {
            GetGeometry()[i].SetValue(POTENTIAL_JUMP,
                                      -2.0 / v_infinity_norm * (potential_jump));
        }
        else
        {
            GetGeometry()[i].SetValue(POTENTIAL_JUMP, 2.0 / v_infinity_norm * (potential_jump));
        }
    }
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::FindUpwindElement(const ProcessInfo& rCurrentProcessInfo)
{
    const TransonicPerturbationPotentialFlowElement& r_this = *this;

    std::vector<size_t> upwind_element_nodes, neighbor_element_ids;

    // find nodes of element which should be the upwind element
    GeometryType upwind_element_boundary;
    FindUpwindNodes(upwind_element_boundary, rCurrentProcessInfo);
    PotentialFlowUtilities::GetSortedIds<TDim, TNumNodes>(upwind_element_nodes, upwind_element_boundary);

    // find neighboring elements to nodes of current element
    GlobalPointersVector<Element> upwind_element_candidates;
    PotentialFlowUtilities::GetNodeNeighborElementCandidates<TDim, TNumNodes>(upwind_element_candidates, upwind_element_boundary);

    for (SizeType i = 0; i < upwind_element_candidates.size(); i++)
    {
        // get sorted node ids of neighbording elements
        PotentialFlowUtilities::GetSortedIds<TDim, TNumNodes>(neighbor_element_ids, upwind_element_candidates[i].GetGeometry());

        // find element which shares the upwind element nodes with current element
        // but is not the current element
        if(std::includes(neighbor_element_ids.begin(), neighbor_element_ids.end(), 
            upwind_element_nodes.begin(), upwind_element_nodes.end()) 
            && upwind_element_candidates[i].Id() != r_this.Id())
        {
            mpUpwindElement = upwind_element_candidates(i);
            break;
        }
    }

    // If no upwind element is found, the element is an INLET element and the
    // upwind element pointer points to itself
    if (mpUpwindElement.get() == nullptr)
    {
        mpUpwindElement = this;
        this->SetFlags(INLET);
    }

}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::FindUpwindNodes(GeometryType& rResult,
    const ProcessInfo& rCurrentProcessInfo)
{   
    const TransonicPerturbationPotentialFlowElement& r_this = *this;

    // free stream values
    const array_1d<double, 3> free_stream_velocity = rCurrentProcessInfo[FREE_STREAM_VELOCITY];

    // current element geometry
    const GeometryType& r_geom = r_this.GetGeometry();

    // get element edges or faces depending on dimension of the problem
    Element::GeometryType::GeometriesArrayType element_boundary_geometry;

    if(TDim == 2)
    {
        // current element edges
        element_boundary_geometry = r_geom.GenerateEdges();
    }
    else if(TDim == 3)
    {
        // current element faces
        element_boundary_geometry = r_geom.GenerateFaces();
    }

    array_1d<double,3> aux_coordinates;
    vector<double> element_boundary_normal_velocity_components(element_boundary_geometry.size(), 0.0);

    for (SizeType i = 0; i < element_boundary_geometry.size(); i++)
    { 
        // get local coordinates of bondary geometry center
        element_boundary_geometry[i].PointLocalCoordinates(aux_coordinates, element_boundary_geometry[i].Center());
        
        // compute portion of normal vectors of faces or edges in free stream direction
        element_boundary_normal_velocity_components[i] = 
            PotentialFlowUtilities::ComputeScalarProductProjection<TDim, TNumNodes>(
                element_boundary_geometry[i].Normal(aux_coordinates), free_stream_velocity);
    }

    // find min normal component, which belongs to the edge or face which acts as the boundary to the upwind element
    const auto min_normal_v_comp = std::min_element(
        element_boundary_normal_velocity_components.begin(), element_boundary_normal_velocity_components.end());    
    
    // get vector location of the edg of face which is shared between the current and upwind element
    const auto upwind_geometry_iterator = std::distance(
        std::begin(element_boundary_normal_velocity_components), min_normal_v_comp);

    rResult = element_boundary_geometry[upwind_geometry_iterator];
}

template<int TDim, int TNumNodes>
int TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::GetAdditionalNode() const
{
    const TransonicPerturbationPotentialFlowElement& r_this = *this;

    // current and upwind element geometry
    const GeometryType& r_geom = r_this.GetGeometry();
    const GeometryType& r_upwind_geom = mpUpwindElement->GetGeometry();

    int non_matching_nodes_counter = 0;
    int additional_node = 0;
    bool loop_stop = false;

    for (unsigned int i = 0; i < TNumNodes && !loop_stop; i++)
    {
        for (unsigned int j = 0; j < TNumNodes && !loop_stop; j++)
        {
            // check if nodes are the same
            if(r_upwind_geom[i].Id() != r_geom[j].Id())
            {
                non_matching_nodes_counter++;
            }
        }
        // get index of non matching node
        if (non_matching_nodes_counter == 3)
        {
            additional_node = i;
            loop_stop = true;
        }
        non_matching_nodes_counter = 0;
    }
    // KRATOS_WATCH(r_geom[0].Id());
    // KRATOS_WATCH(r_geom[1].Id());
    // KRATOS_WATCH(r_geom[2].Id());
    // KRATOS_WATCH(r_upwind_geom[0].Id());
    // KRATOS_WATCH(r_upwind_geom[1].Id());
    // KRATOS_WATCH(r_upwind_geom[2].Id());
    return additional_node;
}

template <int TDim, int TNumNodes>
double TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::ComputeDensity(const ProcessInfo& rCurrentProcessInfo) const
{
    // Reading free stream conditions
    const double rho_inf = rCurrentProcessInfo[FREE_STREAM_DENSITY];
    const double M_inf = rCurrentProcessInfo[FREE_STREAM_MACH];
    const double heat_capacity_ratio = rCurrentProcessInfo[HEAT_CAPACITY_RATIO];
    const double mach_number_limit = rCurrentProcessInfo[MACH_LIMIT];

    // Computing local mach number
    double local_mach_number = PotentialFlowUtilities::ComputePerturbationLocalMachNumber<TDim, TNumNodes>(*this, rCurrentProcessInfo);

    if (local_mach_number > mach_number_limit)
    { // Clamping the mach number to mach_number_limit
        KRATOS_WARNING("ComputeDensity") << "Clamping the local mach number to " << mach_number_limit << std::endl;
        local_mach_number = mach_number_limit;
    }

    // Computing squares
    const double M_inf_2 = M_inf * M_inf;
    const double M_2 = local_mach_number * local_mach_number;

    // Computing density according to Equation 8.9 of Drela, M. (2014) Flight Vehicle
    // Aerodynamics, The MIT Press, London
    const double numerator = 1 + (heat_capacity_ratio - 1) * M_inf_2 / 2;
    const double denominator = 1 + (heat_capacity_ratio - 1) * M_2 / 2;
    const double base = numerator / denominator;

    if (base > 0.0)
    {
        return rho_inf * pow(base, 1 / (heat_capacity_ratio - 1));
    }
    else
    {
        KRATOS_WARNING("ComputeDensity") << "Using density correction" << std::endl;
        return rho_inf * 0.00001;
    }
}

template <int TDim, int TNumNodes>
double TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::ComputeDensityDerivative(
    const double rho,
    const ProcessInfo& rCurrentProcessInfo) const
{
    // Reading free stream conditions
    const double rho_inf = rCurrentProcessInfo[FREE_STREAM_DENSITY];
    const double heat_capacity_ratio = rCurrentProcessInfo[HEAT_CAPACITY_RATIO];
    const double a_inf = rCurrentProcessInfo[SOUND_VELOCITY];

    return -pow(rho_inf, heat_capacity_ratio - 1) *
           pow(rho, 2 - heat_capacity_ratio) / (2 * a_inf * a_inf);
}

// serializer

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::save(Serializer& rSerializer) const
{
    KRATOS_SERIALIZE_SAVE_BASE_CLASS(rSerializer, Element);
}

template <int TDim, int TNumNodes>
void TransonicPerturbationPotentialFlowElement<TDim, TNumNodes>::load(Serializer& rSerializer)
{
    KRATOS_SERIALIZE_LOAD_BASE_CLASS(rSerializer, Element);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Template class instantiation

template class TransonicPerturbationPotentialFlowElement<2, 3>;
template class TransonicPerturbationPotentialFlowElement<3, 4>;

} // namespace Kratos
