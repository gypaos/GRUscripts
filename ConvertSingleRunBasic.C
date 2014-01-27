// This macro converts a single run producing a ROOT output file
// with user class format
{
  gROOT->Reset();
  
  TString GRUpath = gSystem->Getenv("GRUDIR");
  gROOT->ProcessLine(Form(".include %s/include", GRUpath.Data()));
  gROOT->ProcessLine(".L ./GUser_C.so"); // load and compile GUser class
                                         //  gSystem->Load("./GUser_convert_C.so"); // load and compile GUser class
  
  GTape *file = new GTape("../e628_run/run_1020.dat.16Dec13_18h43m51s");
  file->Open();
  
  string input_file(file->GetDeviceName());
  int runnumber = atoi((input_file.substr(input_file.find("_")+1,input_file.find(".dat.") - input_file.find("_")-1)).c_str());
  
  GUser * a= new GUser(file);
  a->EventInit();
  a->SetSpectraMode(1);
  a->SetUserMode(1);
  file->Rewind();
  
  //  a->SetTTreeMode(1, "/scratch/gypaos/data/testCOFEE/root/run_0002_test.root");
  //  a->SetTTreeMode(3, "/data/e628X/e628/acquisition/run_root/run_0035_2.root");
  a->SetTTreeMode(3, "run_1020.root");
  cout << "======== Debug START DoRun() =========" << endl;
  a->SetRunNumber(runnumber);
  a->DoRun();
  cout << "======== Debug END DoRun() =========" << endl;
  
  
  //  file->Close();
  a->EndUser();                       // must be explicitly called , if it needs
 // a->SpeSave("hist_1007.root");   // save all declared histogram
                                     //  delete (a);                         // finish
  
  // gROOT->ProcessLine(".q");
}
