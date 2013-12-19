{
   gROOT->Reset();

   // load GUser
   TString GRUpath = gSystem->Getenv("GRUDIR");
   gROOT->ProcessLine(Form(".include %s/include", GRUpath.Data()));
   gROOT->ProcessLine(".L ./GUser_C.so"); // load and compile GUser class 

   // open data file to read
//   GTape *file = new GTape("../e628_run/run_1005.dat.13Dec13_17h38m10s"); 
//     GTape *file = new GTape("../e628_run/run_1007.dat.13Dec13_20h17m15s");
//   GTape *file = new GTape("../e628_run/run_1070.dat.19Dec13_12h29m00s");
   GTape *file = new GTape("../e628_run/run_1067.dat.19Dec13_10h08m35s"); 

   // define GUser
   GUser *a = new GUser(file);
   // define GNetServerRoot to be able to connect vigru
   GNetServerRoot *serv = new GNetServerRoot(9090, a);
   a->EventInit();
   a->SetSpectraMode(1);
   a->SetUserMode(1);
   // start spectra server for vigru
   serv->StartServer();
   file->Rewind();

   // convert run
   a->SetTTreeMode(3, "run_test.root");
   cout << "======== Debug START DoRun() =========" << endl;
   a->DoRun(10000);
   cout << "======== Debug END DoRun() =========" << endl;

//   file->Close();
//   a->EndUser();                       // must be explicitly called , if it needs
 a->SpeSave("histo.root");   // save all declared histogram 
//   delete (a);                         // finish 

   // gROOT->ProcessLine(".q");
}
