//my
#include "src/ana.hh"
#include "src/anashort.hh"

//root
#include "TROOT.h"
#include "TString.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TRandom3.h"
#include "TGraph.h"

//C, C++
#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <fstream>
#include <iomanip>
#include <time.h>

using namespace std;

int main(int argc, char *argv[]){
  if(argc == 4 && atoi(argv[1])==0){
    TString rootFilesList = argv[2];
    TString outRootFileF = argv[3];
    cout<<"--> Parameter calculation from the WF <--"<<endl
	<<"rootFilesList : "<<rootFilesList<<endl
	<<"outRootFileF  : "<<outRootFileF<<endl;
    ana a(rootFilesList);
    a.Loop(outRootFileF);
  }
  else if(argc == 4 && atoi(argv[1])==1){
    TString inRootFiles = argv[2];
    TString outRootFileF = argv[3];
    cout<<"--> Parameter calculation from the WF <--"<<endl
	<<"inRootFiles   : "<<inRootFiles<<endl
	<<"outRootFileF  : "<<outRootFileF<<endl;
    ana a( inRootFiles, atoi(argv[1]));
    a.Loop(outRootFileF);
  }
  else if(argc == 4 && atoi(argv[1])==2){
    TString rootFilesList = argv[2];
    TString outRootFileF = argv[3];
    cout<<"--> Parameter calculation from the WF <--"<<endl
	<<"rootFilesList : "<<rootFilesList<<endl
	<<"outRootFileF  : "<<outRootFileF<<endl;
    anashort a(rootFilesList);
    a.Loop(outRootFileF);
  }
  else if(argc == 4 && atoi(argv[1])==3){
    TString inRootFiles = argv[2];
    TString outRootFileF = argv[3];
    cout<<"--> Parameter calculation from the WF <--"<<endl
	<<"inRootFiles   : "<<inRootFiles<<endl
	<<"outRootFileF  : "<<outRootFileF<<endl;
    anashort a( inRootFiles, atoi(argv[1]));
    a.Loop(outRootFileF);
  }
  else if(argc == 5 && atoi(argv[1])==4){
    TString inRootFiles = argv[2];
    TString outRootFileF = argv[3];
    Long64_t evID = (Long64_t)atoi(argv[4]);
    cout<<"--> Parameter calculation from the WF <--"<<endl
	<<"inRootFiles   : "<<inRootFiles<<endl
	<<"outRootFileF  : "<<outRootFileF<<endl
      	<<"evID          : "<<evID<<endl;
    ana a(inRootFiles, 1);
    a.save_wf_for_event(outRootFileF, evID);
  }
  else{
    cout<<" --> ERROR in input arguments "<<endl
	<<" runID [1] = 0 (execution ID number)"<<endl
      	<<"       [2] - file with list of the root files"<<endl
	<<"       [3] - name of root file with histograms"<<endl;
    cout<<" runID [1] = 1 (execution ID number)"<<endl
      	<<"       [2] - in root files"<<endl
	<<"       [3] - name of root file with histograms"<<endl;
    cout<<" runID [1] = 2 (execution ID number) short file format"<<endl
      	<<"       [2] - file with list of the root files"<<endl
	<<"       [3] - name of root file with histograms"<<endl;
    cout<<" runID [1] = 3 (execution ID number) short file format"<<endl
      	<<"       [2] - in root files"<<endl
	<<"       [3] - name of root file with histograms"<<endl;
    cout<<" runID [1] = 4 (execution ID number) short file format"<<endl
      	<<"       [2] - in root files"<<endl
	<<"       [3] - name of root file with histograms"<<endl
    	<<"       [4] - evID"<<endl;
  }
  return 0;
}
