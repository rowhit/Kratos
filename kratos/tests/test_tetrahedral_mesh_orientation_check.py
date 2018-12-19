from __future__ import print_function, absolute_import, division
import KratosMultiphysics

import KratosMultiphysics.KratosUnittest as KratosUnittest
import os

def GetFilePath(fileName):
    return os.path.join(os.path.dirname(os.path.realpath(__file__)), fileName)

class TestTetrahedralMeshOrientationCheck(KratosUnittest.TestCase):

    def test_TetrahedralMeshOrientationCheck(self):
        KratosMultiphysics.Logger.GetDefaultOutput().SetSeverity(KratosMultiphysics.Logger.Severity.WARNING)
        current_model = KratosMultiphysics.Model()

        model_part = current_model.CreateModelPart("Main")
        model_part_io = KratosMultiphysics.ModelPartIO(GetFilePath("coarse_sphere"))
        model_part_io.ReadModelPart(model_part)

        check_mesh = KratosMultiphysics.TetrahedralMeshOrientationCheck(model_part, True)
        check_mesh.Execute()

    @KratosUnittest.expectedFailure
    def test_ErrorTetrahedralMeshOrientationCheck(self):
        KratosMultiphysics.Logger.GetDefaultOutput().SetSeverity(KratosMultiphysics.Logger.Severity.WARNING)
        current_model = KratosMultiphysics.Model()

        model_part = current_model.CreateModelPart("Main")
        model_part_io = KratosMultiphysics.ModelPartIO(GetFilePath("coarse_sphere"))
        model_part_io.ReadModelPart(model_part)

        check_mesh = KratosMultiphysics.TetrahedralMeshOrientationCheck(model_part, True)
        check_mesh.SwapAll()
        check_mesh.Execute()

if __name__ == '__main__':
    KratosUnittest.main()

