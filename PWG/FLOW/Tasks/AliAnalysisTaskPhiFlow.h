/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. */
/* See cxx source for full Copyright notice */
/* $Id$ */

// AliAnalysisTaskPhiFlow:
// origin: Redmer Alexander Bertens (rbertens@nikhef.nl)
// analyis task for phi-meson reconstruction and estimation of v_n

#ifndef ALIANALYSISTASKPHIFLOW_H
#define ALIANALYSISTASKPHIFLOW_H

class TH1F;
class TH2F;
class TProfile;
class AliESDEvent;
class AliESDtrackCuts;
class AliFlowTrackCuts;
class AliFlowEvent;
class AliFlowCandidateTrack;
class AliFlowBayesianPID;
class AliEventPoolManager;

#include "AliAnalysisTaskSE.h"

enum ESDEventStats_t
{
   kNUnlikePairs = 0,
   kNLikeNPairs,
   kNLikePPairs,
   kNUnlikeKPairs,
   kNLikeNKPairs,
   kNLikePKPairs,
   kNStats = kNLikePKPairs,
};

class AliPhiMesonHelperTrack : public TObject
{
public:
        AliPhiMesonHelperTrack(Float_t eta, Float_t phi, Float_t p, Float_t px, Float_t py, Float_t pz, Float_t pt, Int_t charge) : fEta(eta), fPhi(phi), fp(p), fpX(px), fpY(py), fpZ(pz), fpT(pt), fCharge(charge) {  }
    ~AliPhiMesonHelperTrack() {}
    virtual Double_t P()                const { return fp; }
    virtual Double_t Px()               const { return fpX; }
    virtual Double_t Py()               const { return fpY; }
    virtual Double_t Pz()               const { return fpZ; }
    virtual Double_t Pt()               const { return fpT; }
    virtual Double_t Phi()              const { return fPhi; }
    virtual Double_t Eta()              const { return fEta; }
    virtual Int_t Charge()              const { return fCharge; }
    void    InitializeHelperTrack(Float_t eta, Float_t phi, Float_t p, Float_t px, Float_t py, Float_t pz, Float_t pt, Int_t charge) {     fEta = eta;
                                                                                                                                fPhi =phi;
                                                                                                                                fp = p;
                                                                                                                                fpX = px;
                                                                                                                                fpY = py;
                                                                                                                                fpZ = pz;
                                                                                                                                fpT = pt;
                                                                                                                                fCharge = charge; }
private:
    Float_t                             fEta;      // eta
    Float_t                             fPhi;      // phi
    Float_t                             fp;        // p
    Float_t                             fpX;       // pX
    Float_t                             fpY;       // pY
    Float_t                             fpZ;       // pZ
    Float_t                             fpT;       // pT
    Int_t                               fCharge;   // charge

    ClassDef(AliPhiMesonHelperTrack, 1); // lightweight helper track for phi reconstruction
};

class AliAnalysisTaskPhiFlow : public AliAnalysisTaskSE
{
public:
   AliAnalysisTaskPhiFlow();
   AliAnalysisTaskPhiFlow(const char *name);
   virtual ~AliAnalysisTaskPhiFlow();

   Int_t                                SetDebugLevelPhiTask(Int_t debug) {fDebug = debug; return fDebug; }
   Bool_t                               SetIsMC(Bool_t ismc) {fIsMC = ismc; return fIsMC; }
   Bool_t                               UseEventMixing(Bool_t mix, Bool_t type) {fEventMixing = mix; fTypeMixing = type; return mix; }
   Bool_t                               SetVZEROSubEvents(Bool_t v0) { fV0 = v0; return v0; }
   TH1F*                                BookHistogram(const char * name);
   TH2F*                                BookPIDHistogram(const char * name, Bool_t TPC);
   TH1F*                                InitPtSpectraHistograms(Int_t i);
   TH1F*                                BookPtHistogram(const char* name);
   void                                 AddPhiIdentificationOutputObjects();
   virtual void                         UserCreateOutputObjects();
   AliEventPoolManager*                 InitializeEventMixing();
   template <typename T> Double_t       InvariantMass(const T* track1, const T* track2) const;
   template <typename T> Double_t       DeltaDipAngle(const T* track1, const T* track2) const;
   template <typename T> Bool_t         CheckDeltaDipAngle(const T* track1, const T* track2) const;
   template <typename T> Bool_t         CheckCandidateEtaPtCut(const T* track1, const T* track2) const;
   void                                 SetCentralityParameters(Double_t CentralityMin, Double_t CentralityMax, const char* CentralityMethod) { 
                                                                                          fCentralityMin = CentralityMin; 
                                                                                          fCentralityMax = CentralityMax; 
                                                                                          fkCentralityMethod = CentralityMethod; }
   Double_t                             GetCenMin() const {return fCentralityMin; }
   Double_t                             GetCenMax() const {return fCentralityMax; }
   const char*                          GetCentralityMethod() const {return fkCentralityMethod; }
   void                                 SetVertexZ(Float_t z) { fVertexRange = z; }
   Float_t                              GetVertexZ() const { return fVertexRange; }
   void                                 SetMaxDeltaDipAngleAndPt(Float_t a, Float_t pt) { fDeltaDipAngle = a;
                                                                                          fDeltaDipPt = pt;
                                                                                          fApplyDeltaDipCut = kTRUE; };
   Float_t                              GetDeltaDipAngle() const {return fDeltaDipAngle; }
   Float_t                              GetDeltaDipPt() const {return fDeltaDipPt; }
   template <typename T> Bool_t         EventCut(T* event);
   template <typename T> void           PlotMultiplcities(const T* event) const;
   template <typename T> Bool_t         CheckVertex(const T* event);
   template <typename T> Bool_t         CheckCentrality(T* event);
   void                                 InitializeBayesianPID(AliESDEvent* event);
   void                                 InitializeBayesianPID(AliAODEvent* event);
   template <typename T> Bool_t         PassesTPCbayesianCut(T* track) const;
   Bool_t                               PassesDCACut(AliAODTrack* track) const;
   Bool_t                               IsKaon(AliESDtrack* track) const;
   Bool_t                               IsKaon(AliAODTrack* track) const;
   template <typename T> Double_t       PhiPt(const T* track_1, const T* track_2) const;
   template <typename T> void           PtSelector(Int_t _track_type, const T* track_1, const T* track_2) const;
   template <typename T> Bool_t         PhiTrack(T* track) const;
   template <typename T> void           SetNullCuts(T* esd);
   void                                 PrepareFlowEvent(Int_t iMulti);
   void                                 VZEROSubEventAnalysis();
   virtual void                         UserExec(Option_t *option);
   void                                 ReconstructionWithEventMixing(TObjArray* MixingCandidates) const;
   virtual void                         Terminate(Option_t *);
   void                                 SetPOICuts(AliFlowTrackCuts *cutsPOI) { fPOICuts = cutsPOI; }
   void                                 SetRPCuts(AliFlowTrackCuts *cutsRP) { fCutsRP = cutsRP; }
   void                                 SetRequireTPCStandAloneKaons() { fRequireTPCStandAlone = kTRUE; }
   void                                 SetOlderThanPbPbPass2() { fOldTrackParam = kTRUE; }
   void                                 SetPIDConfiguration(Double_t prob[7]) { for(Int_t i = 0; i < 7; i++) fPIDConfig[i] = prob[i]; }
   void                                 GetPIDConfiguration(Double_t prob[7]) const {for(Int_t i = 0; i < 7; i++) prob[i] = fPIDConfig[i]; }
   void                                 SetPOIDCAXYZ(Double_t dca[5]) { for(Int_t i = 0; i < 5; i++) fDCAConfig[i] = dca[i]; }
   void                                 GetPOIDCZXYZ(Double_t dca[5]) const { for(Int_t i = 0; i < 5; i++) dca[i] = fDCAConfig[i]; }
   void                                 SetMixingBins(Int_t c[20], Int_t v[20]) {for(Int_t i = 0; i < 20; i++) { fCentralityMixingBins[i] = c[i];
                                                                                                                 fVertexMixingBins[i] = v[i]; } }
   void                                 SetMixingParameters(Int_t p[3]) { for(Int_t i = 0; i < 3; i++) fMixingParameters[i] = p[i]; }
   void                                 GetMixingParameters(Int_t p[3]) const { for(Int_t i = 0; i < 3; i++) p[i] = fMixingParameters[i]; } 
   void                                 SetCandidateEtaAndPt(Double_t mineta, Double_t maxeta, Double_t minpt, Double_t maxpt) { fCandidateMinEta = mineta;
                                                                                                                                fCandidateMaxEta = maxeta;
                                                                                                                                fCandidateMinPt = minpt;
                                                                                                                                fCandidateMaxPt = maxpt;
                                                                                                                                fCandidateEtaPtCut = kTRUE;}
   void                                 GetCandidateEtaAndPt(Double_t etapt[4]) const { etapt[0] = fCandidateMinEta;
                                                                                        etapt[1] = fCandidateMaxEta;
                                                                                        etapt[2] = fCandidateMinPt;
                                                                                        etapt[3] = fCandidateMaxPt; }
   void                                 SetCommonConstants(Int_t massBins, Double_t minMass, Double_t maxMass) {        fMassBins = massBins;
                                                                                                                        fMinMass = minMass;
                                                                                                                        fMaxMass= maxMass; }
   void                                 IsMC();
   Bool_t                               SetQA(Bool_t qa) {fQA = qa; return fQA;}

private:

   Int_t                fDebug; // debug level (0 none, 1 fcn calls, 2 verbose)
   Bool_t               fIsMC; // use mc mode
   Bool_t               fEventMixing; // use event mixing
   Bool_t               fTypeMixing; // select type: kTRUE for unlike sign background, kFALSE for like sign background
   Bool_t               fQA; // make qa plots
   Bool_t               fV0; // use three subevents including vzero
   Bool_t               fAODAnalysis; // set aod analysis
   Int_t                fMassBins; // mass bins
   Double_t             fMinMass; // mass range
   Double_t             fMaxMass; // mass range
   AliFlowTrackCuts     *fCutsRP; // track cuts for reference particles
   AliFlowTrackCuts     *fNullCuts; // dummy cuts for flow event tracks
   AliPIDResponse       *fPIDResponse; //! pid response object
   AliFlowEvent         *fFlowEvent; //! flow events (one for each inv mass band)
   AliFlowBayesianPID   *fBayesianResponse; //!PID response object
   TObjArray            *fCandidates; // candidate array
   Bool_t               fOldTrackParam; // set to true if data is older than pbpb pass 2 production
   Bool_t               fRequireTPCStandAlone; // set TPC standalone cut for kaon selection
   Bool_t               fCandidateEtaPtCut; // set eta and pt cut for candidate tracks and combinatorial background
   Double_t             fCandidateMinEta; // minimum eta for candidates
   Double_t             fCandidateMaxEta; // maximum eta for candidates
   Double_t             fCandidateMinPt; // minimum pt for candidates
   Double_t             fCandidateMaxPt; // maximum pt for candidates
   Double_t             fPIDConfig[7]; // configure pid routine
   Double_t             fDCAConfig[5]; // configure dca routine
   Int_t                fMixingParameters[3]; // mixing: poolsize, mixing tracks, pool buffer
   Int_t                fCentralityMixingBins[20]; // configure centrality bins for event mixing
   Int_t                fVertexMixingBins[20]; // configure vertex bins for event mixing
   Double_t             fCentrality; // event centrality
   Double_t             fVertex; // event vertex z 
   AliESDEvent          *fESD;    //! ESD object
   AliAODEvent          *fAOD;    //! AOD oject
   AliEventPoolManager  *fPoolManager; //! event pool manager
   TList                *fOutputList; // ! Output list
   TH1F                 *fEventStats; // ! Histogram for event statistics
   TH1F                 *fCentralityPass; // ! QA histogram of events that pass centrality cut
   TH1F                 *fCentralityNoPass; //! QA histogram of events that do not pass centrality cut
   TH2F                 *fNOPID;//! QA histogram of TPC response of all charged particles
   TH2F                 *fPIDk;//! QA histogram of TPC response of kaons
   TH2F                 *fNOPIDTOF; //! QA histo of TOF repsonse charged particles
   TH2F                 *fPIDTOF; //! QA histo of TOF response kaons
   TH1F                 *fInvMNP[18]; //! unlike sign kaon pairs
   TH1F                 *fInvMNN[18]; //! like-sign kaon pairs
   TH1F                 *fInvMPP[18]; //! like-sign kaon pairs
   TH1F                 *fPtSpectra[18]; //! pt spectra
   TH1F                 *fPtP; //! QA histogram of p_t distribution of positive particles
   TH1F                 *fPtN; //! QA histogram of p_t distribution of negative particles
   TH1F                 *fPtKP; //! QA histogram of p_t distribution of positive kaons
   TH1F                 *fPtKN; //! QA histogram of p_t distribution of negative kaons
   Double_t             fCentralityMin; // lower bound of cenrality bin
   Double_t             fCentralityMax; // upper bound of centrality bin
   const char           *fkCentralityMethod; // method used to determine centrality (V0 by default)
   AliFlowTrackCuts     *fPOICuts; // cuts for particles of interest (flow package)
   Float_t              fVertexRange; // absolute value of maximum distance of vertex along the z-axis
   TH1F                 *fPhi; //! QA plot of azimuthal distribution of tracks used for event plane estimation
   TH1F                 *fPt; //! QA plot of p_t sectrum of tracks used for event plane estimation
   TH1F                 *fEta; //! QA plot of eta distribution of tracks used for event plane estimation
   TH1F                 *fVZEROA; //! QA plot vzeroa multiplicity (all tracks in event)
   TH1F                 *fVZEROC; //! QA plot vzeroc multiplicity (all tracks in event)
   TH1F                 *fTPCM; //! QA plot TPC multiplicity (tracks used for event plane estimation)
   Float_t              fDeltaDipAngle; // absolute value of delta dip angle to be excluded
   Float_t              fDeltaDipPt; // upper value of pt range in which delta dip angle must be applied
   Bool_t               fApplyDeltaDipCut; // enforce delta dip cut
   TH2F                 *fDCAAll;//! qa dca of all charged particles
   TH1F                 *fDCAXYQA; //! qa plot of dca xz
   TH1F                 *fDCAZQA; //!qa plot of dca z
   TH2F                 *fDCAPrim; //!dca of primaries (mc) or kaons (data)
   TH2F                 *fDCASecondaryWeak; //! dca of weak (mc)
   TH2F                 *fDCAMaterial; //!dca material (mc) all (data)
   TProfile             *fSubEventDPhiv2; //! subevent resolution info for v2
   TProfile             *fSubEventDPhiv3; //! subevent resolution info for v3
   TProfile             *fV0Data[18][2][2]; //! profiles for vzero vn(minv)

   AliAnalysisTaskPhiFlow(const AliAnalysisTaskPhiFlow&); // Not implemented
   AliAnalysisTaskPhiFlow& operator=(const AliAnalysisTaskPhiFlow&); // Not implemented
   void                 MakeTrack(Double_t, Double_t, Double_t, Double_t, Int_t , Int_t[]) const;

   ClassDef(AliAnalysisTaskPhiFlow, 5);
};

#endif



