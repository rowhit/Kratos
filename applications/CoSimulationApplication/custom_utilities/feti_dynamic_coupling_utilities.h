//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Philipp  DetectosIntersectosOverlapposAndMappos
//

#if !defined(KRATOS_FETI_DYNAMIC_COUPLING_UTILITIES_H_INCLUDED)
#define  KRATOS_FETI_DYNAMIC_COUPLING_UTILITIES_H_INCLUDED

// System includes

// External includes

// Project includes
#include "includes/define.h"
#include "includes/model_part.h"
#include "spaces/ublas_space.h"
#include "includes/ublas_complex_interface.h"


namespace Kratos
{
    class FetiDynamicCouplingUtilities
    {
    public:
        typedef std::size_t SizeType;
        typedef std::size_t IndexType;

        typedef Node<3> NodeType;
        typedef typename NodeType::Pointer NodePointerType;
        typedef Geometry<NodeType> GeometryType;
        typedef typename GeometryType::Pointer GeometryPointerType;

        typedef UblasSpace<double, CompressedMatrix, boost::numeric::ublas::vector<double>> SparseSpaceType;
        typedef typename SparseSpaceType::MatrixType SystemMatrixType;

        typedef Matrix DenseMappingMatrixType;
        typedef Kratos::unique_ptr<DenseMappingMatrixType> DenseMappingMatrixUniquePointerType;

        typedef UblasSpace<double, CompressedMatrix, boost::numeric::ublas::vector<double>> SparseSpaceType;
        typedef typename SparseSpaceType::MatrixType MappingMatrixType;



        FetiDynamicCouplingUtilities(ModelPart& rInterfaceOrigin, ModelPart& rInterFaceDestination,
            double OriginNewmarkBeta, double OriginNewmarkGamma,
            double DestinationNewmarkBeta, double DestinationNewmarkGamma)
            :mrOriginModelPart(rInterfaceOrigin), mrDestinationModelPart(rInterFaceDestination),
            mOriginBeta(OriginNewmarkBeta), mOriginGamma(OriginNewmarkGamma),
            mDestinationBeta(DestinationNewmarkBeta), mDestinationGamma(DestinationNewmarkGamma)
        {
            KRATOS_WATCH("11111");

            // Check newmark parameters are valid
            KRATOS_ERROR_IF(mOriginBeta < 0.0 || mOriginBeta > 1.0)
                << "FetiDynamicCouplingUtilities | Error, mOriginBeta has invalid value. It must be between 0 and 1.";
            KRATOS_ERROR_IF(mOriginGamma < 0.0 || mOriginGamma > 1.0)
                << "FetiDynamicCouplingUtilities | Error, mOriginGamma has invalid value. It must be between 0 and 1.";
            KRATOS_ERROR_IF(mDestinationBeta < 0.0 || mDestinationBeta > 1.0)
                << "FetiDynamicCouplingUtilities | Error, mDestinationBeta has invalid value. It must be between 0 and 1.";
            KRATOS_ERROR_IF(mDestinationGamma < 0.0 || mDestinationGamma > 1.0)
                << "FetiDynamicCouplingUtilities | Error, mDestinationGamma has invalid value. It must be between 0 and 1.";

            // Todo
            // more todo
            // add more liens
        };

        void SetEffectiveStiffnessMatrices(SystemMatrixType& rK, IndexType SolverIndex)
        {
            if (SolverIndex == 0) mpKOrigin = &rK;
            else if (SolverIndex == 1) mpKDestination = &rK;
            else KRATOS_ERROR << "SetEffectiveStiffnessMatrices, Index must be 0 or 1";
        };

        void SetMappingMatrix(MappingMatrixType* pMappingMatrix)
        {
            mpMappingMatrix = pMappingMatrix;
        };

        void EquilibrateDomains();


        //void KRATOS_API(CO_SIMULATION_APPLICATION) FindIntersection1DGeometries2D(


    private:
        ModelPart& mrOriginModelPart;
        ModelPart& mrDestinationModelPart;

        SystemMatrixType* mpKOrigin = nullptr;
        SystemMatrixType* mpKDestination = nullptr;

        //DenseMappingMatrixUniquePointerType mpMappingMatrix = nullptr;
        MappingMatrixType* mpMappingMatrix = nullptr;

        const double mOriginBeta;
        const double mOriginGamma;
        const double mDestinationBeta;
        const double mDestinationGamma;


        void CalculateUnbalancedInterfaceFreeVelocities(Vector& rUnbalancedVelocities);





    };  // namespace FetiDynamicCouplingUtilities.

}  // namespace Kratos.

#endif // KRATOS_FETI_DYNAMIC_COUPLING_UTILITIES_H_INCLUDED  defined