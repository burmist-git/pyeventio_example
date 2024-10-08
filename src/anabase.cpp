//my
#include "anabase.hh"

//root
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TString.h>
#include <TChain.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TGraph.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TProfile.h>
#include <TVector2.h>

//C, C++
#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <fstream>
#include <iomanip>
#include <time.h>
#include <bits/stdc++.h>
#include <vector>

using namespace std;

anabase::anabase(TString fileList, Bool_t short_format_flag) : _particle_type_name("NONE"), _short_format_flag(short_format_flag), fChain(0), _n_data_chunks(20) 
{
  ifstream indata;
  TString rootFileName;
  //TChain *theChain = new TChain("arich");
  TChain *theChain = new TChain(treeName.Data());
  indata.open(fileList.Data()); 
  assert(indata.is_open());  
  while (indata  >> rootFileName ){
    if(indata.eof()){
      std::cout<<"EOF"<<std::endl;
      break;
    }
    cout<<"        adding "<<rootFileName<<endl;
    theChain->Add(rootFileName.Data(),-1);
  }
  indata.close();
  Init(theChain);
}

anabase::anabase(TString inFileName, Int_t keyID, Bool_t short_format_flag) : _particle_type_name("NONE"), _short_format_flag(short_format_flag), fChain(0), _n_data_chunks(20)
{
  if(keyID == 1){
    ifstream indata;
    //TChain *theChain = new TChain("T");
    TChain *theChain = new TChain(treeName.Data());
    cout<<"        adding "<<inFileName<<endl;
    theChain->Add(inFileName.Data(),-1);
    Init(theChain);
  }
  else
    assert(0);
}

anabase::anabase(TString fileList) : anabase(fileList, true) {}
anabase::anabase(TString inFileName, Int_t keyID) : anabase( inFileName, keyID, true) {}

void anabase::tGraphInit(TGraph *gr[nChannels], TString grName, TString grTitle){
  Int_t i;
  TString grNameh;
  TString grTitleh;
  for(i = 0;i<nChannels;i++){
    grNameh = grName; grNameh += "_"; grNameh += "ch_"; grNameh += i;
    grTitleh = grTitle; grTitleh += " "; grTitleh += "ch "; grTitleh += i;
    gr[i] = new TGraph();
    gr[i]->SetTitle(grTitleh.Data());
    gr[i]->SetName(grNameh.Data());
  }
}

void anabase::h1D1Init(TH1D *h1D1[nChannels],TString h1name, TString h1Title,
		      Int_t Nbin, Float_t Vmin, Float_t Vmax){
  Int_t i;
  TString h1nameh;
  TString h1Titleh;
  for(i = 0;i<nChannels;i++){    
    h1nameh = h1name; h1nameh += "_"; h1nameh += "ch_"; h1nameh += i;
    h1Titleh = h1Title; h1nameh += " "; h1Titleh += "ch "; h1Titleh += i;
    h1D1[i] = new TH1D(h1nameh.Data(), h1Titleh.Data(),
                       Nbin, Vmin, Vmax);
  }
}

void anabase::h1D1Init(vector<TH1D*> &h1D1_v, unsigned int n_len,
		       TString h1name, TString h1Title,
		       Int_t Nbin, Float_t Vmin, Float_t Vmax){
  unsigned int i;
  TString h1nameh;
  TString h1Titleh;
  for(i = 0;i<n_len;i++){    
    h1nameh = h1name; h1nameh += "_"; h1nameh += "ch_"; h1nameh += i;
    h1Titleh = h1Title; h1nameh += " "; h1Titleh += "ch "; h1Titleh += i;
    h1D1_v.push_back(new TH1D(h1nameh.Data(), h1Titleh.Data(), Nbin, Vmin, Vmax));
  }
}

void anabase::h2D2Init(TH2D *h2D1[nChannels],TString h2name, TString h2Title,
		      Int_t Nbin1, Float_t Vmin1, Float_t Vmax1,
		      Int_t Nbin2, Float_t Vmin2, Float_t Vmax2){
  Int_t i;
  TString h2nameh;
  TString h2Titleh;
  for(i = 0;i<nChannels;i++){    
    h2nameh = h2name; h2nameh += "_"; h2nameh += "ch_"; h2nameh += i;
    h2Titleh = h2Title; h2nameh += " "; h2Titleh += "ch "; h2Titleh += i;
    h2D1[i] = new TH2D(h2nameh.Data(), h2Titleh.Data(),
                       Nbin1, Vmin1, Vmax1,
		       Nbin2, Vmin2, Vmax2);
  }  
}

void anabase::h2D2Init(vector<TH2D*> &h2D1_v, unsigned int n_len,TString h2name, TString h2Title,
		      Int_t Nbin1, Float_t Vmin1, Float_t Vmax1,
		      Int_t Nbin2, Float_t Vmin2, Float_t Vmax2){
  unsigned int i;
  TString h2nameh;
  TString h2Titleh;
  for(i = 0;i<n_len;i++){    
    h2nameh = h2name; h2nameh += "_"; h2nameh += "ch_"; h2nameh += i;
    h2Titleh = h2Title; h2nameh += " "; h2Titleh += "ch "; h2Titleh += i;
    h2D1_v.push_back(new TH2D(h2nameh.Data(), h2Titleh.Data(),
			      Nbin1, Vmin1, Vmax1,
			      Nbin2, Vmin2, Vmax2));
  }  
}

void anabase::tProfInit(TProfile *tprof[nChannels],TString prname, TString prTitle,
		       Int_t Nbin, Float_t Vmin, Float_t Vmax){
  Int_t i;
  TString prnameh;
  TString prTitleh;
  for(i = 0;i<nChannels;i++){    
    prnameh = prname; prnameh += "_"; prnameh += "ch_"; prnameh += i;
    prTitleh = prTitle; prnameh += " "; prTitleh += "ch "; prTitleh += i;
    tprof[i] = new TProfile(prnameh.Data(), prTitleh.Data(), Nbin, Vmin, Vmax,"");
  }
}

void anabase::tProfInit(vector<TProfile*> &tprof_v, unsigned int n_len,
			TString prname, TString prTitle,
			Int_t Nbin, Float_t Vmin, Float_t Vmax){
  unsigned int i;
  TString prnameh;
  TString prTitleh;
  for(i = 0;i<n_len;i++){
    prnameh = prname; prnameh += "_"; prnameh += "ch_"; prnameh += i;
    prTitleh = prTitle; prnameh += " "; prTitleh += "ch "; prTitleh += i;
    tprof_v.push_back(new TProfile(prnameh.Data(), prTitleh.Data(), Nbin, Vmin, Vmax,""));
  }
}

double anabase::getUnixTimeFromTime(double d_year, double d_month, double d_day, double d_hour, double d_min, double d_sec){
  //time_t timer;
  struct tm y2k = {0};
  y2k.tm_year = d_year - 1900; y2k.tm_mon = d_month-1; y2k.tm_mday = d_day;
  y2k.tm_hour = d_hour;   y2k.tm_min = d_min; y2k.tm_sec = d_sec;
  return difftime(mktime(&y2k),0);
}

anabase::~anabase(){
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

void anabase::Loop(TString histOut){
}

Int_t anabase::GetEntry(Long64_t entry){
  // Read contents of entry.
  if (!fChain) return 0;
  return fChain->GetEntry(entry);
}

Long64_t anabase::LoadTree(Long64_t entry){
  // Set the environment to read one entry
  if (!fChain) return -5;
  Long64_t centry = fChain->LoadTree(entry);
  if (centry < 0) return centry;
  if (fChain->GetTreeNumber() != fCurrent) {
    fCurrent = fChain->GetTreeNumber();
    Notify();
  }
  return centry;
}

void anabase::Init(TTree *tree){
  // The Init() function is called when the selector needs to initialize
  // a new tree or chain. Typically here the branch addresses and branch
  // pointers of the tree will be set.
  // It is normally not necessary to make changes to the generated
  // code, but the routine can be extended by the user if needed.
  // Init() will be called many times when running on PROOF
  // (once per file to be processed).
  // Set branch addresses and branch pointers
  if (!tree) return;
  fChain = tree;
  fCurrent = -1;
  fChain->SetMakeClass(1);
  //fChain->SetBranchAddress("evt", &evt, &b_evt);
  //fChain->SetBranchAddress("run", &run, &b_run);
  //fChain->SetBranchAddress("pValue", &pValue, &b_pValue);
  //...
  //...
  //
  //---------------------------------------------------
  // ADD HERE :
  fChain->SetBranchAddress("event_id", &event_id, &b_event_id);
  fChain->SetBranchAddress("energy", &energy, &b_energy);
  //
  fChain->SetBranchAddress("azimuth", &azimuth, &b_azimuth);
  fChain->SetBranchAddress("altitude", &altitude, &b_altitude);
  fChain->SetBranchAddress("h_first_int", &h_first_int, &b_h_first_int);
  fChain->SetBranchAddress("xmax", &xmax, &b_xmax);
  fChain->SetBranchAddress("hmax", &hmax, &b_hmax);
  fChain->SetBranchAddress("emax", &emax, &b_emax);
  fChain->SetBranchAddress("cmax", &cmax, &b_cmax);
  //
  fChain->SetBranchAddress("xcore", &xcore, &b_xcore);
  fChain->SetBranchAddress("ycore", &ycore, &b_ycore);
  fChain->SetBranchAddress("ev_time", &ev_time, &b_ev_time);
  fChain->SetBranchAddress("nphotons", &nphotons, &b_nphotons);
  fChain->SetBranchAddress("n_pe", &n_pe, &b_n_pe);
  fChain->SetBranchAddress("n_pixels", &n_pixels, &b_n_pixels);
  fChain->SetBranchAddress("pe_chID", pe_chID, &b_pe_chID);
  fChain->SetBranchAddress("pe_time", pe_time, &b_pe_time);
  if(!_short_format_flag){
    std::cout<<"_short_format_flag "<<_short_format_flag<<std::endl;
    std::cout<<"                   fChain->SetBranchAddress(wf, wf, &b_wf) "<<std::endl;
    fChain->SetBranchAddress("wf", wf, &b_wf);
  }
  else{
    std::cout<<"_short_format_flag "<<_short_format_flag<<std::endl;
  }
  //---------------------------------------------------
  Notify();
}

Bool_t anabase::Notify(){
  // The Notify() function is called when a new file is opened. This
  // can be either for a new TTree in a TChain or when when a new TTree
  // is started when using PROOF. It is normally not necessary to make changes
  // to the generated code, but the routine can be extended by the
  // user if needed. The return value is currently not used.
  return kTRUE;
}

void anabase::Show(Long64_t entry){
  // Print contents of entry.
  // If entry is not specified, print current entry
  if (!fChain) return;
  fChain->Show(entry);
}

Int_t anabase::Cut(Long64_t entry){
  // This function may be called from Loop.
  // returns  1 if entry is accepted.
  // returns -1 otherwise.
  return 1;
}

void anabase::printEv(){
  cout<<"event_id        "<<event_id<<endl
      <<"energy          "<<energy<<endl
      <<"azimuth         "<<azimuth<<endl
      <<"altitude        "<<altitude<<endl
      <<"h_first_int     "<<h_first_int<<endl
      <<"xmax            "<<xmax<<endl
      <<"hmax            "<<hmax<<endl
      <<"emax            "<<emax<<endl
      <<"cmax            "<<cmax<<endl
      <<"xcore           "<<xcore<<endl
      <<"ycore           "<<ycore<<endl
      <<"ev_time         "<<ev_time<<endl
      <<"nphotons        "<<nphotons<<endl
      <<"n_pe            "<<n_pe<<endl
      <<"n_pixels        "<<n_pixels<<endl
      <<"pe_chID[0]      "<<pe_chID[0]<<endl
      <<"pe_time[0]      "<<pe_time[0]<<endl
      <<"pe_chID[n_pe-1] "<<pe_chID[n_pe-1]<<endl
      <<"pe_time[n_pe-1] "<<pe_time[n_pe-1]<<endl;
}

void anabase::TH2D_divide( TH2D *h2_w, TH2D *h2, TH2D *h2_norm){
  Double_t val;
  Double_t norm;
  Double_t val_norm;
  for(Int_t i = 1;i<=h2_w->GetNbinsX();i++){
    for(Int_t j = 1;j<=h2_w->GetNbinsY();j++){
      val = h2_w->GetBinContent(i,j);
      norm = h2->GetBinContent(i,j);
      if(norm>0)
	val_norm = val/norm;
      else
	val_norm = 0.0;
      h2_norm->SetBinContent(i,j,val_norm);
    }
  }
}

void anabase::TH1D_divide( TH1D *h1_w, TH1D *h1, TH1D *h1_norm){
  Double_t val;
  Double_t norm;
  Double_t val_norm;
  for(Int_t i = 1;i<=h1_w->GetNbinsX();i++){
    val = h1_w->GetBinContent(i);
    norm = h1->GetBinContent(i);
    if(norm>0)
      val_norm = val/norm;
    else
      val_norm = 0.0;
    h1_norm->SetBinContent(i,val_norm);
  }
}

void anabase::TH1D_divide( TH1D *h1, TH1D *h1_norm, Double_t norm){
  Double_t val;
  Double_t val_norm;
  for(Int_t i = 1;i<=h1->GetNbinsX();i++){
    val = h1->GetBinContent(i);
    if(norm>0)
      val_norm = val/norm;
    else
      val_norm = 0.0;
    h1_norm->SetBinContent(i,val_norm);
  }
}
  
void anabase::getCore_rel_R_theta(const Double_t x0, const Double_t y0, const Double_t xx, const Double_t yy, Double_t &rr, Double_t &theta){
  TVector2 vv(xx-x0,yy-y0);
  rr = vv.Mod();
  theta = vv.Phi();
}

Int_t anabase::get_current_data_chunk_ID(Long64_t nentries, Long64_t jentry){
  Int_t delta_n = (Double_t)nentries/(Double_t)_n_data_chunks;
  if(delta_n>0)
    _data_chunk_ID = (Int_t)jentry/delta_n;
  else
    _data_chunk_ID = 0;
  return _data_chunk_ID;
}
