// This macro converts a single run producing a ROOT output file
// with user class format
{
  gROOT->Reset(); 
  
  TString GRUpath = gSystem->Getenv("GRUDIR");
  gROOT->ProcessLine(Form(".include %s/include", GRUpath.Data()));
//  gROOT->ProcessLine(".L ./GUser_convert.C++O"); // load and compile GUser class 
//  gROOT->ProcessLine(".L ./GUser_convert_C.so"); // load and compile GUser class 
  gROOT->ProcessLine(".L ./GUser_C.so"); // load and compile GUser class 
//  gSystem->Load("./GUser_convert_C.so"); // load and compile GUser class 
 
//  GTape *file = new GTape("/scratch/gypaos/data/testCOFEE/raw/run_0002.dat.13Apr12_14h09m19s");
//  GTape *file = new GTape("/scratch/gypaos/GanilData/e530/run/run_0363.dat.20Apr09_11h20m41s");
//  GTape *file = new GTape("/data/e628X/e628/acquisition/run/run_0035.dat.10Dec13_20h43m26s.2");
  GTape *file = new GTape("/data/e628X/e628/acquisition/run/run_1028.dat.17Dec13_09h30m43s");
//  GTape *file = new GTape("/data/e628X/e628/acquisition/run/run_1022.dat.16Dec13_19h21m07s");
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
  a->SetTTreeMode(3, "/data/e628X/e628/acquisition/run_root/run_1028.root");
//  a->SetTTreeMode(3, "/data/e628X/e628/acquisition/run_root/run_1022.root");
  cout << "======== Debug START DoRun() =========" << endl;
  a->SetRunNumber(runnumber);
  a->DoRun();
  cout << "======== Debug END DoRun() =========" << endl;

  
//  file->Close();
  a->EndUser();                       // must be explicitly called , if it needs
  a->SpeSave("run_1028.root");   // save all declared histogram 
  //a->SpeSave("./matchsticks/hyball-sectors_matchsticks.root");
//  delete (a);                         // finish 

  // gROOT->ProcessLine(".q");
}
