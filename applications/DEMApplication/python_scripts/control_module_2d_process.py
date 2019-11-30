import KratosMultiphysics
import KratosMultiphysics.DEMApplication as Dem

def Factory(settings, Model):
    if(type(settings) != KratosMultiphysics.Parameters):
        raise Exception("expected input shall be a Parameters object, encapsulating a json string")
    return ControlModuleProcess(Model, settings["Parameters"])

## All the processes python should be derived from "Process"

class ControlModule2DProcess(KratosMultiphysics.Process):
    def __init__(self, Model, settings ):
        KratosMultiphysics.Process.__init__(self)

        self.params = KratosMultiphysics.Parameters( """
        {
            "dem_force_variable_name": "TOTAL_FORCES",
            "target_stress_variable_name": "TARGET_STRESS",
            "reaction_stress_variable_name": "REACTION_STRESS",
            "loading_velocity_variable_name": "LOADING_VELOCITY",
            "radial_displacement" : false,
            "imposed_z_strain": false
        }  """ )

        self.model_part = Model[settings["model_part_name"].GetString()]
        #self.model_part = model_part.GetSubModelPart("sub_model_part_name")
        target_stress_variable_name = settings["target_stress_variable_name"].GetString()
        reaction_stress_variable_name = settings["reaction_stress_variable_name"].GetString()
        loading_velocity_variable_name = settings["loading_velocity_variable_name"].GetString()

        self.components_process_list = []

        if settings["radial_displacement"].GetBool() == True:
            # Control module on the vertical walls of a right cylinder with the base on the 'X-Y' plane centered on (0,0).
            # Negative target_stress means compression.
            self.params = KratosMultiphysics.Parameters("{}")
            self.params["radial_displacement"].SetBool(settings["radial_displacement"].GetBool())
            self.params["imposed_z_strain"].SetBool(settings["imposed_z_strain"].GetBool())
            self.params.AddValue("model_part_name",settings["model_part_name"])
            self.params.AddValue("target_stress_table_id",settings["target_stress_table_id"])

            self.params.AddValue("initial_velocity",settings["initial_velocity"])
            self.params.AddValue("limit_velocity",settings["limit_velocity"])
            self.params.AddValue("velocity_factor",settings["velocity_factor"])
            self.params.AddValue("compression_length",settings["compression_length"])
            self.params.AddValue("face_area",settings["face_area"])
            self.params.AddValue("young_modulus",settings["young_modulus"])
            self.params.AddValue("stress_increment_tolerance",settings["stress_increment_tolerance"])
            self.params.AddValue("update_stiffness",settings["update_stiffness"])
            self.params.AddValue("start_time",settings["start_time"])
            self.components_process_list.append(Dem.ControlModule2DProcess(self.model_part, self.params))
        elif settings["imposed_z_strain"].GetBool() == True:
            self.params = KratosMultiphysics.Parameters("{}")
            self.params.AddValue("model_part_name",settings["model_part_name"])
            self.params.AddEmptyValue("variable_name").SetString(variable_name+"_Z")
            self.params.AddEmptyValue("reaction_variable_name").SetString(reaction_variable_name+"_Z")
            self.params.AddEmptyValue("target_stress_variable_name").SetString(target_stress_variable_name+"_Z")
            self.params.AddEmptyValue("reaction_stress_variable_name").SetString(reaction_stress_variable_name+"_Z")
            self.params.AddEmptyValue("loading_velocity_variable_name").SetString(loading_velocity_variable_name+"_Z")
            self.params.AddValue("target_stress_table_id",settings["target_stress_table_id"][2])
            self.params.AddValue("initial_velocity",settings["initial_velocity"][2])
            self.params.AddValue("limit_velocity",settings["limit_velocity"][2])
            self.params.AddValue("velocity_factor",settings["velocity_factor"])
            self.params.AddValue("compression_length",settings["compression_length"])
            self.params.AddValue("young_modulus",settings["young_modulus"])
            self.params.AddValue("stress_increment_tolerance",settings["stress_increment_tolerance"])
            self.params.AddValue("update_stiffness",settings["update_stiffness"])
            self.params.AddValue("start_time",settings["start_time"])
            self.components_process_list.append(Dem.ControlModule2DProcess(self.model_part, self.params))
        else:
            if settings["fixed"][0].GetBool() == True:
                self.x_params = KratosMultiphysics.Parameters("{}")
                self.x_params.AddValue("model_part_name",settings["model_part_name"])
                self.x_params.AddEmptyValue("variable_name").SetString(variable_name+"_X")
                self.x_params.AddEmptyValue("reaction_variable_name").SetString(reaction_variable_name+"_X")
                self.x_params.AddEmptyValue("target_stress_variable_name").SetString(target_stress_variable_name+"_X")
                self.x_params.AddEmptyValue("reaction_stress_variable_name").SetString(reaction_stress_variable_name+"_X")
                self.x_params.AddEmptyValue("loading_velocity_variable_name").SetString(loading_velocity_variable_name+"_X")
                self.x_params.AddValue("target_stress_table_id",settings["target_stress_table_id"][0])
                self.x_params.AddValue("initial_velocity",settings["initial_velocity"][0])
                self.x_params.AddValue("limit_velocity",settings["limit_velocity"][0])
                self.x_params.AddValue("velocity_factor",settings["velocity_factor"])
                self.x_params.AddValue("compression_length",settings["compression_length"])
                self.x_params.AddValue("young_modulus",settings["young_modulus"])
                self.x_params.AddValue("stress_increment_tolerance",settings["stress_increment_tolerance"])
                self.x_params.AddValue("update_stiffness",settings["update_stiffness"])
                self.x_params.AddValue("start_time",settings["start_time"])
                self.components_process_list.append(Dem.ControlModule2DProcess(self.model_part, self.x_params))
            if settings["fixed"][1].GetBool() == True:
                self.y_params = KratosMultiphysics.Parameters("{}")
                self.y_params.AddValue("model_part_name",settings["model_part_name"])
                self.y_params.AddEmptyValue("variable_name").SetString(variable_name+"_Y")
                self.y_params.AddEmptyValue("reaction_variable_name").SetString(reaction_variable_name+"_Y")
                self.y_params.AddEmptyValue("target_stress_variable_name").SetString(target_stress_variable_name+"_Y")
                self.y_params.AddEmptyValue("reaction_stress_variable_name").SetString(reaction_stress_variable_name+"_Y")
                self.y_params.AddEmptyValue("loading_velocity_variable_name").SetString(loading_velocity_variable_name+"_Y")
                self.y_params.AddValue("target_stress_table_id",settings["target_stress_table_id"][1])
                self.y_params.AddValue("initial_velocity",settings["initial_velocity"][1])
                self.y_params.AddValue("limit_velocity",settings["limit_velocity"][1])
                self.y_params.AddValue("velocity_factor",settings["velocity_factor"])
                self.y_params.AddValue("compression_length",settings["compression_length"])
                self.y_params.AddValue("young_modulus",settings["young_modulus"])
                self.y_params.AddValue("stress_increment_tolerance",settings["stress_increment_tolerance"])
                self.y_params.AddValue("update_stiffness",settings["update_stiffness"])
                self.y_params.AddValue("start_time",settings["start_time"])
                self.components_process_list.append(Dem.ControlModule2DProcess(self.model_part, self.y_params))

    def ExecuteInitialize(self):

        for component in self.components_process_list:
            component.ExecuteInitialize()

    def ExecuteInitializeSolutionStep(self):

        for component in self.components_process_list:
            component.ExecuteInitializeSolutionStep()

    def ExecuteFinalizeSolutionStep(self):

        for component in self.components_process_list:
            component.ExecuteFinalizeSolutionStep()