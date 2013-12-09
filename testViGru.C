{
   gROOT->Reset();

   // load GUser
   TString GRUpath = gSystem->Getenv("GRUDIR");
   gROOT->ProcessLine(Form(".include %s/include", GRUpath.Data()));
   gROOT->ProcessLine(".L ./GUser_C.so"); // load and compile GUser class 

   // open data file to read
   GTape *file = new GTape("../e628_run/run_0026.dat.09Dec13_11h58m27s"); 

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
   a->DoRun();
   cout << "======== Debug END DoRun() =========" << endl;

//   file->Close();
//   a->EndUser();                       // must be explicitly called , if it needs
 a->SpeSave("histo.root");   // save all declared histogram 
//   delete (a);                         // finish 

   // gROOT->ProcessLine(".q");
}
