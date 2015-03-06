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

#include "./GUser_convert.h"

// ROOT headers
#include "TROOT.h"
#include "TString.h"

// NPTOOL headers
#include "NPOptionManager.h"
#include "RootOutput.h"
// GANIL2ROOT
#include "TModularLabel.h" 

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
