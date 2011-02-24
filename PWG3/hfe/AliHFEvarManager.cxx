/**************************************************************************
* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
*                                                                        *
* Author: The ALICE Off-line Project.                                    *
* Contributors are mentioned in the code where appropriate.              *
*                                                                        *
* Permission to use, copy, modify and distribute this software and its   *
* documentation strictly for non-commercial purposes is hereby granted   *
* without fee, provided that the above copyright notice appears in all   *
* copies and that both the copyright notice and this permission notice   *
* appear in the supporting documentation. The authors make no claims     *
* about the suitability of this software for any purpose. It is          *
* provided "as is" without express or implied warranty.                  *
**************************************************************************/

/* $Id$ */

//
// Class AliHFEvarManager:
// Contains definition of the variables which are filled into the 
// Correction framework container. The class handles both AliCFContainer and
// AliHFEcontainer.
// Defining a new variable which has to be monitored can be done via the
// function AddVariable. This function also defines a new dimension for the
// particle container and appends it to the list of variables. For a new
// track the function NewTrack caches the values to be filled into the 
// correction framework container. With FillContainer the specified 
// correction framework container is filled. The VarManager also handles
// the filling of the correlation matrices
//
// Author:
//   Markus Fasel <M.Fasel@gsi.de>
//
#include <TClass.h>
#include <TH3.h>
#include <TF3.h>
#include <TMath.h>
#include <THnSparse.h>
#include <TString.h>

#include "AliCFContainer.h"
#include "AliLog.h"
#include "AliVParticle.h"

#include "AliHFEcontainer.h"
#include "AliHFEsignalCuts.h"
#include "AliHFEvarManager.h"

ClassImp(AliHFEvarManager)
ClassImp(AliHFEvarManager::AliHFEvariable);

//____________________________________________________________
AliHFEvarManager::AliHFEvarManager():
  TNamed(),
  fVariables(NULL),
  fContent(NULL),
  fContentMC(NULL),
  fWeightFactor(1.),
  fSignalTrack(kTRUE),
	fWeighting(kFALSE),
  fSignal(NULL),
	fWeightFactors(NULL),
	fWeightFactorsFunction(NULL)
{
	//
	// Dummy constructor
  //
  SetOwner();
}

//____________________________________________________________
AliHFEvarManager::AliHFEvarManager(const Char_t *name):
  TNamed(name, ""),
  fVariables(NULL),
  fContent(NULL),
  fContentMC(NULL),
  fWeightFactor(1.),
  fSignalTrack(kTRUE),
	fWeighting(kFALSE),
  fSignal(NULL),
	fWeightFactors(NULL),
	fWeightFactorsFunction(NULL)
{
	//
	// Default constructor
  //
  fVariables = new TObjArray;
  SetOwner();
}

//____________________________________________________________
AliHFEvarManager::AliHFEvarManager(const AliHFEvarManager &ref):
  TNamed(ref),
  fVariables(NULL),
  fContent(NULL),
  fContentMC(NULL),
  fWeightFactor(ref.fWeightFactor),
  fSignalTrack(ref.fSignalTrack),
	fWeighting(ref.fWeighting),
  fSignal(NULL),
	fWeightFactors(NULL),
	fWeightFactorsFunction(NULL)
{
  //
  // Copy Constructor
  //
  ref.Copy(*this);
}

//____________________________________________________________
AliHFEvarManager &AliHFEvarManager::operator=(const AliHFEvarManager &ref){
  //
  // Assignment operator
  //
  if(&ref != this){ 
    this->~AliHFEvarManager();
    ref.Copy(*this);
  }
  return *this;
}

//____________________________________________________________
AliHFEvarManager::~AliHFEvarManager(){
	//
	// Destructor
	//
  if(IsOwner()){
    if(fVariables) delete fVariables;
  }
  if(fContent) delete[] fContent;
  if(fContentMC) delete[] fContentMC;
}

//____________________________________________________________
void AliHFEvarManager::Copy(TObject &o) const{
  //
  // Make Copy
  //
  AliHFEvarManager &target = dynamic_cast<AliHFEvarManager &>(o);
  target.fVariables = fVariables;
  target.fContent = new Double_t[sizeof(fContent)/sizeof(Double_t)]; 
  target.fContentMC = new Double_t[sizeof(fContentMC)/sizeof(Double_t)];
  target.fWeightFactor = fWeightFactor;
  target.fSignalTrack = fSignalTrack;
	target.fWeighting = fWeighting;
  target.fSignal = fSignal;
	target.fWeightFactors = fWeightFactors;
  target.fWeightFactorsFunction = fWeightFactorsFunction; 
  target.SetOwner(kFALSE);
}

//____________________________________________________________
void AliHFEvarManager::AddVariable(TString name){
  //
  // Add new variable to the var manager
  // Value derived via GetValue()
  //
   AliDebug(1, Form("Var Name: %s", name.Data()));

  if(!name.CompareTo("pt")) 
    fVariables->AddLast(new AliHFEvariable("pt", "pt", kPt, 44, 0.1, 20, kTRUE));
  else if(!name.CompareTo("eta"))
    fVariables->AddLast(new AliHFEvariable("eta", "eta", kEta, 8, -0.8, 0.8));
  else if(!name.CompareTo("phi"))
    fVariables->AddLast(new AliHFEvariable("phi", "phi", kPhi, 18, -0, 2*TMath::Pi()));
  else if(!name.CompareTo("charge"))
    fVariables->AddLast(new AliHFEvariable("charge", "charge", kCharge, 2, -1.1, 1.1));
  else if(!name.CompareTo("source"))
    fVariables->AddLast(new AliHFEvariable("source", "source", kSource, 4, 0, 4));
  else if(!name.CompareTo("centrality"))
    fVariables->AddLast(new AliHFEvariable("centrality", "centrality", kCentrality, 11, 0.0, 11.0));
  else if(!name.CompareTo("species"))
    fVariables->AddLast(new AliHFEvariable("species", "species", kSpecies, 6, -1, 5));

  // More to come ...
}

//____________________________________________________________
Bool_t AliHFEvarManager::IsVariableDefined(TString name){
  //
  // Add new variable to the var manager
  // Value derived via GetValue()
  //
   AliDebug(1, Form("Var Name: %s", name.Data()));

   AliHFEvariable *u = (AliHFEvariable *) fVariables->FindObject((const char*)name);
   if(u) return kTRUE;
   else return kFALSE;
  
   // More to come ...
}

//____________________________________________________________
void AliHFEvarManager::DefineVariables(AliHFEcontainer *cont){
	//
	// Define Variables
	//
  Int_t nVars = fVariables->GetEntriesFast();
  cont->SetNumberOfVariables(nVars);
  TIter vars(fVariables);
  AliHFEvariable *var;
  Int_t counter = 0;
  while((var = dynamic_cast<AliHFEvariable *>(vars()))){
    cont->SetVariableName(counter, var->GetName());
    if(var->IsLogarithmic())
      cont->MakeLogarithmicBinning(counter, var->GetNumberOfBins(), var->GetMinimum(), var->GetMaximum());
    else
      cont->MakeLinearBinning(counter, var->GetNumberOfBins(), var->GetMinimum(), var->GetMaximum());
    counter++;
  }
	fContent = new Double_t[nVars]; 
  memset(fContent, 0, sizeof(Double_t) * nVars);
  fContentMC = new Double_t[nVars]; 
  memset(fContentMC, 0, sizeof(Double_t) * nVars);
}

//____________________________________________________________
void AliHFEvarManager::NewTrack(AliVParticle *recTrack, AliVParticle *mcTrack, Float_t centrality, Int_t aprioriPID, Bool_t signal){
  //
  // Cache information for new track pair
  //
  AliDebug(1, "Filling new Track");
  fSignalTrack = signal;
  FillArray(recTrack, fContent, centrality, aprioriPID);
  if(mcTrack) FillArray(mcTrack, fContentMC, centrality, aprioriPID);
  if(fWeighting){
    Int_t indexpt = -1, indexeta = -1, indexphi = -1, counter = 0;
    AliHFEvariable *var = NULL;
    TIter vars(fVariables);
    while((var = dynamic_cast<AliHFEvariable *>(vars()))){
      switch(var->GetVarCode()){
        case kPt:   indexpt = counter;  break;
        case kEta:  indexeta = counter; break;
        case kPhi:  indexphi = counter; break;
      };
      if(indexpt >= 0 && indexeta >= 0 && indexphi >= 0) // all dimensions found
        break;
      counter++;
    }
    if(indexpt >= 0 && indexeta >= 0 && indexphi >= 0)
      fWeightFactor = FindWeight(fContent[indexpt], fContent[indexeta], fContent[indexphi]);
  }
}

//____________________________________________________________
Double_t AliHFEvarManager::GetValue(AliVParticle *track, UInt_t code, Float_t centrality, Int_t aprioriPID) const {
  //
  // Definition of the variables
  //
  if(!track) return 0.;
  Double_t value = 0.;
  switch(code){
    case kPt:       value = track->Pt();  break;
    case kEta:      value = track->Eta(); break;
    case kPhi:      value = track->Phi(); break;
    case kCharge:{
      value = track->Charge();
      if(TString(track->IsA()->GetName()).Contains("MC")) value /= 3;
      break;
    }
    case kSource:{
      if(fSignal){
	      if(fSignal->IsCharmElectron(track)) value = 0;
	      else if(fSignal->IsBeautyElectron(track)) value = 1;
	      else if(fSignal->IsGammaElectron(track)) value = 2;
	      else value = 3;
      }
      break;
    }
    case kSpecies:  value = aprioriPID; break; 
    case kCentrality:  value = centrality; break;  
  };
  return value;
}

//____________________________________________________________
void AliHFEvarManager::FillArray(AliVParticle *track, Double_t* container, Float_t centrality, Int_t aprioriPID) const{
	//
	// Fill array with variables
	//
  TIter vars(fVariables);
  AliHFEvariable *var = NULL;
  Int_t counter = 0;
  while((var = dynamic_cast<AliHFEvariable *>(vars())))
    container[counter++] = GetValue(track , var->GetVarCode(), centrality, aprioriPID);
}

//____________________________________________________________
void AliHFEvarManager::FillContainer(AliCFContainer *cont, Int_t step, Bool_t useMC) const{
	//
	// Fill CF container with defined content
	//

	// Do reweighting if necessary
  Double_t *content = fContent;
  if(useMC) content = fContentMC;
	cont->Fill(content, step, fWeightFactor);
}

//____________________________________________________________
void AliHFEvarManager::FillContainer(AliHFEcontainer *cont, const Char_t *contname, UInt_t step, Bool_t useMC, Double_t externalWeight) const {
	//
	// Fill CF container with defined content
	//

  // Do reweighting if necessary
  Double_t *content = fContent;
  if(useMC) content = fContentMC;
	cont->FillCFContainer(contname, step, content, fWeightFactor * externalWeight);
}  

//____________________________________________________________
void AliHFEvarManager::FillContainerStepname(AliHFEcontainer *cont, const Char_t *contname, const Char_t *step, Bool_t useMC, Double_t externalWeight) const {
	//
	// Fill CF container with defined content
	//

  // Do reweighting if necessary
  Double_t *content = fContent;
  if(useMC) content = fContentMC;
	cont->FillCFContainerStepname(contname, step, content, fWeightFactor * externalWeight);
}  

//____________________________________________________________
void AliHFEvarManager::FillCorrelationMatrix(THnSparseF *matrix) const {
	//
	// Fill Correlation Matrix
	//

	// Do reweighting if necessary
  Double_t content[10];
  memcpy(content, fContent, sizeof(Double_t) * 5);
  memcpy(&content[5], fContentMC, sizeof(Double_t) * 5);
	matrix->Fill(content, fWeightFactor);
}

//_______________________________________________
void AliHFEvarManager::SetWeightFactors(TH3F *weightFactors){
	//
	// Set the histos with the weights for the efficiency maps
	//
	fWeighting = kTRUE;
	fWeightFactors = weightFactors;
}

//_______________________________________________
void AliHFEvarManager::SetWeightFactorsFunction(TF3 *weightFactorsFunction){
	//
	// Set the histos with the weights for the efficiency maps
	//
	fWeighting = kTRUE;
	fWeightFactorsFunction = weightFactorsFunction;
}

//_______________________________________________
Double_t AliHFEvarManager::FindWeight(Double_t pt, Double_t eta, Double_t phi) const {
	//
	// Find the weight corresponding to pt eta and phi in the TH3D
	//
	Double_t weight = 1.0;
	if(fWeightFactors) {

		TAxis *ptaxis = fWeightFactors->GetXaxis();
		TAxis *etaaxis = fWeightFactors->GetYaxis();
		TAxis *phiaxis = fWeightFactors->GetZaxis();

		Int_t ptbin = ptaxis->FindBin(pt);
		Int_t etabin = etaaxis->FindBin(eta);
		Int_t phibin = phiaxis->FindBin(phi);


		weight = fWeightFactors->GetBinContent(ptbin,etabin,phibin);
	}
	else if(fWeightFactorsFunction) {

		weight = fWeightFactorsFunction->Eval(pt,eta,phi);

	}

	AliDebug(2, Form("pt %f, eta %f, phi %f, weight %f",pt,eta,phi,weight));

	return weight;
}

//_______________________________________________
AliHFEvarManager::AliHFEvariable::AliHFEvariable():
    TNamed()
  , fCode(0)
  , fNBins(0)
  , fMin(0)
  , fMax(0)
  , fIsLogarithmic(kFALSE)
{
  // 
  // Dummy constructor
  //
}

//_______________________________________________
AliHFEvarManager::AliHFEvariable::AliHFEvariable(const Char_t *name, const Char_t *title, UInt_t code, UInt_t nBins, Double_t min, Double_t max, Bool_t isLogarithmic):
    TNamed(name, title)
  , fCode(code)
  , fNBins(nBins)
  , fMin(min)
  , fMax(max)
  , fIsLogarithmic(isLogarithmic)
{
  // 
  // Default constructor
  //
}

//_______________________________________________
AliHFEvarManager::AliHFEvariable::AliHFEvariable(const AliHFEvarManager::AliHFEvariable &ref):
    TNamed(ref)
  , fCode(ref.fCode)
  , fNBins(ref.fNBins)
  , fMin(ref.fMin)
  , fMax(ref.fMax)
  , fIsLogarithmic(ref.fIsLogarithmic)
{
  // 
  // Copy constructor
  //
}

//_______________________________________________
AliHFEvarManager::AliHFEvariable& AliHFEvarManager::AliHFEvariable::operator=(const AliHFEvarManager::AliHFEvariable &ref){
  //
  // Assignment operator
  //
  
  if(&ref != this){
    TNamed::operator=(ref);
    fCode = ref.fCode;
    fNBins = ref.fNBins;
    fMax = ref.fMax;
    fMin = ref.fMin;
    fIsLogarithmic = ref.fIsLogarithmic;
  }
  return *this;
}

