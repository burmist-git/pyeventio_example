#ifndef ana_hh
#define ana_hh

//My
#include "anabase.hh"

//root
#include <TROOT.h>

//C, C++
#include <vector>

class TChain;
class TFile;
class TTree;
class TString;
class TBranch;
class TGraph;

class ana: public anabase {
public:
  
  ana(TString fileList) : anabase(fileList)
  {
  }

  ana(TString file, Int_t key) : anabase(file, key)
  {
  }

  void Loop(TString histOut);
  void print_ev_info(Long64_t jentry, Int_t max_pix_info, Int_t chid);
  void print_ev_info(Long64_t jentry);
  static void load_Template(TString file_name, TGraph *gr, Double_t t_max_shift, Double_t ampl, Double_t pedestal);
  static void load_spe(TString file_name, TGraph *gr, TH1D *h1, Double_t &Prompt_max, Double_t &Ampl_Prompt_max);
  void generate_gif_for_event(TString pathPref);
  void save_wf_for_event(TString histOut, Long64_t evID);
  void save_data_for_luca(std::vector<TGraph*> &v_gr, std::vector<TGraph*> &v_gr_sim, Float_t fadc_sample_in_ns);
  void write_wf_to_file(TString data_out, TGraph *gr, Float_t fadc_sample_in_ns);
  
};

#endif
