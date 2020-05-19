﻿from __future__ import print_function, absolute_import, division

import KratosMultiphysics
import KratosMultiphysics.KratosUnittest as KratosUnittest
import KratosMultiphysics.kratos_utilities as kratos_utils
import KratosMultiphysics.FluidDynamicsApplication as KratosFluid
from KratosMultiphysics.FluidDynamicsApplication.apply_mass_conservation_check_process import ApplyMassConservationCheckProcess
import re

if KratosMultiphysics.DataCommunicator.GetDefault().IsDistributed():
    import KratosMultiphysics.mpi.distributed_import_model_part_utility as distributed_import_model_part_utility

def GetFilePath(fileName):
    return KratosMultiphysics.os.path.dirname(KratosMultiphysics.os.path.realpath(__file__)) + "/" + fileName

class MassConservationUtility(KratosUnittest.TestCase):
    def setUp(self):
        self.comm = KratosMultiphysics.DataCommunicator.GetDefault()
        self.size = self.comm.Size()
        self.rank = self.comm.Rank()

    def tearDown(self):
        with KratosUnittest.WorkFolderScope(self.work_folder, __file__):
            self.comm.Barrier()
            if self.rank == 0:
                kratos_utils.DeleteFileIfExisting(self.file_name + ".time")
            kratos_utils.DeleteFileIfExisting(self.file_name + "_" + str(self.rank) + ".mdpa")
            self.comm.Barrier()

    @staticmethod
    def _SetParameters(name, use_memory):
        parameters = """{
            "echo_level" : 0,
            "model_import_settings" : {
                "input_type" : "mdpa",
                "input_filename" :\""""+ name +"""\",
                "partition_in_memory" : """ + use_memory + """
            }
        }"""

        return parameters

    def _CreateModelPart(self):
        self.parameters = self._SetParameters(self.file_name, use_memory="false")
        self.model = KratosMultiphysics.Model()
        model_part = self.model.CreateModelPart("ModelPart")
        model_part.AddNodalSolutionStepVariable(KratosMultiphysics.MESH_VELOCITY)
        model_part.AddNodalSolutionStepVariable(KratosMultiphysics.VELOCITY)
        model_part.AddNodalSolutionStepVariable(KratosMultiphysics.DISTANCE)
        if self.comm.IsDistributed():
            model_part.AddNodalSolutionStepVariable(KratosMultiphysics.PARTITION_INDEX)

        model_part.SetBufferSize(3)

        with KratosUnittest.WorkFolderScope(self.work_folder, __file__):
            import_settings = KratosMultiphysics.Parameters(self.parameters)
            self._ImportModelPart(model_part, import_settings)

        return model_part

    def _ImportModelPart(self, model_part, import_settings):
        if self.comm.IsDistributed():
            distributed_model_part_importer = distributed_import_model_part_utility.DistributedImportModelPartUtility(model_part, import_settings)
            distributed_model_part_importer.ImportModelPart()
            distributed_model_part_importer.CreateCommunicators()
        else:
            import_flags = KratosMultiphysics.ModelPartIO.READ
            KratosMultiphysics.ModelPartIO(import_settings['model_import_settings']['input_filename'].GetString(), import_flags).ReadModelPart(model_part)

    @staticmethod
    def _SetInletAndOutlet(model_part):
        inlet_conds = [5,6,17,18,29,30,41,42]
        outlet_conds = [11,12,23,24,35,36,47,48]
        for cond in model_part.Conditions:
            if cond.Id in inlet_conds:
                cond.Set(KratosMultiphysics.INLET)
            if cond.Id in outlet_conds:
                cond.Set(KratosMultiphysics.OUTLET)

    def test_Initialize(self):
        self.work_folder = "auxiliary_files"
        self.file_name = "cube_tetrahedra_elements_coarse"
        model_part = self._CreateModelPart()
        for node in model_part.GetCommunicator().LocalMesh().Nodes:
            node.SetSolutionStepValue(KratosMultiphysics.DISTANCE, node.Y - 0.5)
        settings = KratosMultiphysics.Parameters()
        mass_conservation_process = KratosFluid.MassConservationUtility(model_part, settings)
        message = mass_conservation_process.Initialize()
        filtered_results = re.findall(r"[-+]?\d*\.\d+|\d+", message)
        self.assertAlmostEqual(float(filtered_results[0]), 0.5)
        self.assertAlmostEqual(float(filtered_results[1]), 0.5)
        self.assertAlmostEqual(float(filtered_results[2]), 1.0)

    def test_ComputeBalancedVolume(self):
        self.work_folder = "auxiliary_files"
        self.file_name = "cube_tetrahedra_elements_coarse"
        model_part = self._CreateModelPart()
        for node in model_part.GetCommunicator().LocalMesh().Nodes:
            node.SetSolutionStepValue(KratosMultiphysics.DISTANCE, node.Y - 0.5)
            node.SetSolutionStepValue(KratosMultiphysics.VELOCITY_X, 1.0 - 0.5*node.X)
        self._SetInletAndOutlet(model_part)
        settings = KratosMultiphysics.Parameters()
        mass_conservation_process = KratosFluid.MassConservationUtility(model_part, settings)
        model_part.CloneTimeStep(0.1)
        model_part.ProcessInfo[KratosMultiphysics.TIME_CRT] = 0.1
        mass_conservation_process.Initialize()
        balance_message = mass_conservation_process.ComputeBalancedVolume()
        filtered_results = re.findall(r"[-+]?\d*\.\d+|\d+", balance_message)
        current_time = float(filtered_results[0])
        neg_vol = float(filtered_results[1])
        pos_vol = float(filtered_results[2])
        volume_error = float(filtered_results[3])
        net_inflow_inlet = float(filtered_results[4])
        net_inflow_outlet = float(filtered_results[5])
        inlet_area = float(filtered_results[6])
        self.assertAlmostEqual(current_time, 0.1)
        self.assertAlmostEqual(neg_vol, 0.5)
        self.assertAlmostEqual(pos_vol, 0.5)
        self.assertAlmostEqual(volume_error, 0.025)
        self.assertAlmostEqual(net_inflow_inlet, 0.5)
        self.assertAlmostEqual(net_inflow_outlet, -0.25)
        self.assertAlmostEqual(inlet_area, 1.0)

    def test_OrthogonalFlowIntoAir(self):
        self.work_folder = "auxiliary_files"
        self.file_name = "cube_tetrahedra_elements_coarse"
        model_part = self._CreateModelPart()
        for node in model_part.Nodes:
            node.SetSolutionStepValue(KratosMultiphysics.DISTANCE, node.Y - 0.5)
            node.SetSolutionStepValue(KratosMultiphysics.VELOCITY_X, 2.0) #This term should not affect
            node.SetSolutionStepValue(KratosMultiphysics.VELOCITY_Y, 3.0 - node.Z)
            node.SetSolutionStepValue(KratosMultiphysics.MESH_VELOCITY_Y, 1.0)

        settings = KratosMultiphysics.Parameters()
        mass_conservation_process = KratosFluid.MassConservationUtility(model_part, settings)
        orthogonal_flow = mass_conservation_process.OrthogonalFlowIntoAir(1.0)
        self.assertAlmostEqual(orthogonal_flow,1.5)

    def test_NoOrthogonalFlowIntoAir(self):
        self.work_folder = "auxiliary_files"
        self.file_name = "cube_tetrahedra_elements_coarse"
        model_part = self._CreateModelPart()
        for node in model_part.Nodes:
            node.SetSolutionStepValue(KratosMultiphysics.DISTANCE, node.Y - 0.5)
            node.SetSolutionStepValue(KratosMultiphysics.VELOCITY_X, 2.0) #This term should not affect
            node.SetSolutionStepValue(KratosMultiphysics.VELOCITY_Y, -3.0 + node.Z)
            node.SetSolutionStepValue(KratosMultiphysics.MESH_VELOCITY_Y, -1.0)

        settings = KratosMultiphysics.Parameters()
        mass_conservation_process = KratosFluid.MassConservationUtility(model_part, settings)
        orthogonal_flow = mass_conservation_process.OrthogonalFlowIntoAir(1.0)
        self.assertAlmostEqual(orthogonal_flow,0.0)

    def test_ReverseOrthogonalFlowIntoAir(self):
        self.work_folder = "auxiliary_files"
        self.file_name = "cube_tetrahedra_elements_coarse"
        model_part = self._CreateModelPart()
        for node in model_part.Nodes:
            node.SetSolutionStepValue(KratosMultiphysics.DISTANCE, node.Y - 0.5)
            node.SetSolutionStepValue(KratosMultiphysics.VELOCITY_X, 2.0) #This term should not affect
            node.SetSolutionStepValue(KratosMultiphysics.VELOCITY_Y, -3.0 + node.Z)
            node.SetSolutionStepValue(KratosMultiphysics.MESH_VELOCITY_Y, -1.0)

        settings = KratosMultiphysics.Parameters()
        mass_conservation_process = KratosFluid.MassConservationUtility(model_part, settings)
        orthogonal_flow = mass_conservation_process.OrthogonalFlowIntoAir(-1.0)
        self.assertAlmostEqual(orthogonal_flow,1.5)

    def test_ComputeDtForConvection(self):
        self.work_folder = "auxiliary_files"
        self.file_name = "cube_tetrahedra_elements_coarse"
        model_part = self._CreateModelPart()
        for node in model_part.GetCommunicator().LocalMesh().Nodes:
            node.SetSolutionStepValue(KratosMultiphysics.DISTANCE, node.X - 0.75)
            node.SetSolutionStepValue(KratosMultiphysics.VELOCITY_X, 1.0 - node.X)

        self._SetInletAndOutlet(model_part)
        settings = KratosMultiphysics.Parameters()
        mass_conservation_process = KratosFluid.MassConservationUtility(model_part, settings)
        model_part.CloneTimeStep(0.1)
        mass_conservation_process.Initialize()
        mass_conservation_process.ComputeBalancedVolume()
        dt = mass_conservation_process.ComputeDtForConvection()
        self.assertAlmostEqual(dt, 0.4)

    def test_ComputeDtForConvectionMaxDt(self):
        self.work_folder = "auxiliary_files"
        self.file_name = "cube_tetrahedra_elements_coarse"
        model_part = self._CreateModelPart()
        for node in model_part.GetCommunicator().LocalMesh().Nodes:
            node.SetSolutionStepValue(KratosMultiphysics.DISTANCE, node.X - 0.5)
            node.SetSolutionStepValue(KratosMultiphysics.VELOCITY_X, 1.0 - node.X)

        self._SetInletAndOutlet(model_part)
        settings = KratosMultiphysics.Parameters("""{
            "dt_upper_limit" : 0.19
        }""")
        mass_conservation_process = KratosFluid.MassConservationUtility(model_part, settings)
        model_part.CloneTimeStep(0.1)
        mass_conservation_process.Initialize()
        mass_conservation_process.ComputeBalancedVolume()
        dt = mass_conservation_process.ComputeDtForConvection()
        self.assertAlmostEqual(dt, 0.19)

    def test_ComputeDtForConvectionMinDt(self):
        self.work_folder = "auxiliary_files"
        self.file_name = "cube_tetrahedra_elements_coarse"
        model_part = self._CreateModelPart()
        for node in model_part.GetCommunicator().LocalMesh().Nodes:
            node.SetSolutionStepValue(KratosMultiphysics.DISTANCE, node.X - 0.5)
            node.SetSolutionStepValue(KratosMultiphysics.VELOCITY_X, 1.0 - node.X)

        self._SetInletAndOutlet(model_part)
        settings = KratosMultiphysics.Parameters("""{
            "dt_lower_limit" : 0.21
        }""")
        mass_conservation_process = KratosFluid.MassConservationUtility(model_part, settings)
        model_part.CloneTimeStep(0.1)
        mass_conservation_process.Initialize()
        mass_conservation_process.ComputeBalancedVolume()
        dt = mass_conservation_process.ComputeDtForConvection()
        self.assertAlmostEqual(dt, 0.21)

    def test_CorrectMass(self):
        self.work_folder = "auxiliary_files"
        self.file_name = "cube_tetrahedra_elements_coarse"
        model_part = self._CreateModelPart()
        for node in model_part.GetCommunicator().LocalMesh().Nodes:
            node.SetSolutionStepValue(KratosMultiphysics.DISTANCE, node.X - 0.5)
            node.SetSolutionStepValue(KratosMultiphysics.VELOCITY_X, 1.0 - node.Y)

        self._SetInletAndOutlet(model_part)
        settings = KratosMultiphysics.Parameters("""{
            "model_part_name" : "ModelPart"
        }""")
        mass_conservation_process = ApplyMassConservationCheckProcess(model_part.GetModel(), settings)
        mass_conservation_process.ExecuteInitialize()
        model_part.CloneTimeStep(0.1)
        mass_conservation_process.ExecuteFinalizeSolutionStep()
        results = {1: -0.5938515490094666,2: -0.09699619395711377,3: 0.40140301120375166,4: -0.5517655789133667,
            5: -0.051199842079361124, 6: 0.44927297736419847,7: -0.5033021486670106,8: -5e-07,9: 0.4991147797571505,
            10: -0.5917443573706135,11: -0.09536542517311021,12: 0.40203241373063053,13: -0.5508310005042678,
            14: -0.05344762008016252,15: 0.4487502655835266,16: -0.5042584855417436,17: -0.000271931088392015,
            18: 0.5,19: -0.5867198230113498,20: -0.09537664178326279,21: 0.40337839857437485,22: -0.546629205139377,
            23: -0.05774121962805403,24: 0.44941171225031157,25: -0.5256186036645334,26: -5e-07,27: 0.4998413776910875}
        for node in model_part.GetCommunicator().LocalMesh().Nodes:
            self.assertAlmostEqual(node.GetSolutionStepValue(KratosMultiphysics.DISTANCE), results.get(node.Id))
        self.assertAlmostEqual(model_part.ProcessInfo[KratosMultiphysics.DELTA_TIME], 0.1)


if __name__ == '__main__':
    KratosUnittest.main()