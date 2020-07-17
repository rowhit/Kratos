# Importing the Kratos Library
import KratosMultiphysics

# Import base class file
from KratosMultiphysics.FluidDynamicsApplication.navier_stokes_solver_vmsmonolithic import NavierStokesSolverMonolithic
import KratosMultiphysics.RomApplication as romapp

def CreateSolver(model, custom_settings):
    return ROMSolver(model, custom_settings)

class ROMSolver(NavierStokesSolverMonolithic):

    def __init__(self, model, custom_settings):
        super().__init__(model, custom_settings)
        KratosMultiphysics.Logger.PrintInfo("::[ROMSolver]:: ", "Construction finished")

    #### Private functions ####
    @classmethod
    def GetDefaultSettings(cls):
        default_settings = KratosMultiphysics.Parameters("""
        {
            "rom_settings": {
            "nodal_unknowns": [ "TEMPERATURE" ],
            "number_of_rom_dofs": 3
            }
        }
        """)
        default_settings.AddMissingParameters(super(ROMSolver,cls).GetDefaultSettings())
        return default_settings

    def _CreateBuilderAndSolver(self):
        linear_solver = self._GetLinearSolver()
        rom_parameters=self.settings["rom_settings"]
        builder_and_solver = romapp.ROMBuilderAndSolver(linear_solver, rom_parameters)
        return builder_and_solver
