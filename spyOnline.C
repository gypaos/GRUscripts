{  
   gROOT->Reset(); 

   // load GUser
   TString GRUpath = gSystem->Getenv("GRUDIR");
   gROOT->ProcessLine(Form(".include %s/include", GRUpath.Data()));
   gROOT->ProcessLine(".L ./GUser_C.so");   //load and compile GUser class 

   // connect to data flow
   GNetClientNarval *net = new GNetClientNarval("ganp166"); //
   net->SetPort(10202);  
   net->SetBufferSize(65536);

   // instantiate GUSer object
   GUser *a = new GUser(net,"-D ./detector.txt  -C calibration.txt -GH");       
   // get exp. name
   TString ExpName = gSystem->Getenv("USER");
   a->EventInit(ExpName.Data());// event initialisation 
   a->SetSpectraMode(1); 
   a->InitUser();// execute GUser::InitUser()
   
   // instantiate spectrum server for vigru
   GNetServerRoot *serv = new GNetServerRoot(9090, a);

   // start hist server
   serv->StartServer();
   a->DoRun();

   net->Close();                       

   a->EndUser();               // must be explicitly called , if it needs
   a->SpeSave("histo.root");   // save all declared histogram 
   delete (a);                 // finish 

   gROOT->ProcessLine(".q");
}


