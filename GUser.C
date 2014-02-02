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

GUser::GUser (GDevice* _fDevIn, GDevice* _fDevOut){ 
  // Constructor/initialisator of Acquisition object 
  // entry:
  // - Input Device
  // - Output Device
  fDevIn   = _fDevIn;
  fDevOut  = _fDevOut;

  // instantiate detector objects
  fMust2         = new TMust2();
  fCATS          = new TCATS();
  fExogam        = new TExogam();
  fTrigger	      = new TTrigger();
  fTac           = new TTac();
  fPlastic       = new TPlastic();
  fLise          = new TLise();
  fTiaraHyball   = new TTiaraHyball();
  fTiaraBarrel   = new TTiaraBarrel();
  fCharissa      = new TCharissa();

  MySpectraList = GetSpectra();
  cout << "Spectra done" << endl;  
}



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



void GUser::InitUser(){
  // Initialisation for global  user treatement

  cout<<  "- ---------< Init User  >------------------!\n";
  cout << "+++++ Init NPOption Manager" << endl;
  // -GH for generating histos
  // -CH for checking histos
  // -C for calibration files
  // -O for outputting the Physical Tree (followed by its name)
  string argument = "-D ./detector.txt  -C calibration.txt -GH -O testGRU";
  NPOptionManager *myOptionManager = NPOptionManager::getInstance(argument);

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
}


void GUser::InitUserRun(){
  // Initialisation for user treatemeant for each  run  
  // For specific user treatement
  cout << "Init run" << endl;
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

  for (Int_t i = 0; i < GetEventArraySize(); i++) {
    bool included = true;

    if (fMust2->GetLabelMap(GetDataParameters()->GetLabel(i)) == GetDataParameters()->GetParName(i)){}
    else if (fCATS->GetLabelMap(GetDataParameters()->GetLabel(i)) == GetDataParameters()->GetParName(i)) {}
    else if (fExogam->GetLabelMap(GetDataParameters()->GetLabel(i)) == GetDataParameters()->GetParName(i)) {}
    else if(fTac->GetLabelMap(GetDataParameters()->GetLabel(i)) == GetDataParameters()->GetParName(i)) {}
    else if(fTrigger->GetLabelMap(GetDataParameters()->GetLabel(i)) == GetDataParameters()->GetParName(i)) {}
    else if(fLise->GetLabelMap(GetDataParameters()->GetLabel(i)) == GetDataParameters()->GetParName(i)) {}
    else if(fPlastic->GetLabelMap(GetDataParameters()->GetLabel(i)) == GetDataParameters()->GetParName(i)) {}
    else if(fTiaraHyball->GetLabelMap(GetDataParameters()->GetLabel(i)) == GetDataParameters()->GetParName(i)) {}
    else if(fTiaraBarrel->GetLabelMap(GetDataParameters()->GetLabel(i)) == GetDataParameters()->GetParName(i)) {}
    else if(fCharissa->GetLabelMap(GetDataParameters()->GetLabel(i)) == GetDataParameters()->GetParName(i)) {}
    else included = false;
   
    if (!included) 
      out_rej << i <<" "<<GetDataParameters()->GetParName(i)<<endl;
    else 
      out_acc << i <<" "<<GetDataParameters()->GetParName(i)<<endl;
  }

  out_rej.close();
  out_acc.close();
}



void GUser::User(){ 
  // clear objects
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

  map<int,int> label_rec;
  int mySize =  GetEventArrayLabelValueSize()/2;
  for (Int_t i = 0; i < mySize; i++) {
    label_rec[GetEventArrayLabelValue_Label(i)]+=1;
    if (fMust2->Is(GetEventArrayLabelValue_Label(i),GetEventArrayLabelValue_Value(i))) {}
    else if (fCATS->Is(GetEventArrayLabelValue_Label(i),GetEventArrayLabelValue_Value(i))) {}
    else if (fExogam->Is(GetEventArrayLabelValue_Label(i),GetEventArrayLabelValue_Value(i))) {}
    else if (fTrigger->Is(GetEventArrayLabelValue_Label(i),GetEventArrayLabelValue_Value(i))) {}
    else if (fPlastic->Is(GetEventArrayLabelValue_Label(i),GetEventArrayLabelValue_Value(i))) {}
    else if (fLise->Is(GetEventArrayLabelValue_Label(i),GetEventArrayLabelValue_Value(i))){} 
    else if (fTac->Is(GetEventArrayLabelValue_Label(i),GetEventArrayLabelValue_Value(i))) {}
    else if (fTiaraHyball->Is(GetEventArrayLabelValue_Label(i),GetEventArrayLabelValue_Value(i))) {}
    else if (fTiaraBarrel->Is(GetEventArrayLabelValue_Label(i),GetEventArrayLabelValue_Value(i))) {}
    else if (fCharissa->Is(GetEventArrayLabelValue_Label(i),GetEventArrayLabelValue_Value(i))) {}
  }
  /*
     bool trig= false;
     map<int,int>::iterator it ;
     for(it=label_rec.begin();it!=label_rec.end();++it)
     if(it->second>1&&it->first!=0) trig=true;

     if(trig){
     int ll = 0;
     cout << endl << "Wrong label" << endl ; 
     for(it=label_rec.begin();it!=label_rec.end();++it){
     if(it->second>1){ll++; cout << it->first  << " " << it->second <<"|" ;}
     if(ll%10==0)cout << endl ;
     }
     cout <<endl <<  "///////////////////////////////////" << endl; 
     for (Int_t i = 0; i < mySize; i++) {
     cout << GetEventArrayLabelValue_Label(i) << " "  ;
     if(i%20==0&&i!=0) cout << endl ;
     }
     }
     */
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //        Call BuildPhysicalEvent (physical treatment) for each declared detector in the detector.txt file      //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  fMyDetector->BuildSimplePhysicalEvent();

  // Fill the Physics Tree
  RootOutput::getInstance()->GetTree()->Fill(); 


}




void GUser::EndUserRun(){
  //  end of run ,  executed a end of each run
  cout <<"--------------< End User Run >------------------\n";

}

//______________________________________________________________
void GUser::EndUser(){
  // globlal final end executed a end of runs
  // must be explicitly called! 


  cout <<"--------------< End User 1> ------------------\n";

  //cout << "End save spectra " << endl;    

}
//______________________________________________________________________________

void GUser::InitTTreeUser(){
  cout << "GUser::InitTTreeUser()" << endl;
  cout << "GUser::InitTTreeUser() -> fTheTree " << fTheTree << endl;

  fTheTree->Branch("RunNumber",&fRunNumber,"RunNumber/I");
  fTheTree->Branch("EvtNumber",&fEventCount,"EvtNumber/I");

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
