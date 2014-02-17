//////////////////////////////////////////////////////////////////////////////
//
// Class GUser
//
// Class for User treatment
//
////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------
// ***************************************************************************
// *                                                                         *
// *   This program is free software; you can redistribute it and/or modify  *
// *   it under the terms of the GNU General Public License as published by  *
// *   the Free Software Foundation; either version 2 of the License, or     *
// *   (at your option) any later version.                                   *
// *                                                                         *
// **************************************************************************/


#include "./GUser.h"

// ROOT headers
#include "TROOT.h"
#include "TH1F.h"
#include "TString.h"

// NPTOOL headers
#include "NPOptionManager.h"
#include "RootOutput.h"
#include "TTiaraHyballPhysics.h"
#include "TTiaraBarrelPhysics.h"
#include "TMust2Physics.h"
#include "TCATSPhysics.h"
#include "TCharissaPhysics.h"

// C++ headers
#include <iostream>
#include <fstream>
#include <string>
#include <map>
using namespace std;


ClassImp (GUser); 
////////////////////////////////////////////////////////////////////////////////
GUser::GUser (GDevice* _fDevIn, GDevice* _fDevOut){ 
  // Constructor/initialisator of Acquisition object 
  // entry:
  // - Input Device
  // - Output Device
  fDevIn   = _fDevIn;
  fDevOut  = _fDevOut;

  // instantiate detector objects
  fModularLabel  = new TModularLabel();
  fMust2         = new TMust2();
  fCATS          = new TCATS();
  fExogam        = new TExogam();
  fTrigger	     = new TTrigger();
  fTac           = new TTac();
  fPlastic       = new TPlastic();
  fLise          = new TLise();
  fTiaraHyball   = new TTiaraHyball();
  fTiaraBarrel   = new TTiaraBarrel();
  fCharissa      = new TCharissa();

  fNPToolArgument = "";

  MySpectraList = GetSpectra();
  cout << "Spectra done" << endl;  
}
////////////////////////////////////////////////////////////////////////////////
GUser::GUser (GDevice* _fDevIn, string NPToolArgument){ 
  // Constructor/initialisator of Acquisition object 
  // entry:
  // - Input Device
  // - Output Device
  fDevIn   = _fDevIn;
  fDevOut  = NULL;

  // instantiate detector objects
  fModularLabel  = new TModularLabel();
  fMust2         = new TMust2();
  fCATS          = new TCATS();
  fExogam        = new TExogam();
  fTrigger	     = new TTrigger();
  fTac           = new TTac();
  fPlastic       = new TPlastic();
  fLise          = new TLise();
  fTiaraHyball   = new TTiaraHyball();
  fTiaraBarrel   = new TTiaraBarrel();
  fCharissa      = new TCharissa();

  fNPToolArgument = NPToolArgument;

  MySpectraList = GetSpectra();
  cout << "Spectra done" << endl;  
}
////////////////////////////////////////////////////////////////////////////////
GUser::~GUser()  {
  // Destructor of class GUser
  delete fMust2;
  delete fCATS;
  delete fExogam;
  delete fTrigger;
  delete fTac;
  delete fPlastic;
  delete fLise;
  delete fTiaraHyball;
  delete fTiaraBarrel;
  delete fCharissa;

  gROOT->cd();
  RootOutput::getInstance()->Destroy();

}
////////////////////////////////////////////////////////////////////////////////
void GUser::InitUser(){
  // Initialisation for global  user treatement

  cout<<  "- ---------< Init User  >------------------!\n";
  cout << "+++++ Init NPOption Manager" << endl;
  // -GH for generating histos
  // -CH for checking histos
  // -C for calibration files
  // -O for outputting the Physical Tree (followed by its name)
  NPOptionManager *myOptionManager = NPOptionManager::getInstance(fNPToolArgument);

  string detectorFile = myOptionManager->GetDetectorFile();
  string OutputfileName      = myOptionManager->GetOutputFile();
  RootOutput::getInstance("Analysis/"+OutputfileName, "AnalysedTree"); 

  cout << "+++++ Init Detector Manager" << endl;
  fMyDetector = new DetectorManager();
  fMyDetector->ReadConfigurationFile(detectorFile);

  cout << "+++++ Register detector spectra to viGru" << endl;
  vector < map < vector <string>, TH1* > > mySpectra = fMyDetector->GetSpectra();
 
  for (unsigned int i = 0; i < mySpectra.size(); ++i) {
    map<vector <string>, TH1*>::iterator it;
    for (it = mySpectra[i].begin(); it != mySpectra[i].end(); ++it) {
      GetSpectra()->AddSpectrum(it->second, it->first[0].c_str());
    }
  } 

  // connect data objects to the physics
  // need to be done detector by detector
  ((TCATSPhysics*)        fMyDetector->GetDetector("CATS"))        -> SetRawDataPointer(fCATS        -> GetCATSData());
  ((TMust2Physics*)       fMyDetector->GetDetector("MUST2"))       -> SetRawDataPointer(fMust2       -> GetMust2Data());
  ((TTiaraHyballPhysics*) fMyDetector->GetDetector("TiaraHyball")) -> SetRawDataPointer(fTiaraHyball -> GetTiaraHyballData());
  ((TTiaraBarrelPhysics*) fMyDetector->GetDetector("TiaraBarrel")) -> SetRawDataPointer(fTiaraBarrel -> GetTiaraBarrelData());
  ((TCharissaPhysics*)    fMyDetector->GetDetector("Charissa"))    -> SetRawDataPointer(fCharissa    -> GetCharissaData());

  // Add correlation Spectra
  int NBins = 16384/8;
  int MinBin = 0;
  int MaxBin = 16384;
  TH2F* MUST_CATS = new TH2F("MUST_CATS","MUST_CATS",NBins,MinBin,MaxBin,NBins,MinBin,MaxBin);
  GetSpectra()->AddSpectrum(MUST_CATS,"Correlation");

  TH2F* MUST_TIARA = new TH2F("MUST_TIARA","MUST_TIARA",NBins,MinBin,MaxBin,NBins,MinBin,MaxBin);
  GetSpectra()->AddSpectrum(MUST_TIARA,"Correlation");

  TH2F* MUST_CHARISSA = new TH2F("MUST_CHARISSA","MUST_CHARISSA",NBins,MinBin,MaxBin,NBins,MinBin,MaxBin);
  GetSpectra()->AddSpectrum(MUST_CHARISSA,"Correlation");

  TH2F* MUST_EXO = new TH2F("MUST_EXO","MUST_EXO",NBins,MinBin,MaxBin,NBins,MinBin,MaxBin);
  GetSpectra()->AddSpectrum(MUST_EXO,"Correlation");

}
////////////////////////////////////////////////////////////////////////////////
void GUser::InitUserRun(){
  // Initialisation for user treatemeant for each  run  
  // For specific user treatement
  cout << "Init run" << endl;
  fModularLabel->LoadLabel("ModularLabel.txt");
  fModularLabel->Init(GetEvent()->GetDataParameters());
  cout << "End Init ModularLabel" << endl ;
  fMust2->Init(GetEvent()->GetDataParameters());
  cout << "End Init Must2"<<endl;
  fCATS->Init(GetEvent()->GetDataParameters());
  cout << "End Init CATS"<<endl;
  fExogam->Init(GetEvent()->GetDataParameters());
  cout << "End Init Exogam"<<endl;
  fTrigger->Init(GetEvent()->GetDataParameters());
  cout << "End Init Trigg"<<endl;
  fPlastic->Init(GetEvent()->GetDataParameters());
  cout << "End Init Plastic"<<endl;
  fLise->Init(GetEvent()->GetDataParameters());
  cout << "End Init Lise"<<endl;
  fTac->Init(GetEvent()->GetDataParameters());
  cout << "End Init Tac"<<endl;
  fTiaraHyball->Init(GetEvent()->GetDataParameters());
  cout << "End Init Tiara/Hyball"<<endl;
  fTiaraBarrel->Init(GetEvent()->GetDataParameters());
  cout << "End Init Tiara/Barrel"<<endl;
  fCharissa->Init(GetEvent()->GetDataParameters());
  cout << "End Init Charissa"<<endl;
  cout << "End Init run"<<endl;

  // keep track of read labels
  ofstream out_rej,out_acc;
  out_acc.open("label_accepted.dat");
  out_rej.open("label_rejected.dat");
  // Simulate an event loop with all label and value 0
  for (Int_t i = 0; i < GetEvent()->GetDataParameters()->GetNbParameters(); i++) {
    bool included = true;

    if (fModularLabel         ->Is(GetEvent()->GetDataParameters()->GetLabel(i),0)) {}
    else if (fMust2           ->Is(GetEvent()->GetDataParameters()->GetLabel(i),0)) {}
    else if (fCATS            ->Is(GetEvent()->GetDataParameters()->GetLabel(i),0)) {}
    else if (fExogam          ->Is(GetEvent()->GetDataParameters()->GetLabel(i),0)) {}
    else if (fTrigger         ->Is(GetEvent()->GetDataParameters()->GetLabel(i),0)) {}
    else if (fPlastic         ->Is(GetEvent()->GetDataParameters()->GetLabel(i),0)) {}
    else if (fLise            ->Is(GetEvent()->GetDataParameters()->GetLabel(i),0)) {} 
    else if (fTac             ->Is(GetEvent()->GetDataParameters()->GetLabel(i),0)) {}
    else if (fTiaraHyball     ->Is(GetEvent()->GetDataParameters()->GetLabel(i),0)) {}
    else if (fTiaraBarrel     ->Is(GetEvent()->GetDataParameters()->GetLabel(i),0)) {}
    else if (fCharissa        ->Is(GetEvent()->GetDataParameters()->GetLabel(i),0)) {}
    else included = false;

    if (!included) 
      out_rej << i <<" "<<GetDataParameters()->GetParName(i)<<endl;
    else 
      out_acc << i <<" "<<GetDataParameters()->GetParName(i)<<endl;
  }

  out_rej.close();
  out_acc.close();
}
////////////////////////////////////////////////////////////////////////////////
void GUser::User(){ 
  // clear objects
  fModularLabel -> Clear();
  fMust2        -> Clear();
  fCATS         -> Clear();
  fExogam       -> Clear();
  fTrigger      -> Clear();
  fTac          -> Clear();
  fPlastic      -> Clear();
  fLise         -> Clear();
  fTiaraHyball  -> Clear();
  fTiaraBarrel  -> Clear();
  fCharissa     -> Clear();

  //////////////////////////////////////////////////
  //     Unpack events & fill raw data objects    //
  //////////////////////////////////////////////////

  int mySize =  GetEventArrayLabelValueSize()/2;
  for (Int_t i = 0; i < mySize; i++) {
         if (fModularLabel  ->Is(GetEventArrayLabelValue_Label(i),GetEventArrayLabelValue_Value(i))) {}
    else if (fMust2         ->Is(GetEventArrayLabelValue_Label(i),GetEventArrayLabelValue_Value(i))) {}
    else if (fCATS          ->Is(GetEventArrayLabelValue_Label(i),GetEventArrayLabelValue_Value(i))) {}
    else if (fExogam        ->Is(GetEventArrayLabelValue_Label(i),GetEventArrayLabelValue_Value(i))) {}
    else if (fTrigger       ->Is(GetEventArrayLabelValue_Label(i),GetEventArrayLabelValue_Value(i))) {}
    else if (fPlastic       ->Is(GetEventArrayLabelValue_Label(i),GetEventArrayLabelValue_Value(i))) {}
    else if (fLise          ->Is(GetEventArrayLabelValue_Label(i),GetEventArrayLabelValue_Value(i))) {} 
    else if (fTac           ->Is(GetEventArrayLabelValue_Label(i),GetEventArrayLabelValue_Value(i))) {}
    else if (fTiaraHyball   ->Is(GetEventArrayLabelValue_Label(i),GetEventArrayLabelValue_Value(i))) {}
    else if (fTiaraBarrel   ->Is(GetEventArrayLabelValue_Label(i),GetEventArrayLabelValue_Value(i))) {}
    else if (fCharissa      ->Is(GetEventArrayLabelValue_Label(i),GetEventArrayLabelValue_Value(i))) {}
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //        Call BuildPhysicalEvent (physical treatment) for each declared detector in the detector.txt file      //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  fMyDetector->BuildPhysicalEvent();
  // Fill the Physics Tree
  RootOutput::getInstance()->GetTree()->Fill(); 

  /////////////////////////////////////////////////////////////////////////////
  // Fill the Correlation Histo
  short must = fModularLabel->GetValue("EXO_MUST");
  short exogam = fModularLabel->GetValue("EXO_EXO");
  short cats = fModularLabel->GetValue("EXO_CATS");
  short tiara = fModularLabel->GetValue("EXO_TIARA");
  short charissa = fModularLabel->GetValue("EXO_CHARISSA");

  // Add correlation Spectra
  int NBins = 4000;
  int MinBin = 0;
  int MaxBin = 16384;
  GetSpectra()->GetHisto("MUST_CATS")->Fill(must,cats);
  GetSpectra()->GetHisto("MUST_TIARA")->Fill(must,tiara);
  GetSpectra()->GetHisto("MUST_CHARISSA")->Fill(must,charissa);
  GetSpectra()->GetHisto("MUST_EXO")->Fill(must,exogam);

}
////////////////////////////////////////////////////////////////////////////////
void GUser::EndUserRun(){
  //  end of run ,  executed a end of each run
  cout <<"--------------< End User Run >------------------\n";

}
////////////////////////////////////////////////////////////////////////////////
void GUser::EndUser(){
  // globlal final end executed a end of runs
  // must be explicitly called! 


  cout <<"--------------< End User 1> ------------------\n";

  //cout << "End save spectra " << endl;    

}
////////////////////////////////////////////////////////////////////////////////
void GUser::InitTTreeUser(){
  cout << "GUser::InitTTreeUser()" << endl;
  cout << "GUser::InitTTreeUser() -> fTheTree " << fTheTree << endl;

  fTheTree->Branch("RunNumber",&fRunNumber,"RunNumber/I");
  fTheTree->Branch("EvtNumber",&fEventCount,"EvtNumber/I");

  fModularLabel -> InitBranch(fTheTree);
  fMust2        -> InitBranch(fTheTree);
  fCATS         -> InitBranch(fTheTree);
  fExogam       -> InitBranch(fTheTree);
  fTrigger      -> InitBranch(fTheTree);
  fTac          -> InitBranch(fTheTree);
  fPlastic      -> InitBranch(fTheTree);
  fLise         -> InitBranch(fTheTree);
  fTiaraHyball  -> InitBranch(fTheTree);
  fTiaraBarrel  -> InitBranch(fTheTree);
  fCharissa     -> InitBranch(fTheTree);

  cout << "End GUser::InitTTreeUser()" << endl;
}
