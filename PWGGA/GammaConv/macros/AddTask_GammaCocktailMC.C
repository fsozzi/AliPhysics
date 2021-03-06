void AddTask_GammaCocktailMC(Double_t maxy = 0.8, Bool_t runLightOutput = kFALSE) {

  // ================== GetAnalysisManager ===============================
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr) {
    Error("AddTask_GammaCocktailMC", "No analysis manager found.");
    return ;
  }

  // ================== GetInputEventHandler =============================
  AliVEventHandler *inputHandler=mgr->GetInputEventHandler();

  AliAnalysisDataContainer *cinput = mgr->GetCommonInputContainer();

  
  //================================================
  //========= Add task to the ANALYSIS manager =====
  //================================================
  //            find input container
  AliAnalysisTaskGammaCocktailMC *task=NULL;
  task= new AliAnalysisTaskGammaCocktailMC(Form("GammaCocktailMC_%1.2f",maxy));
  task->SetMaxY(maxy);
  task->SetLightOutput(runLightOutput);
  
  //connect containers
  AliAnalysisDataContainer *coutput =
    mgr->CreateContainer(Form("GammaCocktailMC_%1.2f",maxy), TList::Class(), AliAnalysisManager::kOutputContainer, Form("%s:GammaCocktailMC",AliAnalysisManager::GetCommonFileName()));
    
  mgr->AddTask(task);
  mgr->ConnectInput(task,0,cinput);
  mgr->ConnectOutput(task,1,coutput);
  
  return;
  
}
