{
  gROOT->Reset();

  // load GUser
  TString GRUpath = gSystem->Getenv("GRUDIR");
  gROOT->ProcessLine(Form(".include %s/include", GRUpath.Data()));
  gROOT->ProcessLine(".L ./GUser_C.so"); // load and compile GUser class 

  // open data file to read
//  GTape *file = new GTape("../e628_run/run_1064.dat.19Dec13_09h17m14s"); 
//  GTape *file = new GTape("../e628_run/run_1067.dat.19Dec13_10h08m35s"); 
// GTape *file = new GTape("../e628_run/run_1133.dat.20Dec13_21h11m12s");
// GTape *file = new GTape("../e628_run/run_1168.dat.22Dec13_00h51m18s"); 
 
<<<<<<< HEAD
  //GTape *file = new GTape("../e628_run/run_1115.dat.20Dec13_00h38m48s");  
  GTape *file = new GTape("/data/e628X/e628/acquisition/run/run_1127.dat.20Dec13_15h25m33s.1");
=======
  GTape *file = new GTape("../e628_run/run_1170.dat.22Dec13_02h46m12s");

>>>>>>> 9de24afb867ba4b195001e14204fc44b347ea434

 file->Open();
  file->Rewind();
  // define GUser
  GUser *a = new GUser(file,"-D ./detector.txt  -C calibration.txt -O testGRU2");
  a->InitUser();
  a->EventInit();
<<<<<<< HEAD
  a->InitUserRun();
  a->SetTTreeMode(3, "test1127.root");
=======
  a->SetTTreeMode(3, "run_test2.root");
>>>>>>> 9de24afb867ba4b195001e14204fc44b347ea434
  // Set compression of the Output File
  // 0 extremely large file, no CPU use, high I/O latency so bad perf
  // 1 minimum compression, low on CPU (Recommanded by ROOT)
  // 2-9 bigger and bigger CPU load, smaller file
  a->SetCompressionLevel(1);

  // define GNetServerRoot to be able to connect vigru
  GNetServerRoot *serv = new GNetServerRoot(9090, a);
  serv->StartServer();
 
  
  // Convert  Run //
  a->DoRun();
  
  // Close every thing, save spectra // 
  file->Close();
  a->EndUser();              
<<<<<<< HEAD
//  a->SpeSave("histo.root");
  //serv->StopServer();
=======
  a->SpeSave("histo2.root");
  serv->StopServer();
>>>>>>> 9de24afb867ba4b195001e14204fc44b347ea434

  // Delete all object // 
  delete a;
  //delete serv;  
<<<<<<< HEAD
  delete file;                        
}
=======
//  delete file;                        

>>>>>>> 9de24afb867ba4b195001e14204fc44b347ea434
