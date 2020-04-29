import KratosMultiphysics

# Import Kratos "wrapper" for unittests
import KratosMultiphysics.KratosUnittest as KratosUnittest

try:
    import KratosMultiphysics.mpi as KratosMPI
    import KratosMultiphysics.MetisApplication as MetisApplication
    import KratosMultiphysics.TrilinosApplication as TrilinosApplication
except ImportError:
    raise Exception("KratosMPI could not be imported!")

if KratosMultiphysics.ParallelEnvironment.GetDefaultSize() != 2:
    raise Exception("The MPI tests currently suport only being run with 2 processors!")

# Import the tests or test_classes to create the suits

# Shell tests
from evm_k_epsilon_tests import EvmKEpsilonTest
from incompressible_potential_flow_solver_formulation_tests import IncompressiblePotentialFlowSolverFormulationTest
from fractional_step_velocity_pressure_formulation_tests import FractionalStepVelocityPressureFormulationTest
from monolithic_velocity_pressure_formulation_tests import MonolithicVelocityPressureFormulationTest
from fractional_step_k_epsilon_high_re_formulation_tests import FractionalStepKEpsilonHighReTest
from monolithic_k_epsilon_high_re_formulation_tests import MonolithicKEpsilonHighReTest


def AssembleTestSuites():
    ''' Populates the test suites to run.

    Populates the test suites to run. At least, it should pupulate the suites:
    "small", "nighlty" and "all"

    Return
    ------

    suites: A dictionary of suites
        The set of suites with its test_cases added.
    '''
    suites = KratosUnittest.KratosSuites

    ### Small MPI tests ########################################################
    # smallMPISuite = suites['mpi_small']

    ### Nightly MPI tests ######################################################
    nightlyMPISuite = suites['mpi_nightly']
    nightlyMPISuite.addTest((IncompressiblePotentialFlowSolverFormulationTest('testIncompressiblePotentialFlowMPI')))
    nightlyMPISuite.addTest((FractionalStepVelocityPressureFormulationTest('testFractionalStepVelocityPressureMPI')))
    nightlyMPISuite.addTest((MonolithicVelocityPressureFormulationTest('testMonolithicVelocityPressureMPI')))
    ## fractional step tests
    nightlyMPISuite.addTest((FractionalStepKEpsilonHighReTest('testFractionalStepKEpsilonHighReAfcTkeRhsMPI')))
    nightlyMPISuite.addTest((FractionalStepKEpsilonHighReTest('testFractionalStepKEpsilonHighReAfcTkeRhsMPI')))
    nightlyMPISuite.addTest((FractionalStepKEpsilonHighReTest('testFractionalStepKEpsilonHighReAfcVelocityRhsMPI')))
    nightlyMPISuite.addTest((FractionalStepKEpsilonHighReTest('testFractionalStepKEpsilonHighReRfcTkeRhsMPI')))
    nightlyMPISuite.addTest((FractionalStepKEpsilonHighReTest('testFractionalStepKEpsilonHighReRfcTkeRhsMPI')))
    nightlyMPISuite.addTest((FractionalStepKEpsilonHighReTest('testFractionalStepKEpsilonHighReRfcVelocityRhsMPI')))

    ### monolithic tests
    nightlyMPISuite.addTest((MonolithicKEpsilonHighReTest('testMonolithicKEpsilonHighReAfcTkeRhsMPI')))
    nightlyMPISuite.addTest((MonolithicKEpsilonHighReTest('testMonolithicKEpsilonHighReAfcTkeRhsMPI')))
    nightlyMPISuite.addTest((MonolithicKEpsilonHighReTest('testMonolithicKEpsilonHighReAfcVelocityRhsMPI')))
    nightlyMPISuite.addTest((MonolithicKEpsilonHighReTest('testMonolithicKEpsilonHighReRfcTkeRhsMPI')))
    nightlyMPISuite.addTest((MonolithicKEpsilonHighReTest('testMonolithicKEpsilonHighReRfcTkeRhsMPI')))
    nightlyMPISuite.addTest((MonolithicKEpsilonHighReTest('testMonolithicKEpsilonHighReRfcVelocityRhsMPI')))
    nightlyMPISuite.addTest(EvmKEpsilonTest('testChannelFlowKEpsilonTransientMPI'))
    # nightlyMPISuite.addTest(EvmKEpsilonTest('testChannelFlowKEpsilonSteadyMPI'))

    ### Full MPI set ###########################################################
    allMPISuite = suites['mpi_all']
    allMPISuite.addTests(nightlyMPISuite) # already contains the smallMPISuite

    allSuite = suites['all']
    allSuite.addTests(allMPISuite)

    return suites


if __name__ == '__main__':
    KratosUnittest.runTests(AssembleTestSuites())
