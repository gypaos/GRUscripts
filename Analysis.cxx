#include"Analysis.h"
#include<iostream>
#include<string>
#include<sstream>
using namespace std;

#include <dirent.h>

#include"GTape.h"
#include"GUser_convert.h"
int main (int argc, char** argv){
  if(argc!=3){
    cout << "Need two argument: $1:input file or directory $2: directory" << endl;
    exit(1);
  }

  string inputfile = argv[1];
  string outputdirectory = argv[2];

  if(inputfile.find(".dat.")!=string::npos){
    AnalyseSingleFile(inputfile,outputdirectory);
  }

  else{
    AnalyseDirectory(inputfile,outputdirectory,true);
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
void AnalyseSingleFile(string filename,string outdirectory){
  GTape *file = new GTape(filename.c_str());
  file->Open();

  // Test that output folder exist
  DIR *pDir;
  pDir = opendir(outdirectory.c_str());

  if (pDir != NULL){
    (void) closedir (pDir);
  }
  else{
    cout << "Creating folder " << outdirectory << endl;
    string commd = "mkdir " + outdirectory ;
    system(commd.c_str());
  }


  // Create the output file name base on the run number
  int runnumber_major,runnumber_minor;
  string outputfilename = outdirectory+"/"+FileNameToRootName(filename,runnumber_major,runnumber_minor);

  string NPToolArgument = "-D ./detector.txt  -C calibration.txt";

  GUser * a= new GUser(file,NPToolArgument);
  a->EventInit();
  a->SetSpectraMode(0);
  a->InitUser();
  file->Rewind();

  a->SetTTreeMode(TTreeMode::TREE_USER,outputfilename.c_str());
  a->SetRunNumber(runnumber_major);
  a->DoRun();
  a->EndUser();        
  // Add the root extension at the end
  // Allow to test if the file was completly converted
  string commd = "mv " + outputfilename + " " + outputfilename+ ".root" ;
  system(commd.c_str());
}

////////////////////////////////////////////////////////////////////////////////
void AnalyseDirectory(string directory_name,string outdir,bool check){
  DIR *inDir;
  inDir = opendir(directory_name.c_str());

  if (inDir == NULL){
    cout << "ERROR : Input directory does not exist" << endl;
    exit(1);
  }
  else{
    // Test that output folder exist
    DIR *outDir;
    outDir = opendir(outdir.c_str());
    string outlist= "";
    if (outDir == NULL){
      cout << "Creating output directory " << outdir<< endl;
      string commd = "mkdir " + outdir;
      system(commd.c_str());
    }
    else{
      struct dirent *ent;
      while ((ent = readdir (outDir)) != NULL) {
        outlist+=(string)ent->d_name+" ";
      }
    }


    // Make a list of file in the input dir
    string filename;
    struct dirent *ent;
    while ((ent = readdir (inDir)) != NULL) {
      string dname = ent->d_name;
      if(dname.find(".dat.")!=string::npos){
        filename= directory_name + dname ;
        // make the output file name:
        int M,m;
        string outname = FileNameToRootName(filename,M,m)+".root";
        if(outlist.find(outname)==string::npos){
          cout << filename << " " << outdir << endl;
          AnalyseSingleFile(filename,outdir);
        }
        else{
          cout << " File " << filename << " already converted, skipping" << endl;
        }
      }
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
string FileNameToRootName(string input_file,int& runnumber_major, int& runnumber_minor){
  // Create the output file name base on the run number
  while(input_file.find("/")!=string::npos){
    input_file =  input_file.substr(input_file.find("/")+1); 
  }

  runnumber_major = atoi((input_file.substr(input_file.find("_")+1,input_file.find(".dat.") - input_file.find("_")-1)).c_str());
  if(input_file.find_last_of("s")==input_file.length()-1)
    runnumber_minor=0;
  else
    runnumber_minor = atoi((input_file.substr(input_file.find_last_of(".")+1)).c_str());

  ostringstream naming ;
  naming << "run_"<< runnumber_major << "_" << runnumber_minor;
  return naming.str();
}
