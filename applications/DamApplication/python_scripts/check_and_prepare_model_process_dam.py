import KratosMultiphysics 

def Factory(settings, Model):
    if(type(settings) != KratosMultiphysics.Parameters):
        raise Exception("expected input shall be a Parameters object, encapsulating a json string")
    return CheckAndPrepareModelProcess(Model, settings["Parameters"])


##all the python processes should be derived from "python_process"
class CheckAndPrepareModelProcess(KratosMultiphysics.Process):
    
    def __init__(self, main_model_part, Parameters ):
        
        self.main_model_part = main_model_part
        
        self.thermal_model_part_name  = Parameters["thermal_model_part_name"].GetString()
        self.thermal_domain_sub_model_part_list = Parameters["thermal_domain_sub_model_part_list"]
        
        self.mechanical_model_part_name  = Parameters["mechanical_model_part_name"].GetString()
        self.mechanical_domain_sub_model_part_list = Parameters["mechanical_domain_sub_model_part_list"]
        #~ self.processes_sub_model_part_list = Parameters["processes_sub_model_part_list"]
        self.body_domain_sub_model_part_list = Parameters["body_domain_sub_model_part_list"]
        self.loads_sub_model_part_list = Parameters["loads_sub_model_part_list"]

    def Execute(self):
        
        ## Construct the thermal model part:
        thermal_parts = []
        for i in range(self.thermal_domain_sub_model_part_list.size()):
            thermal_parts.append(self.main_model_part.GetSubModelPart(self.thermal_domain_sub_model_part_list[i].GetString()))
        self.main_model_part.CreateSubModelPart(self.thermal_model_part_name)
        thermal_model_part = self.main_model_part.GetSubModelPart(self.thermal_model_part_name)
        thermal_model_part.ProcessInfo = self.main_model_part.ProcessInfo
        thermal_model_part.Properties = self.main_model_part.Properties
        thermal_model_part.Set(KratosMultiphysics.ACTIVE)
        print("Adding nodes and elements to thermal_model_part")
        list_of_ids = set()
        for part in thermal_parts:
            for node in part.Nodes:
                list_of_ids.add(node.Id)
        thermal_model_part.AddNodes(list(list_of_ids))
        list_of_ids = set()
        for part in thermal_parts:
            for elem in part.Elements:
                list_of_ids.add(elem.Id)
        thermal_model_part.AddElements(list(list_of_ids))
        print(thermal_model_part)
        
        ## Construct the mechanical model part:
        mechanical_parts = []
        for i in range(self.mechanical_domain_sub_model_part_list.size()):
            mechanical_parts.append(self.main_model_part.GetSubModelPart(self.mechanical_domain_sub_model_part_list[i].GetString()))
        #~ processes_parts = []
        #~ for i in range(self.processes_sub_model_part_list.size()):
            #~ processes_parts.append(self.main_model_part.GetSubModelPart(self.processes_sub_model_part_list[i].GetString()))
        self.main_model_part.CreateSubModelPart(self.mechanical_model_part_name)
        mechanical_model_part = self.main_model_part.GetSubModelPart(self.mechanical_model_part_name)
        mechanical_model_part.ProcessInfo = self.main_model_part.ProcessInfo
        mechanical_model_part.Properties = self.main_model_part.Properties
        mechanical_model_part.Set(KratosMultiphysics.ACTIVE)
        print("Adding nodes and elements to mechanical_model_part")
        for node in self.main_model_part.Nodes:
            mechanical_model_part.AddNode(node,0)
        list_of_ids = set()
        for part in mechanical_parts:
            for elem in part.Elements:
                list_of_ids.add(elem.Id)
        mechanical_model_part.AddElements(list(list_of_ids))
        for cond in self.main_model_part.Conditions:
            mechanical_model_part.AddCondition(cond,0)
        #~ list_of_ids = set()
        #~ for part in processes_parts:
            #~ for cond in part.Conditions:
                #~ list_of_ids.add(cond.Id)
        #~ mechanical_model_part.AddConditions(list(list_of_ids))
        # Sub sub model parts
        for i in range(self.body_domain_sub_model_part_list.size()):
            body_sub_model_part = self.main_model_part.GetSubModelPart(self.body_domain_sub_model_part_list[i].GetString())
            mechanical_model_part.CreateSubModelPart(self.body_domain_sub_model_part_list[i].GetString())
            body_sub_sub_model_part = mechanical_model_part.GetSubModelPart(self.body_domain_sub_model_part_list[i].GetString())
            list_of_ids = set()
            for elem in body_sub_model_part.Elements:
                list_of_ids.add(elem.Id)
            body_sub_sub_model_part.AddElements(list(list_of_ids))
        for i in range(self.loads_sub_model_part_list.size()):
            load_sub_model_part = self.main_model_part.GetSubModelPart(self.loads_sub_model_part_list[i].GetString())
            mechanical_model_part.CreateSubModelPart(self.loads_sub_model_part_list[i].GetString())
            load_sub_sub_model_part = mechanical_model_part.GetSubModelPart(self.loads_sub_model_part_list[i].GetString())
            list_of_ids = set()
            for node in load_sub_model_part.Nodes:
                list_of_ids.add(node.Id)
            load_sub_sub_model_part.AddNodes(list(list_of_ids))
        print(mechanical_model_part)
