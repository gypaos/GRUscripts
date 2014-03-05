// This macro converts a single run producing a ROOT output file
// with user class format
{
  gROOT->Reset();
  
  TString GRUpath = gSystem->Getenv("GRUDIR");
  gROOT->ProcessLine(Form(".include %s/include", GRUpath.Data()));
  gROOT->ProcessLine(".L ./GUser_C.so"); // load and compile GUser class
                                         //  gSystem->Load("./GUser_convert_C.so"); // load and compile GUser class
  
  GTape *file = new GTape("/data/e628X/e628/acquisition/run/run_1238.dat.26Feb14_21h26m57s");
  file->Open();
  
  string input_file(file->GetDeviceName());
  int runnumber = atoi((input_file.substr(input_file.find("_")+1,input_file.find(".dat.") - input_file.find("_")-1)).c_str());
  
  GUser * a= new GUser(file);
  a->EventInit();
  a->SetSpectraMode(1);
  a->SetUserMode(1);
  file->Rewind();
  
  //  a->SetTTreeMode(1, "/scratch/gypaos/data/testCOFEE/root/run_0002_test.root");
  a->SetTTreeMode(1, "/data/e628X/e628/acquisition/run_root/run_1238.root");
  //a->SetTTreeMode(3, "/data/e628X/e628/acquisition/run_root/run_1193.root");
  cout << "======== Debug START DoRun() =========" << endl;
  cout << " SHIT HAPPENS!!" << endl;
  a->SetRunNumber(runnumber);
  a->DoRun();
  cout << "======== Debug END DoRun() =========" << endl;
  
  
  //  file->Close();
  a->EndUser();                       // must be explicitly called , if it needs
 a->SpeSave("hist_1238.root");   // save all declared histogram
                                     //  delete (a);                         // finish
  
  // gROOT->ProcessLine(".q");
}
