{
  gROOT->Reset();

  // load GUser
  TString GRUpath = gSystem->Getenv("GRUDIR");
  gROOT->ProcessLine(Form(".include %s/include", GRUpath.Data()));
  gROOT->ProcessLine(".L ./GUser_C.so"); // load and compile GUser class 

  // open data file to read
//  GTape *file = new GTape("../e628_run/run_1067.dat.19Dec13_10h08m35s"); 
// GTape *file = new GTape("../e628_run/run_1147.dat.21Dec13_11h39m01s");
// GTape *file = new GTape("../e628_run/run_1168.dat.22Dec13_00h51m18s"); 
 
  //GTape *file = new GTape("../e628_run/run_1115.dat.20Dec13_00h38m48s");  
  GTape *file = new GTape("/data/e628X/e628/acquisition/run/run_1127.dat.20Dec13_15h25m33s.1");

 file->Open();
  file->Rewind();
  // define GUser
  GUser *a = new GUser(file);
  a->InitUser();
  a->EventInit();
  a->InitUserRun();
  a->SetTTreeMode(3, "test1127.root");
  // Set compression of the Output File
  // 0 extremely large file, no CPU use, high I/O latency so bad perf
  // 1 minimum compression, low on CPU (Recommanded by ROOT)
  // 2-9 bigger and bigger CPU load, smaller file
  a->SetCompressionLevel(1);

  // define GNetServerRoot to be able to connect vigru
//  GNetServerRoot *serv = new GNetServerRoot(9090, a);
//  serv->StartServer();
 
  
  // Convert  Run //
  a->DoRun();
  
  // Close every thing, save spectra // 
  file->Close();
  a->EndUser();              
//  a->SpeSave("histo.root");
  //serv->StopServer();

  // Delete all object // 
  delete a;
  //delete serv;  
  delete file;                        
}
