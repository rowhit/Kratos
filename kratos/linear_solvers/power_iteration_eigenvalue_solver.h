//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Pooyan Dadvand
//  Collaborator:    Vicente Mataix Ferrandiz
//
//

#if !defined(KRATOS_POWER_ITERATION_EIGENVALUE_SOLVER_H_INCLUDED )
#define  KRATOS_POWER_ITERATION_EIGENVALUE_SOLVER_H_INCLUDED

// System includes

// External includes

// Project includes
#include "spaces/ublas_space.h"
#include "includes/define.h"
#include "linear_solvers/iterative_solver.h"
#include "utilities/random_initializer_utility.h"

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

///@}
///@name Kratos Classes
///@{
 
/** 
 * @class PowerIterationEigenvalueSolver
 * @ingroup KratosCore
 * @brief This class uses the inverted power iteration method to obtain the lowest eigenvalue of a system
 * @details The solver has different constructors, so can be constructed with the different parameters of using kartos parameters
 * @see IterativeSolver
 * @author Pooyan Dadvand
 * @author Vicente Mataix Ferrandiz
*/
template<class TSparseSpaceType, class TDenseSpaceType, class TLinearSolverType,
         class TPreconditionerType = Preconditioner<TSparseSpaceType, TDenseSpaceType>,
         class TReordererType = Reorderer<TSparseSpaceType, TDenseSpaceType> >
class PowerIterationEigenvalueSolver 
    : public IterativeSolver<TSparseSpaceType, TDenseSpaceType, TPreconditionerType, TReordererType>
{
public:
    ///@name Type Definitions
    ///@{

    /// Pointer definition of PowerIterationEigenvalueSolver
    KRATOS_CLASS_POINTER_DEFINITION(PowerIterationEigenvalueSolver);

    typedef IterativeSolver<TSparseSpaceType, TDenseSpaceType, TPreconditionerType, TReordererType> BaseType;

    typedef typename TSparseSpaceType::MatrixType SparseMatrixType;

    typedef typename TSparseSpaceType::VectorType VectorType;

    typedef typename TDenseSpaceType::MatrixType DenseMatrixType;

    typedef typename TDenseSpaceType::VectorType DenseVectorType;

    typedef std::size_t SizeType;

    typedef std::size_t IndexType;

    ///@}
    ///@name Life Cycle
    ///@{

    /// Default constructor.
    PowerIterationEigenvalueSolver() {}

    /**
     * @brief Alternative constructor
     * @details It uses additional variables to be initialized
     * @param MaxTolerance The maximal tolerance used as threshold for convergence
     * @param MaxIterationNumber The maximal number of iterations to be considered
     * @param RequiredEigenvalueNumber The required eigen value number 
     * @param pLinearSolver The linear solver used to solve the system of equations
     */
    PowerIterationEigenvalueSolver(
        double MaxTolerance,
        unsigned int MaxIterationNumber,
        unsigned int RequiredEigenvalueNumber,
        typename TLinearSolverType::Pointer pLinearSolver
    ): BaseType(MaxTolerance, MaxIterationNumber),   
       mRequiredEigenvalueNumber(RequiredEigenvalueNumber),
       mpLinearSolver(pLinearSolver)
    {

    }

    /**
     * @brief Alternative constructor
     * @details It uses a Kratos parameters to set the different variables and parameters
     * @param ThisParameters The parameters taht contain the different parameters for configuration
     * @param pLinearSolver The linear solver used to solve the system of equations
     */
    PowerIterationEigenvalueSolver(
        Parameters ThisParameters,
        typename TLinearSolverType::Pointer pLinearSolver
        ): mpLinearSolver(pLinearSolver)
    {
        Parameters DefaultParameters = Parameters(R"(
        {
            "solver_type"             : "power_iteration_eigenvalue_solver",
            "max_iteration"           : 10000,
            "tolerance"               : 1e-8,
            "required_eigen_number"   : 1,
            "shifting_convergence"    : 0.25,
            "verbosity"               : 1,
            "linear_solver_settings"  : {}
        })" );

        ThisParameters.ValidateAndAssignDefaults(DefaultParameters);

        mRequiredEigenvalueNumber = ThisParameters["required_eigen_number"].GetInt();
        mEchoLevel = ThisParameters["verbosity"].GetInt();
        BaseType::SetTolerance( ThisParameters["tolerance"].GetDouble() );
        BaseType::SetMaxIterationsNumber( ThisParameters["max_iteration"].GetInt() );
    }

    /// Copy constructor.
    PowerIterationEigenvalueSolver(const PowerIterationEigenvalueSolver& Other) : BaseType(Other)
    {

    }


    /// Destructor.
    ~PowerIterationEigenvalueSolver() override
    {

    }


    ///@}
    ///@name Operators
    ///@{

    /// Assignment operator.
    PowerIterationEigenvalueSolver& operator=(const PowerIterationEigenvalueSolver& Other)
    {
        BaseType::operator=(Other);
        return *this;
    }

    ///@}
    ///@name Operations
    ///@{

    /**
     * @brief The power iteration algorithm
     * @param K The stiffness matrix
     * @param M The mass matrix
     * @param Eigenvalues The vector containing the eigen values
     * @param Eigenvectors The matrix containing the eigen vectors
     */
    void Solve(
        SparseMatrixType& K,
        SparseMatrixType& M,
        DenseVectorType& Eigenvalues,
        DenseMatrixType& Eigenvectors
        ) override
    {

        const SizeType size = K.size1();
        const SizeType max_iteration = BaseType::GetMaxIterationsNumber();
        const double tolerance = BaseType::GetTolerance();

        VectorType x = boost::numeric::ublas::zero_vector<double>(size);
        VectorType y = boost::numeric::ublas::zero_vector<double>(size);

        RandomInitializeUtility<double>::RandomInitialize(K, y);

        if(Eigenvalues.size() < 1)
            Eigenvalues.resize(1, 0.0);

        // Starting with first step
        double beta = 0.0;
        double rho = 0.0;
        double old_rho = Eigenvalues[0];
        VectorType y_old = boost::numeric::ublas::zero_vector<double>(size);

        for(SizeType i = 0 ; i < max_iteration ; i++) {
            // K*x = y
            mpLinearSolver->Solve(K, x, y);
            
            rho = inner_prod(y, x);
            
            // y = M*x
            TSparseSpaceType::Mult(M, x, y);
            beta = inner_prod(x, y);
            
            KRATOS_ERROR_IF(beta <= 0.0) << "M is not Positive-definite. beta = " << beta << std::endl;

            rho /= beta;
            beta = std::sqrt(beta);
            TSparseSpaceType::InplaceMult(y, 1.0/beta);

            KRATOS_ERROR_IF(rho == 0.0) << "Perpendicular eigenvector to M" << std::endl;

            const double convergence_rho = std::abs((rho - old_rho) / rho);
            const double convergence_norm = TSparseSpaceType::TwoNorm(y - y_old)/TSparseSpaceType::TwoNorm(y);

            if (mEchoLevel > 1)
                KRATOS_INFO("Power Iterator Eigenvalue Solver: ") << "Iteration: " << i << " \t beta: " << beta << "\trho: " << rho << " \tConvergence norm: " << convergence_norm << " \tConvergence rho: " << convergence_rho << std::endl;
            
            if(convergence_norm < tolerance || convergence_rho < tolerance)
                break;

            old_rho = rho;
            TSparseSpaceType::Assign(y_old, 1.0, y);
        }

        if (mEchoLevel > 0) {
            KRATOS_INFO("rho: ") << rho << std::endl;
            KRATOS_INFO("y: ") << y << std::endl;
        }

        Eigenvalues[0] = rho;

        if((Eigenvectors.size1() < 1) || (Eigenvectors.size2() < size))
            Eigenvectors.resize(1,size);

        for(SizeType i = 0 ; i < size ; i++)
            Eigenvectors(0,i) = y[i];
    }
    
    /**
     * @brief This method returns directly the first eigen value obtained
     * @param K The stiffness matrix
     * @param M The mass matrix
     * @return The first eigenvalue
     */
    double GetEigenValue(
        SparseMatrixType& K,
        SparseMatrixType& M
        )
    {
        DenseVectorType eigen_values;
        DenseMatrixType eigen_vectors;
        
        Solve(K, M, eigen_values, eigen_vectors);
        
        return eigen_values[0];
    }
    
    ///@}
    ///@name Access
    ///@{


    ///@}
    ///@name Inquiry
    ///@{


    ///@}
    ///@name Input and output
    ///@{

    /// Turn back information as a string.
    std::string Info() const override
    {
        std::stringstream buffer;
        buffer << "Power iteration eigenvalue solver with " << BaseType::GetPreconditioner()->Info();
        return  buffer.str();
    }

    /// Print information about this object.
    void PrintInfo(std::ostream& rOStream) const override
    {
        rOStream << Info();
    }

    /// Print object's data.
    void PrintData(std::ostream& rOStream) const override
    {
        BaseType::PrintData(rOStream);
    }


    ///@}
    ///@name Friends
    ///@{


    ///@}

protected:
    ///@name Protected static Member Variables
    ///@{


    ///@}
    ///@name Protected member Variables
    ///@{


    ///@}
    ///@name Protected Operators
    ///@{


    ///@}
    ///@name Protected Operations
    ///@{


    ///@}
    ///@name Protected  Access
    ///@{


    ///@}
    ///@name Protected Inquiry
    ///@{


    ///@}
    ///@name Protected LifeCycle
    ///@{


    ///@}

private:
    ///@name Static Member Variables
    ///@{


    ///@}
    ///@name Member Variables
    ///@{

    unsigned int mRequiredEigenvalueNumber;             /// The requiered eigenvalue number @todo Currently not used, check if remove

    unsigned int mEchoLevel;                            /// The verbosity level considered

    typename TLinearSolverType::Pointer mpLinearSolver; /// The pointer to the linear solver considered

    ///@}
    ///@name Private Operators
    ///@{


    ///@}
    ///@name Private Operations
    ///@{


    ///@}
    ///@name Private  Access
    ///@{


    ///@}
    ///@name Private Inquiry
    ///@{


    ///@}
    ///@name Un accessible methods
    ///@{


    ///@}

}; // Class PowerIterationEigenvalueSolver

///@}

///@name Type Definitions
///@{


///@}
///@name Input and output
///@{


/// input stream function
template<class TSparseSpaceType, class TDenseSpaceType,
         class TPreconditionerType,
         class TReordererType>
inline std::istream& operator >> (std::istream& IStream,
                                  PowerIterationEigenvalueSolver<TSparseSpaceType, TDenseSpaceType,
                                  TPreconditionerType, TReordererType>& rThis)
{
    return IStream;
}

/// output stream function
template<class TSparseSpaceType, class TDenseSpaceType,
         class TPreconditionerType,
         class TReordererType>
inline std::ostream& operator << (std::ostream& OStream,
                                  const PowerIterationEigenvalueSolver<TSparseSpaceType, TDenseSpaceType,
                                  TPreconditionerType, TReordererType>& rThis)
{
    rThis.PrintInfo(OStream);
    OStream << std::endl;
    rThis.PrintData(OStream);

    return OStream;
}
///@}


}  // namespace Kratos.

#endif // KRATOS_POWER_ITERATION_EIGENVALUE_SOLVER_H_INCLUDED defined
