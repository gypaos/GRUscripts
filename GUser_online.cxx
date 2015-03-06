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
#include "./GUser_online.h"

// ROOT headers
#include "TROOT.h"
#include "TH1.h"
#include "TString.h"

// NPTOOL headers
#include "NPOptionManager.h"
#include "RootOutput.h"
#include "TTiaraHyballPhysics.h"
#include "TTiaraBarrelPhysics.h"
#include "TMust2Physics.h"
#include "TCATSPhysics.h"
#include "TCharissaPhysics.h"
#include "TExogamPhysics.h"
// GANIL2ROOT
#include "TModularLabel.h" 
#include "TMust2.h"   
#include "TCATS.h"
#include "TExogam.h" 
#include "TTrigger.h"  
#include "TPlastic.h"  
#include "TLise.h"   
#include "TTiaraHyball.h"   
#include "TTiaraBarrel.h"  
#include "TCharissa.h" 

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
  fNPToolArgument = "-D ./detector.txt  -C calibration.txt -GH";
}
  ////////////////////////////////////////////////////////////////////////////////
GUser::GUser (GDevice* _fDevIn, string NPToolArgument){ 
  // Constructor/initialisator of Acquisition object 
  // entry:
  // - Input Device
  // - Output Device
  fDevIn   = _fDevIn;
  fDevOut = NULL;
  fNPToolArgument = NPToolArgument; 
}
////////////////////////////////////////////////////////////////////////////////
void GUser::Init(string NPToolArgument){ 
  // -GH for generating histos
  // -CH for checking histos
  // -C for calibration files
  // -O for outputting the Physical Tree (followed by its name)
  NPOptionManager* myOptionManager = NPOptionManager::getInstance(fNPToolArgument);

  string detectorFile = myOptionManager->GetDetectorFile();
  string OutputfileName = myOptionManager->GetOutputFile();
  if(OutputfileName!=myOptionManager->GetDefaultOutputFile()){
    RootOutput::getInstance("Analysis/"+OutputfileName, "AnalysedTree"); 
  }

  fMyDetector = new DetectorManager();
  fMyDetector->ReadConfigurationFile(detectorFile);

  // Adding spectra  
  MySpectraList = GetSpectra();
  vector < map < vector <string>, TH1* > > mySpectra = fMyDetector->GetSpectra();
 
  for (unsigned int i = 0; i < mySpectra.size(); ++i) {
    map<vector <string>, TH1*>::iterator it;
    for (it = mySpectra[i].begin(); it != mySpectra[i].end(); ++it) {
      GetSpectra()->AddSpectrum(it->second, it->first[0].c_str());
    }
  } 


  // Instantiate the TDetectorManager
  fDetectorManager = new G2R::TDetectorManager();
  // Add the detector from the NPTool detector manager:
  vector<string> DetectorList = fMyDetector->GetDetectorList();
  unsigned int sizeL = DetectorList.size(); 
  for(unsigned int i = 0 ; i < sizeL ; i++){
    fDetectorManager->AddDetector(DetectorList[i]);
  }

  // If you want to add more stuff not dealed with NPTool this is the place //
  fDetectorManager->AddDetector("ModularLabel");
  fDetectorManager->AddDetector("Trigger");
  TModularLabel* ML = (TModularLabel*) fDetectorManager->GetDetector("ModularLabel");
  ML->LoadLabel("ModularLabel.txt");
 
  ////////////////////////////////////////////////////////////////////////////
  // connect data objects to the physics
  // need to be done detector by detector
  TCATS* CATS = (TCATS*) fDetectorManager->GetDetector("CATS");
  ((TCATSPhysics*) fMyDetector->GetDetector("CATS"))-> SetRawDataPointer(CATS->GetCATSData());
  
  TMust2* Must2 = (TMust2*) fDetectorManager->GetDetector("MUST2");
  ((TMust2Physics*) fMyDetector->GetDetector("MUST2"))-> SetRawDataPointer(Must2->GetMust2Data());
  
  TTiaraHyball* TiaraHyball = (TTiaraHyball*) fDetectorManager->GetDetector("TiaraHyball");
  ((TTiaraHyballPhysics*) fMyDetector->GetDetector("TiaraHyball"))->SetRawDataPointer(TiaraHyball->GetTiaraHyballData());
  
  TTiaraBarrel* TiaraBarrel = (TTiaraBarrel*) fDetectorManager->GetDetector("TiaraBarrel");
  ((TTiaraBarrelPhysics*) fMyDetector->GetDetector("TiaraBarrel"))->SetRawDataPointer(TiaraBarrel->GetTiaraBarrelData());

  TCharissa* Charissa = (TCharissa*) fDetectorManager->GetDetector("Charissa");
  ((TCharissaPhysics*) fMyDetector->GetDetector("Charissa"))->SetRawDataPointer(Charissa->GetCharissaData());

  TExogam* Exogam = (TExogam*) fDetectorManager->GetDetector("EXOGAM");
  ((TExogamPhysics*) fMyDetector->GetDetector("EXOGAM"))->SetRawDataPointer(Exogam->GetExogamData());

}

////////////////////////////////////////////////////////////////////////////////
GUser::~GUser()  {
  
   if( NPOptionManager::getInstance()->GetOutputFile()!=NPOptionManager::getInstance()->GetDefaultOutputFile()){
    gROOT->cd();
    RootOutput::getInstance()->Destroy();
  }
}
////////////////////////////////////////////////////////////////////////////////
void GUser::InitUser(){
  // Initialisation for global  user treatement
cout<<  "- ---------< Init User  >------------------!\n";
  // Init the NPTool Part
  Init(fNPToolArgument);

  // Add correlation Spectra
  int NBins = 2000;
  int MinBin = 0;
  int MaxBin = 2000;
  TH2I* MUST_CATS = new TH2I("MUST_CATS","MUST_CATS",NBins,MinBin,MaxBin,NBins,MinBin,MaxBin);
  GetSpectra()->AddSpectrum(MUST_CATS,"Correlation");

  TH2I* MUST_TIARA = new TH2I("MUST_TIARA","MUST_TIARA",NBins,MinBin,MaxBin,NBins,MinBin,MaxBin);
  GetSpectra()->AddSpectrum(MUST_TIARA,"Correlation");

  TH2I* MUST_CHARISSA = new TH2I("MUST_CHARISSA","MUST_CHARISSA",NBins,MinBin,MaxBin,NBins,MinBin,MaxBin);
  GetSpectra()->AddSpectrum(MUST_CHARISSA,"Correlation");

  TH2I* MUST_EXO = new TH2I("MUST_EXO","MUST_EXO",NBins,MinBin,MaxBin,NBins,MinBin,MaxBin);
  GetSpectra()->AddSpectrum(MUST_EXO,"Correlation");

  TH2I* CATS_TIARA = new TH2I("CATS_TIARA","CATS_TIARA",NBins,MinBin,MaxBin,NBins,MinBin,MaxBin);
  GetSpectra()->AddSpectrum(CATS_TIARA,"Correlation");

  TH2I* CATS_CHARISSA = new TH2I("CATS_CHARISSA","CATS_CHARISSA",NBins,MinBin,MaxBin,NBins,MinBin,MaxBin);
  GetSpectra()->AddSpectrum(CATS_CHARISSA,"Correlation");

  TH2I* CATS_EXO = new TH2I("CATS_EXO","CATS_EXO",NBins,MinBin,MaxBin,NBins,MinBin,MaxBin);
  GetSpectra()->AddSpectrum(CATS_EXO,"Correlation");


}
////////////////////////////////////////////////////////////////////////////////
void GUser::InitUserRun(){
  // Initialisation for user treatemeant for each  run  
  // For specific user treatement
  cout << "Init User Run" << endl;
  
  fDetectorManager->Init(GetEvent()->GetDataParameters());

  // Sadly this as to be done by hand here for the Modular label:
 if(fTheTree) 
   fDetectorManager-> GetDetector("ModularLabel")->InitBranch(fTheTree);
 
  // keep track of read labels
  ofstream out_rej,out_acc;
  out_acc.open("label_accepted.dat");
  out_rej.open("label_rejected.dat");
  // Simulate an event loop with all label and value 0
  for (Int_t i = 0; i < GetEvent()->GetDataParameters()->GetNbParameters(); i++) {
    bool included = false;
    
    if (fDetectorManager->Is(GetEvent()->GetDataParameters()->GetLabel(i),0)) 
      included = true;
    
    
    if (!included) 
      out_rej << i <<" "<<GetDataParameters()->GetParName(i)<<endl;
    else 
      out_acc << i <<" "<<GetDataParameters()->GetParName(i)<<endl;
  }

  out_rej.close();
  out_acc.close();
cout << "End Init User Run" << endl ;
}
////////////////////////////////////////////////////////////////////////////////
void GUser::User(){ 
  // clear objects
  fDetectorManager -> Clear();

  //////////////////////////////////////////////////
  //     Unpack events & fill raw data objects    //
  //////////////////////////////////////////////////
  int mySize =  GetEventArrayLabelValueSize()/2;
  for (Int_t i = 0; i < mySize; i++) 
    fDetectorManager->Is(GetEventArrayLabelValue_Label(i),GetEventArrayLabelValue_Value(i));


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //        Call BuildPhysicalEvent (physical treatment) for each declared detector in the detector.txt file      //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  fMyDetector->BuildPhysicalEvent();
  // Fill the Physics Tree
  if( NPOptionManager::getInstance()->GetOutputFile()!=NPOptionManager::getInstance()->GetDefaultOutputFile())  { 
    RootOutput::getInstance()->GetTree()->Fill(); 
}
  /////////////////////////////////////////////////////////////////////////////
  // Fill the Correlation Histo
  TModularLabel* ModularLabel = (TModularLabel*) fDetectorManager->GetDetector("ModularLabel");
  short must = ModularLabel->GetValue("EXO_MUST");
  short exogam = ModularLabel->GetValue("EXO_EXO");
  short cats = ModularLabel->GetValue("EXO_CATS");
  short tiara = ModularLabel->GetValue("EXO_TIARA");
  short charissa = ModularLabel->GetValue("EXO_CHARISSA");

  GetSpectra()->GetHisto("MUST_CATS")->Fill(must,cats);
  GetSpectra()->GetHisto("MUST_TIARA")->Fill(must,tiara);
  GetSpectra()->GetHisto("MUST_CHARISSA")->Fill(must,charissa);
  GetSpectra()->GetHisto("MUST_EXO")->Fill(must,exogam);
  
  GetSpectra()->GetHisto("CATS_TIARA")->Fill(cats,tiara);
  GetSpectra()->GetHisto("CATS_CHARISSA")->Fill(cats,charissa);
  GetSpectra()->GetHisto("CATS_EXO")->Fill(cats,exogam);


}
////////////////////////////////////////////////////////////////////////////////
void GUser::EndUserRun(){
  cout <<"--------------< End User Run >------------------\n";

}
////////////////////////////////////////////////////////////////////////////////
void GUser::EndUser(){
  cout <<"--------------< End User 1> ------------------\n";
}
////////////////////////////////////////////////////////////////////////////////
void GUser::InitTTreeUser(){
  cout << "GUser::InitTTreeUser()" << endl;
  cout << "GUser::InitTTreeUser() -> fTheTree " << fTheTree << endl;

  fTheTree->Branch("RunNumber",&fRunNumber,"RunNumber/I");
  fTheTree->Branch("EvtNumber",&fEventCount,"EvtNumber/I");

  fDetectorManager-> InitBranch(fTheTree);

  cout << "End GUser::InitTTreeUser()" << endl;
}
