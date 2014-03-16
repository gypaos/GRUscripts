{
  gROOT->Reset();

  // load GUse
  TString GRUpath = gSystem->Getenv("GRUDIR");
  gROOT->ProcessLine(Form(".include %s/include", GRUpath.Data()));
  gROOT->ProcessLine(".L ./GUser_C.so"); // load and compile GUser class

  // open data file to read
//  GTape *file = new GTape("../e628_run/run_1064.dat.19Dec13_09h17m14s"); 
//  GTape *file = new GTape("../e628_run/run_1067.dat.19Dec13_10h08m35s"); 
// GTape *file = new GTape("../e628_run/run_1133.dat.20Dec13_21h11m12s.2");
//GTape *file = new GTape("../run/run_1255.dat.27Feb14_21h41m00s");

// GTape *file = new GTape("../e628_run/run_1168.dat.22Dec13_00h51m18s"); 
 //GTape *file = new GTape("../run/run_1209.dat.24Feb14_19h24m07s");

  //GTape *file = new GTape("/data/e628X/e628/acquisition/run/run_1209.dat.24Feb14_19h24m07s");
  //GTape *file = new GTape("/data/elabodecX/adcval/acquisition/run/run_0011.dat.13Mar14_11h01m06s");
 //GTape *file = new GTape("/data/elabodecX/adcval/acquisition/run/run_0012.dat.13Mar14_11h06m00s");
  GTape *file = new GTape("/data/elabodecX/adcval/acquisition/run/run_0013.dat.13Mar14_11h07m43s");


  file->Open();
  file->Rewind();
  // define GUser
  GUser *a = new GUser(file,"-D ./detector.txt  -C calibration.txt -GH -O test");
  a->EventInit();
 // a->SetSpectraMode(1);
  a->InitUser();
  a->SetTTreeMode(3, "run_test.root");  
  // Set compression of the Output File
  // 0 extremely large file, no CPU use, high I/O latency so bad perf
  // 1 minimum compression, low on CPU (Recommanded by ROOT)
  // 2-9 bigger and bigger CPU load, smaller file
  a->SetCompressionLevel(1);

  // define GNetServerRoot to be able to connect vigru
 GNetServerRoot *serv = new GNetServerRoot(2001, a);
  serv->StartServer();
 

  // Convert  Run //
  a->DoRun(100000);
  
  // Close every thing, save spectra // 
  file->Close();
  a->EndUser();              
  a->SpeSave("histo.root");
  // Delete all object // 
  delete a;
  //delete serv;  
}
