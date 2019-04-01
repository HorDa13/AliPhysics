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

///////////////////////////////////////////////////////////////////////////////////
//                                                                               //
//  Task for beauty decay electron spectra in pp collisions with TPC and EMCal   //
//  Author: Deepa Thomas and Erin Gauger (University of Texas at Austin)         //
//                                                                               //
///////////////////////////////////////////////////////////////////////////////////

#include <vector>
#include "TChain.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "THnSparse.h"

#include "AliAnalysisTask.h"
#include "AliAnalysisManager.h"

#include "AliESDEvent.h"
#include "AliESDInputHandler.h"
#include "AliESDtrackCuts.h"
#include "AliAODEvent.h"
#include "AliAODHandler.h"

#include "AliAODMCParticle.h"
#include "AliAODMCHeader.h"

#include "AliMCEventHandler.h"
#include "AliMCEvent.h"
#include "AliMCParticle.h"
#include "AliGenHijingEventHeader.h"
#include "AliGenPythiaEventHeader.h"

#include "AliMultSelection.h"
#include "AliPID.h"
#include "AliESDpid.h"
#include "AliAODPid.h"
#include "AliPIDResponse.h"
#include "AliCentrality.h"
#include "AliEventplane.h"

#include "AliKFParticle.h"
#include "AliKFVertex.h"
#include "AliEMCALTriggerPatchInfo.h"
#include "AliEMCALGeometry.h"

//#include "AliQnCorrectionsManager.h"

#include "AliAnalysisTaskHFEBESpectraEMC.h"

using std::cout;
using std::endl;

ClassImp(AliAnalysisTaskHFEBESpectraEMC)
//________________________________________________________________________
AliAnalysisTaskHFEBESpectraEMC::AliAnalysisTaskHFEBESpectraEMC(const char *name)
: AliAnalysisTaskSE(name),
fVevent(0),
fESD(0),
fAOD(0),
fMCHeader(0),
fpidResponse(0),
fEMCALGeo(0),
fFlagSparse(kFALSE),
fUseTender(kTRUE),
fTracks_tender(0),
fCaloClusters_tender(0),
fMCparticle(0),
fMCArray(0),
fMultSelection(0),
fFlagClsTypeEMC(kTRUE),
fFlagClsTypeDCAL(kTRUE),
fcentMim(0),
fcentMax(0),
fCentralityEstimator("V0M"),
fTPCnSigma(-999.0),
fTPCnSigmaMin(-1),
fTPCnSigmaMax(3),
fM02Min(0.05),
fM02Max(0.5),
fM20Min(0.0),
fM20Max(2000),
fEovPMin(0.9),
fEovPMax(1.2),
fNEle(0),
fTPCnSigmaHadMin(-10),
fTPCnSigmaHadMax(-3.5),
fInvmassCut(0.15),
fCalculateWeight(kFALSE),
fCalculateNonHFEEffi(kFALSE),
fNTotMCpart(0),
fNpureMC(0),
fNembMCpi0(0),
fNembMCeta(0),
fIsFrmEmbPi0(kFALSE),
fIsFrmEmbEta(kFALSE),
ftype(-1),
fWeight(1),
fPi0Weight(0),
fEtaWeight(0),
fOutputList(0),
fNevents(0),
fCent(0),
fMult(0),
fVtxZ(0),
fVtxX(0),
fVtxY(0),
fTrigMulti(0),
fHistClustE(0),
fHistNonLinClustE(0),
fHistClustEcent(0),
fEMCClsEtaPhi(0),
fHistClustEEG1(0),
fHistClustEEG1cent(0),
fHistClustEEG2(0),
fHistClustEEG2cent(0),
fEMCClsEtaPhiEG1(0),
fEMCClsEtaPhiEG2(0),
fHistoNCls(0),
fHistoNCells(0),
fHistoEperCell(0),
fHistoCalCell(0),
fHistoTimeEMC(0),
fNegTrkIDPt(0),
fTrkPt(0),
fTrketa(0),
fTrkphi(0),
fdEdx(0),
fTPCNpts(0),
fTPCnsig(0),
fTPCnsigMcEle(0),
fTPCnsigMcHad(0),
fTPCnsig_Pi(0),
fHistPtMatch(0),
fEMCTrkMatch(0),
fEMCTrkPt(0),
fEMCTrketa(0),
fEMCTrkphi(0),
fEMCdEdx(0),
fEMCTPCnsig(0),
fEMCTPCNpts(0),
fClsEAftMatch(0),
fNonLinClsEAftMatch(0),
fClsEtaPhiAftMatch(0),
fClsEtaPhiAftMatchEMCin(0),
fClsEtaPhiAftMatchEMCout(0),
fHistdEdxEop(0),
fHistNsigEop(0),
fHistEop(0),
fHistMcEopEle(0),
fHistMcEopHad(0),
fM20(0),
fM02(0),
fM20EovP(0),
fM02EovP(0),
fEleCanITShit(0),
fInvmassULSPt(0),
fInvmassLSPt(0),
fHFmomCorr(0),
fEMCTrkMatch_Phi(0),
fEMCTrkMatch_Eta(0),
fInclsElecPt(0),
fHadPt_AftEID(0),
fHistEop_AftEID(0),
fNElecInEvt(0),
fULSElecPt(0),
fLSElecPt(0),
fRealInclsElecPt(0),
fNonHFeTrkPt(0),
fMissingEmbEtaEleTrkPt(0),
fNonHFeEmbAllTypeTrkPt(0),
fNonHFeEmbTrkPt(0),
fNonHFeEmbWeightTrkPt(0),
fPi0eEmbWeightTrkPt(0),
fEtaeEmbWeightTrkPt(0),
fRecoNonHFeTrkPt(0),
fRecoNonHFeEmbTrkPt(0),
fRecoNonHFeEmbWeightTrkPt(0),
fRecoPi0eEmbWeightTrkPt(0),
fRecoEtaeEmbWeightTrkPt(0),
fNonHFePairInvmassLS(0),
fNonHFePairInvmassULS(0),
fNonHFeEmbInvmassLS(0),
fNonHFeEmbInvmassULS(0),
fNonHFeEmbWeightInvmassLS(0),
fNonHFeEmbWeightInvmassULS(0),
fPi0EmbInvmassLS(0),
fPi0EmbInvmassULS(0),
fPi0EmbWeightInvmassLS(0),
fPi0EmbWeightInvmassULS(0),
fEtaEmbInvmassLS(0),
fEtaEmbInvmassULS(0),
fEtaEmbWeightInvmassLS(0),
fEtaEmbWeightInvmassULS(0),
fRecoLSeEmbTrkPt(0),
fRecoLSeEmbWeightTrkPt(0),
fRecoPi0LSeEmbWeightTrkPt(0),
fRecoEtaLSeEmbWeightTrkPt(0),
fRecoULSeEmbTrkPt(0),
fRecoULSeEmbWeightTrkPt(0),
fRecoPi0ULSeEmbWeightTrkPt(0),
fRecoEtaULSeEmbWeightTrkPt(0),
fSparseElectron(0),
fvalueElectron(0),
fSprsPi0EtaWeightCal(0)
{
    // Constructor
    
    fvalueElectron = new Double_t[6];
    // Define input and output slots here
    // Input slot #0 works with a TChain
    DefineInput(0, TChain::Class());
    // Output slot #0 id reserved by the base class for AOD
    // Output slot #1 writes into a TH1 container
    DefineOutput(1, TList::Class());
}
//________________________________________________________________________
AliAnalysisTaskHFEBESpectraEMC::AliAnalysisTaskHFEBESpectraEMC()
: AliAnalysisTaskSE("DefaultTask_HfeEMCQA"),
fVevent(0),
fESD(0),
fAOD(0),
fMCHeader(0),
fpidResponse(0),
fEMCALGeo(0),
fFlagSparse(kFALSE),
fUseTender(kTRUE),
fTracks_tender(0),
fCaloClusters_tender(0),
fMCparticle(0),
fMCArray(0),
fMultSelection(0),
fFlagClsTypeEMC(kTRUE),
fFlagClsTypeDCAL(kTRUE),
fcentMim(0),
fcentMax(0),
fCentralityEstimator("V0M"),
fTPCnSigma(-999.0),
fTPCnSigmaMin(-1),
fTPCnSigmaMax(3),
fM02Min(0.05),
fM02Max(0.5),
fM20Min(0.0),
fM20Max(2000),
fEovPMin(0.9),
fEovPMax(1.2),
fNEle(0),
fTPCnSigmaHadMin(-10),
fTPCnSigmaHadMax(-3.5),
fInvmassCut(0.15),
fCalculateWeight(kFALSE),
fCalculateNonHFEEffi(kFALSE),
fNTotMCpart(0),
fNpureMC(0),
fNembMCpi0(0),
fNembMCeta(0),
fIsFrmEmbPi0(kFALSE),
fIsFrmEmbEta(kFALSE),
ftype(-1),
fWeight(1),
fPi0Weight(0),
fEtaWeight(0),
fOutputList(0),
fNevents(0),
fCent(0),
fMult(0),
fVtxZ(0),
fVtxX(0),
fVtxY(0),
fTrigMulti(0),
fHistClustE(0),
fHistNonLinClustE(0),
fHistClustEcent(0),
fEMCClsEtaPhi(0),
fHistClustEEG1(0),
fHistClustEEG1cent(0),
fHistClustEEG2(0),
fHistClustEEG2cent(0),
fEMCClsEtaPhiEG1(0),
fEMCClsEtaPhiEG2(0),
fHistoNCls(0),
fHistoNCells(0),
fHistoEperCell(0),
fHistoCalCell(0),
fHistoTimeEMC(0),
fNegTrkIDPt(0),
fTrkPt(0),
fTrketa(0),
fTrkphi(0),
fdEdx(0),
fTPCNpts(0),
fTPCnsig(0),
fTPCnsigMcEle(0),
fTPCnsigMcHad(0),
fTPCnsig_Pi(0),
fHistPtMatch(0),
fEMCTrkMatch(0),
fEMCTrkPt(0),
fEMCTrketa(0),
fEMCTrkphi(0),
fEMCdEdx(0),
fEMCTPCnsig(0),
fEMCTPCNpts(0),
fClsEAftMatch(0),
fNonLinClsEAftMatch(0),
fClsEtaPhiAftMatch(0),
fClsEtaPhiAftMatchEMCin(0),
fClsEtaPhiAftMatchEMCout(0),
fHistdEdxEop(0),
fHistNsigEop(0),
fHistEop(0),
fHistMcEopEle(0),
fHistMcEopHad(0),
fM20(0),
fM02(0),
fM20EovP(0),
fM02EovP(0),
fEleCanITShit(0),
fInvmassULSPt(0),
fInvmassLSPt(0),
fHFmomCorr(0),
fEMCTrkMatch_Phi(0),
fEMCTrkMatch_Eta(0),
fInclsElecPt(0),
fHadPt_AftEID(0),
fHistEop_AftEID(0),
fNElecInEvt(0),
fULSElecPt(0),
fLSElecPt(0),
fRealInclsElecPt(0),
fNonHFeTrkPt(0),
fMissingEmbEtaEleTrkPt(0),
fNonHFeEmbAllTypeTrkPt(0),
fNonHFeEmbTrkPt(0),
fNonHFeEmbWeightTrkPt(0),
fPi0eEmbWeightTrkPt(0),
fEtaeEmbWeightTrkPt(0),
fRecoNonHFeTrkPt(0),
fRecoNonHFeEmbTrkPt(0),
fRecoNonHFeEmbWeightTrkPt(0),
fRecoPi0eEmbWeightTrkPt(0),
fRecoEtaeEmbWeightTrkPt(0),
fNonHFePairInvmassLS(0),
fNonHFePairInvmassULS(0),
fNonHFeEmbInvmassLS(0),
fNonHFeEmbInvmassULS(0),
fNonHFeEmbWeightInvmassLS(0),
fNonHFeEmbWeightInvmassULS(0),
fPi0EmbInvmassLS(0),
fPi0EmbInvmassULS(0),
fPi0EmbWeightInvmassLS(0),
fPi0EmbWeightInvmassULS(0),
fEtaEmbInvmassLS(0),
fEtaEmbInvmassULS(0),
fEtaEmbWeightInvmassLS(0),
fEtaEmbWeightInvmassULS(0),
fRecoLSeEmbTrkPt(0),
fRecoLSeEmbWeightTrkPt(0),
fRecoPi0LSeEmbWeightTrkPt(0),
fRecoEtaLSeEmbWeightTrkPt(0),
fRecoULSeEmbTrkPt(0),
fRecoULSeEmbWeightTrkPt(0),
fRecoPi0ULSeEmbWeightTrkPt(0),
fRecoEtaULSeEmbWeightTrkPt(0),
fSparseElectron(0),
fvalueElectron(0),
fSprsPi0EtaWeightCal(0)
{
    //Default constructor
    
    fvalueElectron = new Double_t[6];
    // Define input and output slots here
    // Input slot #0 works with a TChain
    DefineInput(0, TChain::Class());
    // Output slot #0 id reserved by the base class for AOD
    // Output slot #1 writes into a TH1 container
    // DefineOutput(1, TH1I::Class());
    DefineOutput(1, TList::Class());
    //DefineOutput(3, TTree::Class());
}
//________________________________________________________________________
AliAnalysisTaskHFEBESpectraEMC::~AliAnalysisTaskHFEBESpectraEMC()
{
    //Destructor
    delete fOutputList;
    delete fTracks_tender;
    delete fCaloClusters_tender;
    delete fSparseElectron;
    delete []fvalueElectron;
    delete fSprsPi0EtaWeightCal;
}
//________________________________________________________________________
void AliAnalysisTaskHFEBESpectraEMC::UserCreateOutputObjects()
{
    // Create histograms
    // Called once
    AliDebug(3, "Creating Output Objects");
    
    Double_t pi = TMath::Pi();
    fPi0Weight = new TF1("fPi0Weight","[0] / TMath::Power(TMath::Exp(-[1]*x - [2]*x*x) + x/[3], [4])");
    fEtaWeight = new TF1("fEtaWeight","[0] / TMath::Power(TMath::Exp(-[1]*x - [2]*x*x) + x/[3], [4])");
    fPi0Weight->SetParameters(3.72558e+02,-4.25395e-02,2.18681e-03,1.59658e+00,5.60917e+00);
    fEtaWeight->SetParameters(3.34121e+02,-7.09185e-02,2.04493e-03,1.59842e+00,5.43861e+00);
    
    /////////////////////////////////////////////////
    //Automatic determination of the analysis mode//
    ////////////////////////////////////////////////
    AliVEventHandler *inputHandler = dynamic_cast<AliVEventHandler *>(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler());
    if(!TString(inputHandler->IsA()->GetName()).CompareTo("AliAODInputHandler")){
        SetAODAnalysis();
    } else {
        SetESDAnalysis();
    }
    printf("Analysis Mode: %s Analysis\n", IsAODanalysis() ? "AOD" : "ESD");
    
    ////////////////
    //Output list//
    ///////////////
    fOutputList = new TList();
    fOutputList->SetOwner();
    
    fNevents = new TH1F("fNevents","No of events",3,-0.5,2.5);
    fOutputList->Add(fNevents);
    fNevents->GetYaxis()->SetTitle("counts");
    fNevents->GetXaxis()->SetBinLabel(1,"All");
    fNevents->GetXaxis()->SetBinLabel(2,"With >2 Trks");
    fNevents->GetXaxis()->SetBinLabel(3,"Vtx_{z}<10cm");
    
    fCent = new TH1F("fCent","Centrality",100,0,100);
    fOutputList->Add(fCent);
    
    fMult = new TH2F("fMult","Track multiplicity",100,0,100,20000,0,20000);
    fOutputList->Add(fMult);
    
    fVtxZ = new TH1F("fVtxZ","Z vertex position;Vtx_{z};counts",500,-25,25);
    fOutputList->Add(fVtxZ);
    
    fVtxY = new TH1F("fVtxY","Y vertex position;Vtx_{y};counts",500,-25,25);
    fOutputList->Add(fVtxY);
    
    fVtxX = new TH1F("fVtxX","X vertex position;Vtx_{x};counts",500,-25,25);
    fOutputList->Add(fVtxX);
    
    fHistClustE = new TH1F("fHistClustE", "EMCAL cluster energy distribution; Cluster E;counts", 500, 0.0, 50.0);
    fOutputList->Add(fHistClustE);
    
    fHistNonLinClustE = new TH1F("fHistNonLinClustE", "Nonlinearity corrected EMCAL cluster energy distribution; Cluster E;counts", 500, 0.0, 50.0);
    fOutputList->Add(fHistNonLinClustE);
    
    fHistClustEcent = new TH2F("fHistClustEcent", "EMCAL cluster energy distribution vs. centrality; centrality; Cluster E", 100,0,100,500, 0.0, 50.0);
    fOutputList->Add(fHistClustEcent);
    
    fEMCClsEtaPhi = new TH2F("fEMCClsEtaPhi","EMCAL cluster #eta and #phi distribution;#eta;#phi",100,-0.9,0.9,200,0,6.3);
    fOutputList->Add(fEMCClsEtaPhi);
    
    fHistoNCls = new TH1F("fHistoNCls","No of EMCAL cluster in the event;N^{EMC}_{cls};counts",150,0,150);
    fOutputList->Add(fHistoNCls);
    
    fHistoNCells = new TH2F("fHistoNCells","No of EMCAL cells in a cluster;Cluster E;N^{EMC}_{cells}",500,0,50,30,0,30);
    fOutputList->Add(fHistoNCells);
    
    fHistoEperCell = new TH2F("fHistoEperCell","E/cell;Cluster E;E/cell",400,0,40,300,0,30);
    fOutputList->Add(fHistoEperCell);
    
    fHistoCalCell = new TH2F("fHistoCalCell","Energy of EMCAL cells;cell ID;E (GeV)",15000,-0.5,14999.5,150,0,30);
    fOutputList->Add(fHistoCalCell);
    
    fHistoTimeEMC = new TH2F("fHistoTimeEMC","EMCAL Time;E (GeV); t(ns)",500,0,50,1800,-900,900);
    fOutputList->Add(fHistoTimeEMC);
    
    fNegTrkIDPt = new TH1F("fNegTrkIDPt", "p_{T} distribution of tracks with negative track id;p_{T} (GeV/c);counts", 500, 0.0, 50.0);
    fOutputList->Add(fNegTrkIDPt);
    
    fTrkPt = new TH1F("fTrkPt","p_{T} distribution of all tracks;p_{T} (GeV/c);counts",500,0,100);
    fOutputList->Add(fTrkPt);
    
    fTrketa = new TH1F("fTrketa","All Track #eta distribution;#eta;counts",100,-1.5,1.5);
    fOutputList->Add(fTrketa);
    
    fTrkphi = new TH1F("fTrkphi","All Track #phi distribution;#phi;counts",100,0,6.3);
    fOutputList->Add(fTrkphi);
    
    fdEdx = new TH2F("fdEdx","All Track dE/dx distribution;p (GeV/c);dE/dx",500,0,50,500,0,160);
    fOutputList->Add(fdEdx);
    
    fTPCNpts = new TH2F("fTPCNpts","All track TPC Npoints used for dE/dx calculation;p (GeV/c);N points",200,0,20,200,0.,200.);
    fOutputList->Add(fTPCNpts);
    
    fTPCnsig = new TH2F("fTPCnsig","All Track TPC Nsigma distribution;p (GeV/c);#sigma_{TPC-dE/dx}",500,0,50,200,-10,10);
    fOutputList->Add(fTPCnsig);
    
    fTPCnsigMcEle = new TH2F("fTPCnsigMcEle","All Track TPC Nsigma distribution (MC electron);p (GeV/c);#sigma_{TPC-dE/dx}",500,0,50,200,-10,10);
    fOutputList->Add(fTPCnsigMcEle);
    
    fTPCnsigMcHad = new TH2F("fTPCnsigMcHad","All Track TPC Nsigma distribution (MC hadron);p (GeV/c);#sigma_{TPC-dE/dx}",500,0,50,200,-10,10);
    fOutputList->Add(fTPCnsigMcHad);
    
    fTPCnsig_Pi = new TH2F("fTPCnsig_Pi","All Track TPC Nsigma distribution wrt pion;p (GeV/c);#sigma_{TPC-dE/dx}",500,0,50,200,-10,10);
    fOutputList->Add(fTPCnsig_Pi);
    
    fHistPtMatch = new TH1F("fHistPtMatch", "p_{T} distribution of tracks matched to EMCAL;p_{T} (GeV/c);counts",500, 0.0, 50.0);
    fOutputList->Add(fHistPtMatch);
    
    fEMCTrkMatch = new TH2F("fEMCTrkMatch","Distance of EMCAL cluster to its closest track;#phi;z",100,-0.3,0.3,100,-0.3,0.3);
    fOutputList->Add(fEMCTrkMatch);
    
    fEMCTrkMatch_Phi = new TH2F("fEMCTrkMatch_Phi","Distance of EMCAL cluster to its closest track in #Delta#phi vs p_{T};p_{T};#Delta#phi",500,0,50.0,100,-0.3,0.3);
    fOutputList->Add(fEMCTrkMatch_Phi);
    
    fEMCTrkMatch_Eta = new TH2F("fEMCTrkMatch_Eta","Distance of EMCAL cluster to its closest track in #Delta#eta vs p_{T};p_{T};#Delta#eta",500,0,50.0,100,-0.3,0.3);
    fOutputList->Add(fEMCTrkMatch_Eta);
    
    fEMCTrkPt = new TH1F("fEMCTrkPt","p_{T} distribution of tracks with EMCAL cluster;p_{T} (GeV/c);counts",500,0,50);
    fOutputList->Add(fEMCTrkPt);
    
    fEMCTrketa = new TH1F("fEMCTrketa","#eta distribution of tracks matched to EMCAL;#eta;counts",60,-1.5,1.5);
    fOutputList->Add(fEMCTrketa);
    
    fEMCTrkphi = new TH1F("fEMCTrkphi","#phi distribution of tracks matched to EMCAL;#phi;counts",100,0,6.3);
    fOutputList->Add(fEMCTrkphi);
    
    fEMCdEdx = new TH2F("fEMCdEdx","dE/dx distribution of tracks matched to EMCAL;p (GeV/c);dE/dx",200,0,20,500,0,160);
    fOutputList->Add(fEMCdEdx);
    
    fEMCTPCnsig = new TH2F("fEMCTPCnsig","TPC Nsigma distribution of tracks matched to EMCAL;p (GeV/c);#sigma_{TPC-dE/dx}",500,0,50,200,-10,10);
    fOutputList->Add(fEMCTPCnsig);
    
    fEMCTPCNpts = new TH2F("fEMCTPCNpts","TPC Npoints used for dE/dx for tracks matched to EMCAL;p (GeV/c);N points",200,0,20,200,0.,200.);
    fOutputList->Add(fEMCTPCNpts);
    
    fClsEAftMatch = new TH1F("fClsEAftMatch", "EMCAL cluster energy distribution after track matching; Cluster E;counts", 500, 0.0, 50.0);
    fOutputList->Add(fClsEAftMatch);
    
    fNonLinClsEAftMatch = new TH1F("fNonLinClsEAftMatch", "Nonlinearity corrected EMCAL cluster energy distribution after track matching; Cluster E;counts", 500, 0.0, 50.0);
    fOutputList->Add(fNonLinClsEAftMatch);
    
    fClsEtaPhiAftMatch = new TH2F("fClsEtaPhiAftMatch","EMCAL cluster #eta and #phi distribution after track matching;#eta;#phi",100,-0.9,0.9,200,0,6.3);
    fOutputList->Add(fClsEtaPhiAftMatch);
    
    fClsEtaPhiAftMatchEMCin = new TH2F("fClsEtaPhiAftMatchEMCin","EMCAL cluster #eta and #phi distribution after track matching inside EMC #phi acceptence;#eta;#phi",100,-0.9,0.9,200,0,6.3);
    fOutputList->Add(fClsEtaPhiAftMatchEMCin);
    
    fClsEtaPhiAftMatchEMCout = new TH2F("fClsEtaPhiAftMatchEMCout","EMCAL cluster #eta and #phi distribution after track matching outside EMC #phi acceptence;#eta;#phi",100,-0.9,0.9,200,0,6.3);
    fOutputList->Add(fClsEtaPhiAftMatchEMCout);
    
    fHistEop = new TH2F("fHistEop", "E/p distribution;p_{T} (GeV/c);E/p", 200,0,20,40, 0.0, 2.0);
    fOutputList->Add(fHistEop);
    
    fHistMcEopEle = new TH2F("fHistMcEopEle", "E/p distribution (MC electron);p_{T} (GeV/c);E/p", 200,0,20,40, 0.0, 2.0);
    fOutputList->Add(fHistMcEopEle);
    
    fHistMcEopHad = new TH2F("fHistMcEopHad", "E/p distribution (MC hadron);p_{T} (GeV/c);E/p", 200,0,20,40, 0.0, 2.0);
    fOutputList->Add(fHistMcEopHad);
    
    fHistdEdxEop = new TH2F("fHistdEdxEop", "E/p vs dE/dx;E/p;dE/dx", 40, 0.0, 2.0, 500,0,160);
    fOutputList->Add(fHistdEdxEop);
    
    fHistNsigEop = new TH2F ("fHistNsigEop", "E/p vs TPC nsig",40, 0.0, 2.0, 200, -10,10);
    fOutputList->Add(fHistNsigEop);
    
    fM20 = new TH2F ("fM20","M20 vs pt distribution",500,0,50,200,0,2);
    fOutputList->Add(fM20);
    
    fM02 = new TH2F ("fM02","M02 vs pt distribution",500,0,50,200,0,2);
    fOutputList->Add(fM02);
    
    fM20EovP = new TH2F ("fM20EovP","M20 vs E/p distribution;E/p;M20",40,0,2,200,0,2);
    fOutputList->Add(fM20EovP);
    
    fM02EovP = new TH2F ("fM02EovP","M02 vs E/p distribution;E/p;M02",40,0,2,200,0,2);
    fOutputList->Add(fM02EovP);
    
    fEleCanITShit = new TH1F("fEleCanITShit","ITS hit map;ITS layer;counts",7,-0.5,6.5);
    fOutputList->Add(fEleCanITShit);
    
    fInclsElecPt = new TH1F("fInclsElecPt","p_{T} distribution of inclusive electrons;p_{T} (GeV/c);counts",500,0,50);
    fOutputList->Add(fInclsElecPt);
    
    fHadPt_AftEID = new TH1F("fHadPt_AftEID","p_{T} distribution of hadrons after Eid cuts;p_{T} (GeV/c);counts",500,0,50);
    fOutputList->Add(fHadPt_AftEID);
    
    fHistEop_AftEID = new TH2F("fHistEop_AftEID", "E/p distribution after nsig, SS cuts;p_{T} (GeV/c);E/p", 200,0,20,60, 0.0, 3.0);
    fOutputList->Add(fHistEop_AftEID);
    
    fNElecInEvt = new TH1F("fNElecInEvt","No of electrons in the event; N^{ele};counts",20,-0.5,19.5);
    fOutputList->Add(fNElecInEvt);
    
    fInvmassLSPt = new TH2F("fInvmassLSPt", "Invmass of LS (e,e) for pt^{e}>1; p_{T}(GeV/c); mass(GeV/c^2); counts;", 500,0,50,500,0,1.0);
    fOutputList->Add(fInvmassLSPt);
    
    fInvmassULSPt = new TH2F("fInvmassULSPt", "Invmass of ULS (e,e) for pt^{e}>1; p_{T}(GeV/c); mass(GeV/c^2); counts;", 500,0,50,500,0,1.0);
    fOutputList->Add(fInvmassULSPt);
    
    fULSElecPt  = new TH1F("fULSElecPt","p_{T} distribution of ULS electrons;p_{T} (GeV/c);counts",500,0,50);
    fOutputList->Add(fULSElecPt);
    
    fLSElecPt= new TH1F("fLSElecPt","p_{T} distribution of LS electrons;p_{T} (GeV/c);counts",500,0,50);
    fOutputList->Add(fLSElecPt);
    
    if(fFlagSparse){
        Int_t bins[6]=      {280, 160, 40, 200, 200, 20}; //pT;nSigma;eop;m20;m02;iSM
        Double_t xmin[6]={2,  -8,   0,   0,   0, 0};
        Double_t xmax[6]={30,   8,   2,   2,   2, 20};
        fSparseElectron = new THnSparseD ("Electron","Electron;pT;nSigma;eop;m20;m02;iSM;",6,bins,xmin,xmax);
        fOutputList->Add(fSparseElectron);
    }
    
    if(fCalculateWeight){
        Int_t bin[4] = {500,3,2,7}; //pT, PDG, HijingOrNot, pi0etaType
        Double_t xmin[4] = {0,0,0,-1};
        Double_t xmax[4] = {50,3,2,6};
    
        fSprsPi0EtaWeightCal = new THnSparseD("fSprsPi0EtaWeightCal","Sparse to calculate #pi^{0} and #eta weight;p_{T};PDG ID;HijingOrNot;pi0etaType;",4,bin,xmin,xmax);
        fOutputList->Add(fSprsPi0EtaWeightCal);
    }
    
    if(fCalculateNonHFEEffi){
        fRealInclsElecPt = new TH1F("fRealInclsElecPt","p_{T} distribution of MC tagged inclusive electrons;p_{T} (GeV/c);counts",250,0,50);
        fOutputList->Add(fRealInclsElecPt);
        
        fNonHFeTrkPt = new TH1F("fNonHFeTrkPt","Non-HF electrons from all generators;p_{T} (GeV/c);counts",250,0,50);
        fNonHFeTrkPt->Sumw2();
        fOutputList->Add(fNonHFeTrkPt);
        
        fMissingEmbEtaEleTrkPt = new TH1F("fMissingEmbEtaEleTrkPt","Missing electrons from embedded #eta  + No mom ;p_{T} (GeV/c);counts",250,0,50);
        fMissingEmbEtaEleTrkPt->Sumw2();
        fOutputList->Add(fMissingEmbEtaEleTrkPt);
        
        fNonHFeEmbAllTypeTrkPt = new TH1F("fNonHFeEmbAllTypeTrkPt","Non-HF electrons from embedded #pi^{0} and #eta of all type;p_{T} (GeV/c);counts",250,0,50);
        fNonHFeEmbAllTypeTrkPt->Sumw2();
        fOutputList->Add(fNonHFeEmbAllTypeTrkPt);
        
        fNonHFeEmbTrkPt = new TH1F("fNonHFeEmbTrkPt","Non-HF electrons from embedded #pi^{0} and #eta + No mom;p_{T} (GeV/c);counts",250,0,50);
        fNonHFeEmbTrkPt->Sumw2();
        fOutputList->Add(fNonHFeEmbTrkPt);
        
        fNonHFeEmbWeightTrkPt = new TH1F("fNonHFeEmbWeightTrkPt","Non-HF electrons from embedded #pi^{0} and #eta + No mom with weight + No mom;p_{T} (GeV/c);counts",250,0,50);
        fNonHFeEmbWeightTrkPt->Sumw2();
        fOutputList->Add(fNonHFeEmbWeightTrkPt);
        
        fPi0eEmbWeightTrkPt = new TH1F("fPi0eEmbWeightTrkPt","Non-HF electrons from embedded #pi^{0} + No mom with weight;p_{T} (GeV/c);counts",250,0,50);
        fPi0eEmbWeightTrkPt->Sumw2();
        fOutputList->Add(fPi0eEmbWeightTrkPt);
        
        fEtaeEmbWeightTrkPt = new TH1F("fEtaeEmbWeightTrkPt","Non-HF electrons from embedded #eta  + No mom with weight;p_{T} (GeV/c);counts",250,0,50);
        fEtaeEmbWeightTrkPt->Sumw2();
        fOutputList->Add(fEtaeEmbWeightTrkPt);
        
        fRecoNonHFeTrkPt = new TH1F("fRecoNonHFeTrkPt"," Reco Non-HF electrons from all generators;p_{T} (GeV/c);counts",250,0,50);
        fRecoNonHFeTrkPt->Sumw2();
        fOutputList->Add(fRecoNonHFeTrkPt);

        fRecoNonHFeEmbTrkPt = new TH1F("fRecoNonHFeEmbTrkPt","Reco Non-HF electrons from embedded #pi^{0} and #eta + No mom;p_{T} (GeV/c);counts",250,0,50);
        fRecoNonHFeEmbTrkPt->Sumw2();
        fOutputList->Add(fRecoNonHFeEmbTrkPt);
        
        fRecoNonHFeEmbWeightTrkPt = new TH1F("fRecoNonHFeEmbWeightTrkPt","Reco Non-HF electrons from embedded #pi^{0} and #eta  + No mom with weight;p_{T} (GeV/c);counts",250,0,50);
        fRecoNonHFeEmbWeightTrkPt->Sumw2();
        fOutputList->Add(fRecoNonHFeEmbWeightTrkPt);
        
        fRecoPi0eEmbWeightTrkPt = new TH1F("fRecoPi0eEmbWeightTrkPt","Reco Non-HF electrons from embedded #pi^{0}  + No mom with weight;p_{T} (GeV/c);counts",250,0,50);
        fRecoPi0eEmbWeightTrkPt->Sumw2();
        fOutputList->Add(fRecoPi0eEmbWeightTrkPt);
        
        fRecoEtaeEmbWeightTrkPt = new TH1F("fRecoEtaeEmbWeightTrkPt","Reco Non-HF electrons from embedded #eta  + No mom with weight;p_{T} (GeV/c);counts",250,0,50);
        fRecoEtaeEmbWeightTrkPt->Sumw2();
        fOutputList->Add(fRecoEtaeEmbWeightTrkPt);
        
        fNonHFePairInvmassLS = new TH1F("fNonHFePairInvmassLS", "Inv mass of LS (e,e) if both e- are Non-HFE; mass(GeV/c^2); counts;",  50,0,0.5);
        fOutputList->Add(fNonHFePairInvmassLS);
        
        fNonHFePairInvmassULS = new TH1F("fNonHFePairInvmassULS", "Inv mass of ULS (e,e) if both e- are Non-HFE; mass(GeV/c^2); counts;",  50,0,0.5);
        fOutputList->Add(fNonHFePairInvmassULS);
        
        fNonHFeEmbInvmassLS = new TH1F("fNonHFeEmbInvmassLS", "Inv mass of LS (e,e) for Non-HFE from embedded #pi^{0} and #eta; mass(GeV/c^2); counts;",  50,0,0.5);
        fOutputList->Add(fNonHFeEmbInvmassLS);
        
        fNonHFeEmbInvmassULS = new TH1F("fNonHFeEmbInvmassULS", "Inv mass of ULS (e,e) for Non-HFE from embedded #pi^{0} and #eta; mass(GeV/c^2); counts;",  50,0,0.5);
        fOutputList->Add(fNonHFeEmbInvmassULS);
        
        fNonHFeEmbWeightInvmassLS = new TH1F("fNonHFeEmbWeightInvmassLS", "Inv mass of LS (e,e) for Non-HFE from embedded #pi^{0} and #eta with weight; mass(GeV/c^2); counts;",  50,0,0.5);
        fOutputList->Add(fNonHFeEmbWeightInvmassLS);
        
        fNonHFeEmbWeightInvmassULS = new TH1F("fNonHFeEmbWeightInvmassULS", "Inv mass of ULS (e,e) for Non-HFE from embedded #pi^{0} and #eta with weight; mass(GeV/c^2); counts;",  50,0,0.5);
        fOutputList->Add(fNonHFeEmbWeightInvmassULS);
        
        fPi0EmbInvmassLS = new TH1F("fPi0EmbInvmassLS", "Inv mass of LS (e,e) for ele from embedded #pi^{0}; mass(GeV/c^2); counts;",  50,0,0.5);
        fOutputList->Add(fPi0EmbInvmassLS);
        
        fPi0EmbInvmassULS  = new TH1F("fPi0EmbInvmassULS", "Inv mass of ULS (e,e) for ele from embedded #pi^{0}; mass(GeV/c^2); counts;",  50,0,0.5);
        fOutputList->Add(fPi0EmbInvmassULS);
        
        fPi0EmbWeightInvmassLS = new TH1F("fPi0EmbWeightInvmassLS", "Inv mass of LS (e,e) for ele from embedded #pi^{0} with weight; mass(GeV/c^2); counts;",  50,0,0.5);
        fOutputList->Add(fPi0EmbWeightInvmassLS);
        
        fPi0EmbWeightInvmassULS  = new TH1F("fPi0EmbWeightInvmassULS", "Inv mass of ULS (e,e) for ele from embedded #pi^{0} with weight; mass(GeV/c^2); counts;",  50,0,0.5);
        fOutputList->Add(fPi0EmbWeightInvmassULS);
        
        fEtaEmbInvmassLS = new TH1F("fEtaEmbInvmassLS", "Inv mass of LS (e,e) for ele from embedded #eta; mass(GeV/c^2); counts;",  50,0,0.5);
        fOutputList->Add(fEtaEmbInvmassLS);
        
        fEtaEmbInvmassULS = new TH1F("fEtaEmbInvmassULS", "Inv mass of ULS (e,e) for ele from embedded #eta; mass(GeV/c^2); counts;",  50,0,0.5);
        fOutputList->Add(fEtaEmbInvmassULS);
        
        fEtaEmbWeightInvmassLS = new TH1F("fEtaEmbWeightInvmassLS", "Inv mass of LS (e,e) for ele from embedded #eta with weight; mass(GeV/c^2); counts;",  50,0,0.5);
        fOutputList->Add(fEtaEmbWeightInvmassLS);
        
        fEtaEmbWeightInvmassULS  = new TH1F("fEtaEmbWeightInvmassULS", "Inv mass of ULS (e,e) for ele from embedded #eta with weight; mass(GeV/c^2); counts;",  50,0,0.5);
        fOutputList->Add(fEtaEmbWeightInvmassULS);
        
        fRecoLSeEmbTrkPt  = new TH1F("fRecoLSeEmbTrkPt","Reco LS electrons from embedded #pi^{0} and #eta + No mom;p_{T} (GeV/c);counts",250,0,50);
        fRecoLSeEmbTrkPt->Sumw2();
        fOutputList->Add(fRecoLSeEmbTrkPt);
        
        fRecoLSeEmbWeightTrkPt = new TH1F("fRecoLSeEmbWeightTrkPt","Reco LS electrons from embedded #pi^{0} and #eta  + No mom with weight;p_{T} (GeV/c);counts",250,0,50);
        fRecoLSeEmbWeightTrkPt->Sumw2();
        fOutputList->Add(fRecoLSeEmbWeightTrkPt);
        
        fRecoPi0LSeEmbWeightTrkPt = new TH1F("fRecoPi0LSeEmbWeightTrkPt","Reco LS electrons from embedded #pi^{0}  + No mom with weight;p_{T} (GeV/c);counts",250,0,50);
        fRecoPi0LSeEmbWeightTrkPt->Sumw2();
        fOutputList->Add(fRecoPi0LSeEmbWeightTrkPt);
        
        fRecoEtaLSeEmbWeightTrkPt  = new TH1F("fRecoEtaLSeEmbWeightTrkPt","Reco LS electrons from embedded #eta  + No mom with weight;p_{T} (GeV/c);counts",250,0,50);
        fRecoEtaLSeEmbWeightTrkPt->Sumw2();
        fOutputList->Add(fRecoEtaLSeEmbWeightTrkPt);
        
        fRecoULSeEmbTrkPt = new TH1F("fRecoULSeEmbTrkPt","Reco ULS electrons from embedded #pi^{0} and #eta + No mom;p_{T} (GeV/c);counts",250,0,50);
        fRecoULSeEmbTrkPt->Sumw2();
        fOutputList->Add(fRecoULSeEmbTrkPt);
        
        fRecoULSeEmbWeightTrkPt = new TH1F("fRecoULSeEmbWeightTrkPt","Reco ULS electrons from embedded #pi^{0} and #eta  + No mom with weight;p_{T} (GeV/c);counts",250,0,50);
        fRecoULSeEmbWeightTrkPt->Sumw2();
        fOutputList->Add(fRecoULSeEmbWeightTrkPt);
        
        fRecoPi0ULSeEmbWeightTrkPt = new TH1F("fRecoPi0ULSeEmbWeightTrkPt","Reco ULS electrons from embedded #pi^{0}  + No mom with weight;p_{T} (GeV/c);counts",250,0,50);
        fRecoPi0ULSeEmbWeightTrkPt->Sumw2();
        fOutputList->Add(fRecoPi0ULSeEmbWeightTrkPt);
        
        fRecoEtaULSeEmbWeightTrkPt = new TH1F("fRecoEtaULSeEmbWeightTrkPt","Reco ULS electrons from embedded #eta  + No mom with weight;p_{T} (GeV/c);counts",250,0,50);
        fRecoEtaULSeEmbWeightTrkPt->Sumw2();
        fOutputList->Add(fRecoEtaULSeEmbWeightTrkPt);
    }
    
    PostData(1,fOutputList);
}

//________________________________________________________________________
void AliAnalysisTaskHFEBESpectraEMC::UserExec(Option_t *)
{
    // Main loop
    // Called for each event
    // Post output data.
    
    UInt_t evSelMask=((AliInputEventHandler*)(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler()))->IsEventSelected();
    
    fVevent = dynamic_cast<AliVEvent*>(InputEvent());
    if (!fVevent) {
        printf("ERROR: fVEvent not available\n");
        return;
    }
    
    fESD = dynamic_cast<AliESDEvent*>(InputEvent());
    if (fESD) {
        //   printf("fESD available\n");
        //return;
    }
    
    //////////////
    //if Tender //
    //////////////
    if(fUseTender){
        //new branches with calibrated tracks and clusters
        if(IsAODanalysis()){ fTracks_tender = dynamic_cast<TClonesArray*>(InputEvent()->FindListObject("tracks"));
        fCaloClusters_tender = dynamic_cast<TClonesArray*>(InputEvent()->FindListObject("caloClusters"));
        }
    }
    
    fAOD = dynamic_cast<AliAODEvent*>(InputEvent());
    fMCArray = dynamic_cast<TClonesArray*>(fAOD->FindListObject(AliAODMCParticle::StdBranchName()));
    fMCHeader = dynamic_cast<AliAODMCHeader*>(fAOD->GetList()->FindObject(AliAODMCHeader::StdBranchName()));
    
    if(fMCHeader){
        ////////////////////////////////
        //Get number of Gen particles //
        ////////////////////////////////
        GetNMCPartProduced();
        
        /////////////////////////////////
        //Calculate Pi0 and Eta weight //
        /////////////////////////////////
        if(fCalculateWeight) GetPi0EtaWeight(fSprsPi0EtaWeightCal);
    
    }
    
    ///////////////////
    //PID initialised//
    ///////////////////
    fpidResponse = fInputHandler->GetPIDResponse();
    
    ///////////////////
    // centrality
    /////////////////////
    
    Double_t centrality = -1;
    AliCentrality *fCentrality = (AliCentrality*)fAOD->GetCentrality();
    //centrality = fCentrality->GetCentralityPercentile("V0M");
    
    //Double_t centrality = -1;
    if(fAOD)fMultSelection = (AliMultSelection * ) fAOD->FindListObject("MultSelection");
    if( !fMultSelection) {
        //If you get this warning (and lPercentiles 300) please check that the AliMultSelectionTask actually ran (before your task)
        //AliWarning("AliMultSelection object not found!");
        centrality = fCentrality->GetCentralityPercentile(fCentralityEstimator.Data());
    }else{
        //lPercentile = fMultSelection->GetMultiplicityPercentile("V0M");
        centrality = fMultSelection->GetMultiplicityPercentile(fCentralityEstimator.Data(), false);
    }
    
    if(fcentMim>-0.5)
    {
        if(centrality < fcentMim || centrality > fcentMax)return;
    }
    
    ////////////////
    //Event vertex//
    ////////////////
    Int_t ntracks = -999;
    if(!fUseTender)ntracks = fVevent->GetNumberOfTracks();
    if(fUseTender) ntracks = fTracks_tender->GetEntries();
    //if(ntracks < 1) printf("There are %d tracks in this event\n",ntracks);
    fMult->Fill(centrality,ntracks);
    
    fNevents->Fill(0); //all events
    Double_t Zvertex = -100, Xvertex = -100, Yvertex = -100;
    const AliVVertex *pVtx = fVevent->GetPrimaryVertex();
    Double_t NcontV = pVtx->GetNContributors();
    if(NcontV<2)return;
    fNevents->Fill(1); //events with 2 tracks
    
    Zvertex = pVtx->GetZ();
    Yvertex = pVtx->GetY();
    Xvertex = pVtx->GetX();
    fVtxZ->Fill(Zvertex);
    fVtxX->Fill(Xvertex);
    fVtxY->Fill(Yvertex);
    
    ////////////////////
    //event selection///
    ////////////////////
    if(TMath::Abs(Zvertex)>10.0)return;
    fNevents->Fill(2); //events after z vtx cut
    fCent->Fill(centrality); //centrality dist.
    
    /////////////////////////////
    //EMCAL cluster information//
    /////////////////////////////
    GetEMCalClusterInfo();
    
    ////////////////////////////////
    //Look for kink mother for AOD//
    ////////////////////////////////
    Int_t numberofvertices = 100;
    if(fAOD) numberofvertices = fAOD->GetNumberOfVertices();
    Double_t listofmotherkink[numberofvertices];
    Int_t numberofmotherkink = 0;
    if(IsAODanalysis())
    {
        for(Int_t ivertex=0; ivertex < numberofvertices; ivertex++) {
            AliAODVertex *aodvertex = fAOD->GetVertex(ivertex);
            if(!aodvertex) continue;
            if(aodvertex->GetType()==AliAODVertex::kKink) {
                AliAODTrack *mother = (AliAODTrack *) aodvertex->GetParent();
                if(!mother) continue;
                Int_t idmother = mother->GetID();
                listofmotherkink[numberofmotherkink] = idmother;
                numberofmotherkink++;
            }
        }
    } //+++
    
    ///////////////
    //Track loop///
    ///////////////
    
    fNEle = 0;
    
    for (Int_t iTracks = 0; iTracks < ntracks; iTracks++) {
        
        AliVParticle* Vtrack = 0x0;
        if(!fUseTender) Vtrack  = fVevent->GetTrack(iTracks);
        if(fUseTender) Vtrack = dynamic_cast<AliVTrack*>(fTracks_tender->At(iTracks));
        
        if (!Vtrack) {
            printf("ERROR: Could not receive track %d\n", iTracks);
            continue;
        }
        AliVTrack *track = dynamic_cast<AliVTrack*>(Vtrack);
        AliESDtrack *etrack = dynamic_cast<AliESDtrack*>(Vtrack);
        AliAODTrack *atrack = dynamic_cast<AliAODTrack*>(Vtrack);
        
        ///////////////////////
        // Get MC information//
        ///////////////////////
        Int_t ilabel = track->GetLabel();
        Int_t pdg = -999;
        Int_t pidM = -1;
        Double_t pid_ele = 0.0;
        
        ////////////////////
        //Apply track cuts//
        ////////////////////
        if(fAOD)
                if(!atrack->TestFilterMask(AliAODTrack::kTrkGlobalNoDCA)) continue; //mimimum cuts
        
        //reject kink
        if(IsAODanalysis()){
            Bool_t kinkmotherpass = kTRUE;
            for(Int_t kinkmother = 0; kinkmother < numberofmotherkink; kinkmother++) {
                if(track->GetID() == listofmotherkink[kinkmother]) {
                    kinkmotherpass = kFALSE;
                    continue;
                }
            }
            if(!kinkmotherpass) continue;
        }
        else{
            if(etrack->GetKinkIndex(0) != 0) continue;
        }
        
        //other cuts
        Double_t d0z0[2]={-999,-999}, cov[3];
        Double_t DCAxyCut = 0.25, DCAzCut = 1;
        
        if(atrack->GetTPCNcls() < 80) continue;
        if(atrack->GetITSNcls() < 3) continue;
        if((!(atrack->GetStatus()&AliESDtrack::kITSrefit)|| (!(atrack->GetStatus()&AliESDtrack::kTPCrefit)))) continue;
        if(!(atrack->HasPointOnITSLayer(0) || atrack->HasPointOnITSLayer(1))) continue;
            
        if(atrack->PropagateToDCA(pVtx, fVevent->GetMagneticField(), 20., d0z0, cov))
        if(TMath::Abs(d0z0[0]) > DCAxyCut || TMath::Abs(d0z0[1]) > DCAzCut) continue;
        
        ////////////////////
        //Track properties//
        ///////////////////
        Double_t dEdx =-999, fTPCnSigma_Pi=-999;
        Double_t TrkPhi=-999, TrkPt=-999, TrkEta=-999, TrkP = -999;
        dEdx = track->GetTPCsignal();
        fTPCnSigma = fpidResponse->NumberOfSigmasTPC(track, AliPID::kElectron);
        fTPCnSigma_Pi = fpidResponse->NumberOfSigmasTPC(track, AliPID::kPion);
        TrkPhi = track->Phi();
        TrkPt = track->Pt();
        TrkEta = track->Eta();
        TrkP = track->P();
        
        if(track->GetID()<0) fNegTrkIDPt->Fill(track->Pt());
        fTrkPt->Fill(TrkPt);
        fTrketa->Fill(TrkEta);
        fTrkphi->Fill(TrkPhi);
        fdEdx->Fill(TrkP,dEdx);
        fTPCNpts->Fill(TrkP,track->GetTPCsignalN());
        fTPCnsig->Fill(TrkP,fTPCnSigma);
        fTPCnsig_Pi->Fill(TrkP,fTPCnSigma_Pi);
        
        ///////////////////////////
        //Track matching to EMCAL//
        //////////////////////////
        if(!track->IsEMCAL()) continue;
        Int_t EMCalIndex = -1;
        EMCalIndex = track->GetEMCALcluster();
        if(EMCalIndex < 0) continue;
        fHistPtMatch->Fill(track->Pt());
        
        AliVCluster *clustMatch=0x0;
        if(!fUseTender) clustMatch = (AliVCluster*)fVevent->GetCaloCluster(EMCalIndex);
        if(fUseTender) clustMatch = dynamic_cast<AliVCluster*>(fCaloClusters_tender->At(EMCalIndex));
        
        Short_t NcellsInCluster = clustMatch->GetNCells();
        int icell=-1, iSM = -1;
        for(int icl=0; icl<NcellsInCluster; icl++)
        {
            if(icl==0) icell = clustMatch->GetCellAbsId(icl); //first cell = seed cell?
            if(fEMCALGeo)
                iSM = fEMCALGeo->GetSuperModuleNumber(icell);
        }
        
        Double_t emcphi = -999, emceta=-999;
        Bool_t fClsTypeEMC = kFALSE, fClsTypeDCAL = kFALSE;
        if(clustMatch && clustMatch->IsEMCAL())
        {
            // fEMCTrkMatch->Fill(clustMatch->GetTrackDx(),clustMatch->GetTrackDz());
            //if(TMath::Abs(clustMatch->GetTrackDx())>0.05 || TMath::Abs(clustMatch->GetTrackDz())>0.05) continue;
            
            Double_t fPhiDiff = -999, fEtaDiff = -999;
            GetTrkClsEtaPhiDiff(track, clustMatch, fPhiDiff, fEtaDiff);
            fEMCTrkMatch->Fill(fPhiDiff,fEtaDiff);
            fEMCTrkMatch_Phi->Fill(track->Pt(),fPhiDiff);
            fEMCTrkMatch_Eta->Fill(track->Pt(),fEtaDiff);
            
            if(TMath::Abs(fPhiDiff) > 0.05 || TMath::Abs(fEtaDiff)> 0.05) continue;
            
            /////////////////////////////////
            //Select EMCAL or DCAL clusters//
            /////////////////////////////////
            Float_t  emcx[3]; // cluster pos
            clustMatch->GetPosition(emcx);
            TVector3 clustpos(emcx[0],emcx[1],emcx[2]);
            emcphi = clustpos.Phi();
            emceta = clustpos.Eta();
            if(emcphi < 0) emcphi = emcphi+(2*TMath::Pi()); //TLorentz vector is defined between -pi to pi, so negative phi has to be flipped.
            if(emcphi > 1.39 && emcphi < 3.265) fClsTypeEMC = kTRUE; //EMCAL : 80 < phi < 187
            if(emcphi > 4.53 && emcphi < 5.708) fClsTypeDCAL = kTRUE;//DCAL  : 260 < phi < 327
            
            //----selects EMCAL+DCAL clusters when fFlagClsTypeEMC and fFlagClsTypeDCAL is kTRUE
            if(fFlagClsTypeEMC && !fFlagClsTypeDCAL)
                if(!fClsTypeEMC) continue; //selecting only EMCAL clusters
            
            if(fFlagClsTypeDCAL && !fFlagClsTypeEMC)
                if(!fClsTypeDCAL) continue; //selecting only DCAL clusters
            
            /////////////////////////////////////////////
            //Properties of tracks matched to the EMCAL//
            /////////////////////////////////////////////
            fEMCTrkPt->Fill(TrkPt);
            if(TrkPt>1.0)
            {
                fEMCTrketa->Fill(TrkEta);
                fEMCTrkphi->Fill(TrkPhi);
            }
            fEMCdEdx->Fill(TrkP,dEdx);
            fEMCTPCnsig->Fill(TrkP,fTPCnSigma);
            fEMCTPCNpts->Fill(TrkP,track->GetTPCsignalN());
            
            Double_t clustMatchE = clustMatch->E();
            
            fClsEAftMatch->Fill(clustMatchE);
            fNonLinClsEAftMatch->Fill(clustMatch->GetNonLinCorrEnergy());
            
            fClsEtaPhiAftMatch->Fill(emceta,emcphi);
            
            if(TrkPhi > 1.396  && TrkPhi < 3.141) //emc acceptance (80 to 180 degrees)
                fClsEtaPhiAftMatchEMCin->Fill(emceta,emcphi);
            else
                fClsEtaPhiAftMatchEMCout->Fill(emceta,emcphi);
            
            //EMCAL EID info
            Double_t eop = -1.0;
            Double_t m02 = -99999,m20 = -99999,sqm02m20=-99999.0;
            if(track->P()>0)eop = clustMatchE/track->P();
            m02 =clustMatch->GetM02();
            m20 =clustMatch->GetM20();
            
            if(track->Pt()>3.0){
                fHistdEdxEop->Fill(eop,dEdx);
                fHistNsigEop->Fill(eop,fTPCnSigma);
                fM20EovP->Fill(eop,clustMatch->GetM20());
                fM02EovP->Fill(eop,clustMatch->GetM02());
            }
            fM20->Fill(track->Pt(),clustMatch->GetM20());
            fM02->Fill(track->Pt(),clustMatch->GetM02());
            
            //EID THnsparse
            fvalueElectron[0] = track->Pt();
            fvalueElectron[1] = fTPCnSigma;
            fvalueElectron[2] = eop;
            fvalueElectron[3] = m20;
            fvalueElectron[4] = m02;
            fvalueElectron[5] = iSM;
            
            if(fFlagSparse && track->Pt()>2.0){
                fSparseElectron->Fill(fvalueElectron);
            }
            
            //////////////////
            //Apply EID cuts//
            //////////////////
            
            Bool_t fHadTrack = kFALSE, fElectTrack = kFALSE;
            fElectTrack = PassEIDCuts(track, clustMatch, fHadTrack);
            
            if(fHadTrack)
                fHadPt_AftEID->Fill(TrkPt);
            
            if(!fElectTrack) continue;
            
            fInclsElecPt->Fill(TrkPt);
            fNEle++;
            
            ////////////////////
            //NonHFE selection//
            ////////////////////
            Bool_t fFlagNonHFE=kFALSE;
            SelectPhotonicElectron(iTracks,track,fFlagNonHFE,pidM);
            
            //////////////////////////////////
            //Non-HFE efficiency calculation//
            //////////////////////////////////
            Bool_t EffiDenom = kFALSE;
            Bool_t EffiNumTag = kFALSE;
            if(fMCHeader && fCalculateNonHFEEffi){
                EffiDenom = GetNonHFEEffiDenom(track);
                
                if(fFlagNonHFE)
                    EffiNumTag = GetNonHFEEffiRecoTag(track);
            }

        }
    } //track loop
    
    fNElecInEvt->Fill(fNEle);
    
    PostData(1, fOutputList);
}
//___________________________________________
Bool_t AliAnalysisTaskHFEBESpectraEMC::PassEIDCuts(AliVTrack *track, AliVCluster *clust, Bool_t &Hadtrack)
{
    //apply electron identification cuts
    
    Bool_t hadTrk = kFALSE;
    Double_t eop = -1.0;
    Double_t m02 = -999,m20 = -999;
    Double_t clustE = clust->E();
    Double_t TrkPt = track->Pt();
    if(track->P()>0)eop = clustE/track->P();
    m02 =clust->GetM02();
    m20 =clust->GetM20();
    
    //Hadron E/p distribution
    if(fTPCnSigma > fTPCnSigmaHadMin && fTPCnSigma < fTPCnSigmaHadMax)
    {
        if((m02 > fM02Min && m02 < fM02Max) && (m20 > fM20Min && m20 < fM20Max))
        {
            if(eop > fEovPMin && eop < fEovPMax) hadTrk=kTRUE;
        }
    }
    Hadtrack = hadTrk;
    
    if(fTPCnSigma < fTPCnSigmaMin || fTPCnSigma > fTPCnSigmaMax) return kFALSE;
    if(m02 < fM02Min || m02 > fM02Max) return kFALSE;
    if(m20 < fM20Min || m20 > fM20Max) return kFALSE;
    
    fHistEop_AftEID->Fill(TrkPt,eop);
    
    if(eop < fEovPMin || eop > fEovPMax) return kFALSE;
    
    return kTRUE;
}
//________________________________________________________________________
void AliAnalysisTaskHFEBESpectraEMC::SelectPhotonicElectron(Int_t itrack, AliVTrack *track, Bool_t &fFlagPhotonicElec, Int_t iMC)
{
    ///////////////////////////////////////////
    //////Non-HFE - Invariant mass method//////
    ///////////////////////////////////////////
    
    fAOD = dynamic_cast<AliAODEvent*>(InputEvent());
    const AliVVertex *pVtx = fVevent->GetPrimaryVertex();
    Double_t d0z0[2]={-999,-999}, cov[3];
    Double_t DCAxyCut = 0.25, DCAzCut = 1;
    
    Bool_t flagPhotonicElec = kFALSE, flagLSElec = kFALSE;
    Double_t ptAsso=-999., nsigma=-999.0;
    Bool_t fFlagLS=kFALSE, fFlagULS=kFALSE;
    
    Int_t ntracks = -999;
    if(!fUseTender)ntracks = fVevent->GetNumberOfTracks();
    if(fUseTender) ntracks = fTracks_tender->GetEntries();
    
    for (Int_t jtrack = 0; jtrack < ntracks; jtrack++) {
        AliVParticle* VAssotrack = 0x0;
        if(!fUseTender) VAssotrack  = fVevent->GetTrack(jtrack);
        if(fUseTender) VAssotrack = dynamic_cast<AliVTrack*>(fTracks_tender->At(jtrack)); //take tracks from Tender list
        
        if (!VAssotrack) {
            printf("ERROR: Could not receive track %d\n", jtrack);
            continue;
        }
        
        AliVTrack *Assotrack = dynamic_cast<AliVTrack*>(VAssotrack);
        AliESDtrack *eAssotrack = dynamic_cast<AliESDtrack*>(VAssotrack);
        AliAODTrack *aAssotrack = dynamic_cast<AliAODTrack*>(VAssotrack);
        
        //------reject same track
        if(jtrack==itrack) continue;
        
        Double_t mass=-999., width = -999;
        Int_t fPDGe1 = 11; Int_t fPDGe2 = 11;
        
        nsigma = fpidResponse->NumberOfSigmasTPC(Assotrack, AliPID::kElectron);
        ptAsso = Assotrack->Pt();
        
        //------track cuts applied
        if(fAOD) {
            if(!aAssotrack->TestFilterMask(AliAODTrack::kTrkTPCOnly)) continue;
            if(aAssotrack->GetTPCNcls() < 70) continue;
            if((!(aAssotrack->GetStatus()&AliESDtrack::kITSrefit)|| (!(aAssotrack->GetStatus()&AliESDtrack::kTPCrefit)))) continue;
            
            if(aAssotrack->PropagateToDCA(pVtx, fVevent->GetMagneticField(), 20., d0z0, cov))
                if(TMath::Abs(d0z0[0]) > DCAxyCut || TMath::Abs(d0z0[1]) > DCAzCut) continue;
        }
        
        //-------loose cut on partner electron
        if(ptAsso <0.150) continue;
        if(aAssotrack->Eta()<-0.9 || aAssotrack->Eta()>0.9) continue;
        if(nsigma < -3 || nsigma > 3) continue;
        
        Int_t chargeAsso = Assotrack->Charge();
        Int_t charge = track->Charge();
        if(charge>0) fPDGe1 = -11;
        if(chargeAsso>0) fPDGe2 = -11;
        
        fFlagLS=kFALSE; fFlagULS=kFALSE;
        if(charge == chargeAsso) fFlagLS = kTRUE;
        if(charge != chargeAsso) fFlagULS = kTRUE;
        
        //-------define KFParticle to get mass
        AliKFParticle::SetField(fVevent->GetMagneticField());
        AliKFParticle ge1 = AliKFParticle(*track, fPDGe1);
        AliKFParticle ge2 = AliKFParticle(*Assotrack, fPDGe2);
        AliKFParticle recg(ge1, ge2);
        
        if(recg.GetNDF()<1) continue;
        Double_t chi2recg = recg.GetChi2()/recg.GetNDF();
        if(TMath::Sqrt(TMath::Abs(chi2recg))>3.) continue;
        
        Int_t MassCorrect;
        MassCorrect = recg.GetMass(mass,width);
        
        if(fFlagLS && track->Pt()>1) fInvmassLSPt->Fill(track->Pt(),mass);
        if(fFlagULS && track->Pt()>1) fInvmassULSPt->Fill(track->Pt(),mass);
        
        //////////////////////////////////
        //Non-HFE efficiency calculation//
        //////////////////////////////////
        Bool_t EffiNumULSLS = kFALSE;
        if(fMCHeader && fCalculateNonHFEEffi){
            EffiNumULSLS = GetNonHFEEffiULSLS(track, Assotrack, fFlagLS, fFlagULS, mass);
        }

        Double_t TrkPt = track->Pt();
        if(mass < fInvmassCut){
            if(fFlagLS)
                fLSElecPt->Fill(TrkPt);

            if(fFlagULS)
                fULSElecPt->Fill(TrkPt);
        }
        
        if(mass < fInvmassCut && fFlagULS && !flagPhotonicElec)
            flagPhotonicElec = kTRUE; //Tag Non-HFE (random mass cut, not optimised)
    }
    fFlagPhotonicElec = flagPhotonicElec;
}

//________________________________________________________________________
void AliAnalysisTaskHFEBESpectraEMC::GetTrkClsEtaPhiDiff(AliVTrack *t, AliVCluster *v, Double_t &phidiff, Double_t &etadiff)
{
    // Calculate phi and eta difference between a track and a cluster. The position of the track is obtained on the EMCAL surface
    
    phidiff = 999;
    etadiff = 999;
    
    if (!t||!v) return;
    
    Double_t veta = t->GetTrackEtaOnEMCal();
    Double_t vphi = t->GetTrackPhiOnEMCal();
    
    Float_t pos[3] = {0};
    v->GetPosition(pos);
    TVector3 cpos(pos);
    Double_t ceta     = cpos.Eta();
    Double_t cphi     = cpos.Phi();
    etadiff=veta-ceta;
    phidiff=TVector2::Phi_mpi_pi(vphi-cphi);
}
//________________________________________________________________________
void AliAnalysisTaskHFEBESpectraEMC::GetEMCalClusterInfo()
{
    //Get basic EMCal cluster information
    
    Int_t Nclust = -999;
    if(!fUseTender) Nclust = fVevent->GetNumberOfCaloClusters();
    if(fUseTender) Nclust = fCaloClusters_tender->GetEntries();
    
    int NclustAll= 0;
    
    Bool_t fClsTypeEMC = kFALSE, fClsTypeDCAL = kFALSE;
    
    for(Int_t icl=0; icl<Nclust; icl++)
    {
        AliVCluster *clust = 0x0;
        if(!fUseTender) clust = fVevent->GetCaloCluster(icl);
        if(fUseTender) clust = dynamic_cast<AliVCluster*>(fCaloClusters_tender->At(icl));
        if(!clust)  printf("ERROR: Could not receive cluster matched calibrated from track %d\n", icl);
        
        fClsTypeEMC = kFALSE; fClsTypeDCAL = kFALSE;
        
        if(clust && clust->IsEMCAL())
        {
            Double_t clustE = clust->E();
            if(clustE < 0.3) continue;
            
            /////////////////////////////////
            //Select EMCAL or DCAL clusters//
            /////////////////////////////////
            Float_t  emcx[3]; // cluster pos
            clust->GetPosition(emcx);
            TVector3 clustpos(emcx[0],emcx[1],emcx[2]);
            Double_t emcphi = clustpos.Phi();
            Double_t emceta = clustpos.Eta();
            if(emcphi < 0) emcphi = emcphi+(2*TMath::Pi()); //TLorentz vector is defined between -pi to pi, so negative phi has to be flipped.
            
            if(emcphi > 1.39 && emcphi < 3.265) fClsTypeEMC = kTRUE; //EMCAL : 80 < phi < 187
            if(emcphi > 4.53 && emcphi < 5.708) fClsTypeDCAL = kTRUE; //DCAL  : 260 < phi < 327
            
            //----selects EMCAL+DCAL clusters when fFlagClsTypeEMC and fFlagClsTypeDCAL is kTRUE
            if(fFlagClsTypeEMC && !fFlagClsTypeDCAL)
                if(!fClsTypeEMC) continue; //selecting only EMCAL clusters
            
            if(fFlagClsTypeDCAL && !fFlagClsTypeEMC)
                if(!fClsTypeDCAL) continue; //selecting only DCAL clusters
            
            fHistClustE->Fill(clustE);
            fHistNonLinClustE->Fill(clust->GetNonLinCorrEnergy());
            
           // if(centrality>-1)fHistClustEcent->Fill(centrality,clustE);
            fEMCClsEtaPhi->Fill(emceta,emcphi);
            fHistoNCells->Fill(clustE,clust->GetNCells());
            Double_t EperCell = -999.9;
            if(clust->GetNCells()>0)EperCell = clustE/clust->GetNCells();
            fHistoEperCell->Fill(clustE,EperCell);
            
            Float_t tof = clust->GetTOF()*1e+9; // ns
            fHistoTimeEMC->Fill(clustE,tof);
            
            NclustAll++;
        }
    }
    fHistoNCls->Fill(NclustAll);
    
    // cell information
    AliVCaloCells *fCaloCells = fVevent->GetEMCALCells();
    
    //Int_t nSACell, iSACell, mclabel;
    Short_t cellAddr, nSACell;
    Int_t  mclabel;
    Short_t iSACell;
    Double_t cellAmp=-1., cellTimeT=-1., clusterTime=-1., efrac=-1.;
    
    nSACell = fCaloCells->GetNumberOfCells();
    for(iSACell = 0; iSACell < nSACell; iSACell++ ){
        Bool_t haveCell = fCaloCells->GetCell(iSACell, cellAddr, cellAmp, cellTimeT , mclabel, efrac);
        //virtual Bool_t   GetCell(Short_t pos, Short_t &cellNumber, Double_t &amplitude, Double_t &time, Int_t &mclabel,    Double_t  &efrac)
        if(haveCell)fHistoCalCell->Fill(cellAddr,cellAmp);
        
    }
    
    if(!fEMCALGeo)fEMCALGeo  = AliEMCALGeometry::GetInstance(); // not work w.o. Tender
}
//________________________________________________________________________
void AliAnalysisTaskHFEBESpectraEMC::FindMother(AliAODMCParticle* part, Int_t &label, Int_t &pid)
{
    // Find mother in case of MC
    
    if(part->GetMother()>-1)
    {
        label = part->GetMother();
        AliAODMCParticle *partM = (AliAODMCParticle*)fMCArray->At(label);
        pid = partM->GetPdgCode();
    }
    else
    {
        pid = -1;
    }
}
//________________________________________________________________________
void AliAnalysisTaskHFEBESpectraEMC::Terminate(Option_t *)
{
    // Draw result to the screen
    // Called once at the end of the query
    
    fOutputList = dynamic_cast<TList*> (GetOutputData(1));
    if (!fOutputList) {
        printf("ERROR: Output list not available\n");
        return;
    }
}
//_________________________________________
Bool_t AliAnalysisTaskHFEBESpectraEMC::GetNMCPartProduced()
{
    //Get number of MC particles produced by generators.
    
    TList *lh = fMCHeader->GetCocktailHeaders();
    fNTotMCpart = 0;
    fNembMCpi0 = 0;
    fNembMCeta = 0;
    fNpureMC = 0;
    TString MCgen;
    TString embpi0("pi");
    TString embeta("eta");
    
    if(!lh){
        AliError("no MC header");
        return (0);
    }
    
    for(int igene=0; igene<lh->GetEntries(); igene++)
    {
        AliGenEventHeader* gh=(AliGenEventHeader*)lh->At(igene);
        if(!gh) continue;
        
        MCgen =  gh->GetName();
        //   cout << "Gen name, N produced = " << gh->GetName() << ", " << gh->NProduced() << endl;
        if(igene==0) fNpureMC = gh->NProduced();  // generated by HIJING
        
        //   if(MCgen.Contains(embpi0))cout << MCgen << endl;
        //   if(MCgen.Contains(embeta))cout << MCgen << endl;
        
        if(MCgen.Contains(embpi0))fNembMCpi0 = fNTotMCpart;
        if(MCgen.Contains(embeta))fNembMCeta = fNTotMCpart;
        fNTotMCpart += gh->NProduced();
    }
    //  cout << "fNpureMC, fNembMCpi0, fNembMCeta, fNTotMCpart : " <<fNpureMC << ", " << fNembMCpi0 << ", " << fNembMCeta << ", " << fNTotMCpart << endl;
    
    return kTRUE;
}
//_________________________________________
void AliAnalysisTaskHFEBESpectraEMC::GetPi0EtaWeight(THnSparse *SparseWeight)
{
    //Get pi0 and eta information for weight calculation
    
    Double_t fvalue[4] = {-999,-999,-999,-999};
    
    for(int imc=0; imc< fNTotMCpart; imc++)
    {
        AliAODMCParticle *AODMCtrack = (AliAODMCParticle*)fMCArray->At(imc);
        if(TMath::Abs(AODMCtrack->Eta()) > 1.2) continue;
        
        //-------Get PDG
        Int_t TrackPDG = TMath::Abs(AODMCtrack->GetPdgCode());
        if((TrackPDG != 111) && (TrackPDG != 221) && (TrackPDG != 22)) continue;
        
        Double_t fPartPDGid = -999;
        if (TrackPDG == 111) fPartPDGid = 0.2;
        if (TrackPDG == 221) fPartPDGid = 1.2;
        if (TrackPDG == 22) fPartPDGid = 2.2;
        
        Double_t fTrkPt = AODMCtrack->Pt();
        
        //-------Check if the particle is from hijing or not
        Bool_t fFromHijing = kHijing;
        if(imc >= fNpureMC)fFromHijing = kElse;
        
        //------Get type of the particle
        Int_t fType = GetPi0EtaType(AODMCtrack);
        
        fvalue[0] = fTrkPt;
        fvalue[1] = fPartPDGid;
        fvalue[2] = fFromHijing;
        fvalue[3] = fType;
        
        SparseWeight->Fill(fvalue);
    }
}
//_________________________________________
Int_t AliAnalysisTaskHFEBESpectraEMC::GetPi0EtaType(AliAODMCParticle *part)
{
    // Return the type of particle
    
    // IsPrimary
    Bool_t primMC = part->IsPrimary();
    if(!primMC) return kNotIsPrimary;
    
    // Mother
    Int_t motherlabel = part->GetMother();
    if(motherlabel<0) return kNoMother;
    
    else {
        AliAODMCParticle *mother = (AliAODMCParticle*)fMCArray->At(motherlabel);
        Int_t motherpdg = TMath::Abs(mother->GetPdgCode());
        
        if(motherpdg == 111 || motherpdg == 221 || motherpdg == 223 || motherpdg == 333 || motherpdg == 331 || motherpdg == 113 || motherpdg == 213 || motherpdg == 313 || motherpdg == 323) return kLightMesons;
        
        if ( (int(TMath::Abs(motherpdg)/100.)%10) == 5 || (int(TMath::Abs(motherpdg)/1000.)%10) == 5 ) return kBeauty;
        if ( (int(TMath::Abs(motherpdg)/100.)%10) == 4 || (int(TMath::Abs(motherpdg)/1000.)%10) == 4 ) return kCharm;
        return kNoFeedDown;
    }
}
//_________________________________________
Bool_t AliAnalysisTaskHFEBESpectraEMC::GetNonHFEEffiDenom(AliVTrack *track)
{
  //Calculate Non-HFE efficiency demoninator
    
    Double_t TrkPt = track->Pt();
    Int_t iTrklabel = TMath::Abs(track->GetLabel());
    if(iTrklabel == 0) return kFALSE;
    AliAODMCParticle *MCPart = (AliAODMCParticle*)fMCArray->At(iTrklabel);
    if(TMath::Abs(MCPart->GetPdgCode())!=11) return kFALSE;
    fRealInclsElecPt->Fill(TrkPt);
    
    fIsFrmEmbPi0 = kFALSE;
    fIsFrmEmbEta= kFALSE;
    ftype = -1;
    fWeight = 1.0;
    Bool_t fFromHijing = kTRUE;
    Int_t iMCmom=-999, MomPDG = -999;
    Double_t MomPt =-999;
    
    Bool_t fNonHFE = IsNonHFE(MCPart, fFromHijing, ftype, iMCmom, MomPDG, MomPt);
    if(!fNonHFE) return kFALSE;
    fNonHFeTrkPt->Fill(TrkPt);
    
    ///////////////////////////////////////
    // Check for pi0/eta from embbedding //
    ///////////////////////////////////////
    //not considering the cases : eta->pi0->gamma->ele, eta->pi0->elec, check in next iteration
    //Cases considered : pi0->e, pi0->gamma->e, eta->e, eta->gamma->e
    Bool_t IsMissingEta = kFALSE;
    Int_t missingtype=-1;
    
    if(MomPDG == 111) {
        if(iMCmom >= fNembMCpi0 && iMCmom < fNembMCeta) fIsFrmEmbPi0 = kTRUE;
        
        // cout << "I am here pi0 " << fNembMCpi0 << ", " << fNembMCeta << endl;
        //missing eta
        AliAODMCParticle *MCPartMom = (AliAODMCParticle*)fMCArray->At(iMCmom);
        Int_t iMCgmom = -999;
        iMCgmom = MCPartMom->GetMother();
        if(iMCgmom > 0) {
            AliAODMCParticle *MCPartGMom = (AliAODMCParticle*)fMCArray->At(iMCgmom);
            Int_t GMomPDG = TMath::Abs(MCPartGMom->GetPdgCode());
            
            if(GMomPDG == 221){
                if(iMCgmom >= fNembMCeta && iMCgmom < fNTotMCpart){
                    IsMissingEta = kTRUE;
                    missingtype = GetPi0EtaType(MCPartGMom);
                }
            }
        }
    }
    if(MomPDG == 221){
        //  cout << "I am here eta" << fNembMCeta << ", " << fNTotMCpart << endl;
        
        if(iMCmom >= fNembMCeta && iMCmom < fNTotMCpart) fIsFrmEmbEta = kTRUE;
    }
    
    if(MomPDG == 22) //if pi0/eta->gamma->e : Rewrite "ftype" and "MomPt" with Gmother info
    {
        // cout << "I am here gamma " << fNembMCpi0 << ", " << fNembMCeta << ", " << fNTotMCpart << endl;
        
        AliAODMCParticle *MCPartMom = (AliAODMCParticle*)fMCArray->At(iMCmom);
        Int_t iMCgmom = -999;
        iMCgmom = MCPartMom->GetMother();
        AliAODMCParticle *MCPartGMom = (AliAODMCParticle*)fMCArray->At(iMCgmom);
        Int_t GMomPDG = TMath::Abs(MCPartGMom->GetPdgCode());
        
        // cout << "gamma GMom pdg : " << GMomPDG << endl;
        if(GMomPDG == 111){
            // cout << "pi0 GM " << iMCgmom <<endl;
            if(iMCgmom >= fNembMCpi0 && iMCgmom < fNembMCeta){
                fIsFrmEmbPi0 = kTRUE;
                ftype = GetPi0EtaType(MCPartGMom);
                MomPt = MCPartGMom->Pt();
            }
            //missing eta
            Int_t iMCggmom = -999;
            if(iMCggmom > 0) {
                iMCggmom = MCPartGMom->GetMother();
                AliAODMCParticle *MCPartGGMom = (AliAODMCParticle*)fMCArray->At(iMCggmom);
                Int_t GGMomPDG = TMath::Abs(MCPartGGMom->GetPdgCode());
                
                if(GGMomPDG == 221){
                    if(iMCggmom >= fNembMCeta && iMCggmom < fNTotMCpart){
                        IsMissingEta = kTRUE;
                        missingtype = GetPi0EtaType(MCPartGGMom);
                    }
                }
            }
        }
        if(GMomPDG == 221){
            // cout << "eta GM " << iMCgmom <<endl;
            if(iMCgmom >= fNembMCeta && iMCgmom < fNTotMCpart){
                fIsFrmEmbEta = kTRUE;
                ftype = GetPi0EtaType(MCPartGMom);
                MomPt = MCPartGMom->Pt();
            }
        }
    }
    
    if(IsMissingEta && (missingtype == kNoMother)) fMissingEmbEtaEleTrkPt->Fill(TrkPt);
    
    //  cout << "N Gen : " << fNpureMC << ", " << fNembMCpi0 << "," << fNembMCeta << ", " << fNTotMCpart << endl;
    //  cout << iMCmom << ", " << MomPDG << ", "  << fFromHijing << ", " << ftype << ", " << fIsFrmEmbPi0 << ", " << fIsFrmEmbEta << endl;
    //if(iTrklabel > fNembMCeta) cout << "electron from eta embedded events : "<< iTrklabel << ", " << iMCmomNew << ", " << iMCmom << ", " << MomPDG << ", " << ftype << ", " << fIsFrmEmbPi0 << ", " << fIsFrmEmbEta << endl;
    
    //////////////////////////////////////////////////
    ///Get weight for Embedded pi0/eta with NoMother//
    //////////////////////////////////////////////////
    
    if(fIsFrmEmbPi0 && ftype==kNoMother) fWeight = fPi0Weight->Eval(MomPt);
    if(fIsFrmEmbEta && ftype==kNoMother) fWeight = fEtaWeight->Eval(MomPt);
    
    //  if(fIsFrmEmbPi0 && ftype==kNoMother) cout << "weight : " << EvalPi0Weight(MomPt) << endl;
    //  if(fIsFrmEmbEta && ftype==kNoMother) cout << "weight : " << EvalEtaWeight(MomPt) << endl;
    
    //////////////////////////////////////////
    //Select electrons from embedded pi0/eta//
    //////////////////////////////////////////
    
    if(fIsFrmEmbPi0 || fIsFrmEmbEta){
        fNonHFeEmbAllTypeTrkPt->Fill(TrkPt);
        
        if(ftype == kNoMother){ //embedded pi0/eta with no Mom
            fNonHFeEmbTrkPt->Fill(TrkPt);
            fNonHFeEmbWeightTrkPt->Fill(TrkPt,fWeight);
            if(fIsFrmEmbPi0) fPi0eEmbWeightTrkPt->Fill(TrkPt,fWeight);
            if(fIsFrmEmbEta) fEtaeEmbWeightTrkPt->Fill(TrkPt,fWeight);
        }
    }

    return kTRUE;
}

//___________________________________________
Bool_t  AliAnalysisTaskHFEBESpectraEMC::IsNonHFE(AliAODMCParticle *MCPart, Bool_t &fFromHijing, Int_t &type, Int_t &iMCmom, Int_t &MomPDG, Double_t &MomPt)
{
    //Is electron from pi0, eta and gamma
    
    iMCmom = MCPart->GetMother();
    AliAODMCParticle *MCPartMom = (AliAODMCParticle*)fMCArray->At(iMCmom);
    MomPDG = TMath::Abs(MCPartMom->GetPdgCode());
    MomPt = MCPartMom->Pt();
    
    if((MomPDG == 111) || (MomPDG == 221) || (MomPDG == 22)){
        if(iMCmom >= fNpureMC)fFromHijing = kFALSE;
        type = GetPi0EtaType(MCPartMom);
        return kTRUE;
    }
    else return kFALSE;
}
//_________________________________________
Bool_t AliAnalysisTaskHFEBESpectraEMC::GetNonHFEEffiRecoTag(AliVTrack *track)
{
    Double_t TrkPt = track->Pt();
    
    fRecoNonHFeTrkPt->Fill(TrkPt);
    if((fIsFrmEmbPi0 || fIsFrmEmbEta) && (ftype == kNoMother)){
        fRecoNonHFeEmbTrkPt->Fill(TrkPt);
        fRecoNonHFeEmbWeightTrkPt->Fill(TrkPt,fWeight);
        if(fIsFrmEmbPi0) fRecoPi0eEmbWeightTrkPt->Fill(TrkPt,fWeight);
        if(fIsFrmEmbEta) fRecoEtaeEmbWeightTrkPt->Fill(TrkPt,fWeight);
    }
        
    return kTRUE;
}

//_________________________________________
Bool_t AliAnalysisTaskHFEBESpectraEMC::GetNonHFEEffiULSLS(AliVTrack *track, AliVTrack *Assotrack, Bool_t fFlagLS, Bool_t fFlagULS, Double_t mass)
{

    Double_t TrkPt = track->Pt();

    Int_t iTrkAssolabel = TMath::Abs(Assotrack->GetLabel());
    if(iTrkAssolabel == 0) return kFALSE;
    AliAODMCParticle *MCPartAsso = (AliAODMCParticle*)fMCArray->At(iTrkAssolabel);
    
    if(TMath::Abs(MCPartAsso->GetPdgCode())==11){ // check origin of asso elec
        Bool_t fAssoFromHijing = kTRUE;
        Int_t iMCAssomom=-999, AssoMomPDG = -999, fAssotype=-1;
        Double_t AssoMomPt =-999;
        Bool_t fAssoNonHFE = IsNonHFE(MCPartAsso, fAssoFromHijing, fAssotype, iMCAssomom, AssoMomPDG, AssoMomPt);
        
        //cout << "Asso ele mom : " << iMCAssomom << ", " << AssoMomPDG << ", " << iMCmom << ", " << MomPDG << ", " << fIsFrmEmbPi0 << ", " << fIsFrmEmbEta << ", " << ftype << endl;
        
        if(fAssoNonHFE){
            if(fFlagLS) fNonHFePairInvmassLS->Fill(mass);
            if(fFlagULS) fNonHFePairInvmassULS->Fill(mass);
        }
    }
    
    if((fIsFrmEmbPi0 || fIsFrmEmbEta) && ftype==kNoMother){ //If parent e from embedded pi0/eta + NoMom
        if(fFlagLS) fNonHFeEmbInvmassLS->Fill(mass);
        if(fFlagULS) fNonHFeEmbInvmassULS->Fill(mass);
        if(fFlagLS) fNonHFeEmbWeightInvmassLS->Fill(mass, fWeight);
        if(fFlagULS) fNonHFeEmbWeightInvmassULS->Fill(mass, fWeight);
                        
        if(fIsFrmEmbPi0){ //if from pi0
            if(fFlagLS) fPi0EmbInvmassLS->Fill(mass);
            if(fFlagULS) fPi0EmbInvmassULS->Fill(mass);
            if(fFlagLS) fPi0EmbWeightInvmassLS->Fill(mass, fWeight);
            if(fFlagULS) fPi0EmbWeightInvmassULS->Fill(mass, fWeight);
        }
        if(fIsFrmEmbEta){ //if from eta
            if(fFlagLS) fEtaEmbInvmassLS->Fill(mass);
            if(fFlagULS) fEtaEmbInvmassULS->Fill(mass);
            if(fFlagLS) fEtaEmbWeightInvmassLS->Fill(mass, fWeight);
            if(fFlagULS) fEtaEmbWeightInvmassULS->Fill(mass, fWeight);
        }
    }
    
    if(mass < fInvmassCut){
        if(fFlagLS){
            if((fIsFrmEmbPi0 || fIsFrmEmbEta) && (ftype == kNoMother)){
                fRecoLSeEmbTrkPt->Fill(TrkPt);
                fRecoLSeEmbWeightTrkPt->Fill(TrkPt,fWeight);
                if(fIsFrmEmbPi0) fRecoPi0LSeEmbWeightTrkPt->Fill(TrkPt,fWeight);
                if(fIsFrmEmbEta) fRecoEtaLSeEmbWeightTrkPt->Fill(TrkPt,fWeight);
            }
        }
        if(fFlagULS){
            if((fIsFrmEmbPi0 || fIsFrmEmbEta) && (ftype == kNoMother)){
                fRecoULSeEmbTrkPt->Fill(TrkPt);
                fRecoULSeEmbWeightTrkPt->Fill(TrkPt,fWeight);
                if(fIsFrmEmbPi0) fRecoPi0ULSeEmbWeightTrkPt->Fill(TrkPt,fWeight);
                if(fIsFrmEmbEta) fRecoEtaULSeEmbWeightTrkPt->Fill(TrkPt,fWeight);
            }
        }
    }
    
    return kTRUE;
}