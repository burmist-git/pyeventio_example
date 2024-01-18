//my
#include "anaTrgA.hh"
#include "ana.hh"
#include "anabase.hh"
#include "sipmCameraHist.hh"
#include "wfCamSim.hh"
#include "triggerSim.hh"
#include "evstHist.hh"
#include "dbscan.hh"

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
#include <TRandom3.h>
#include <TVector3.h>

//C, C++
#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <fstream>
#include <iomanip>
#include <time.h>
#include <bits/stdc++.h>
#include <sys/stat.h>

using namespace std;

Double_t anaTrgA::get_theta_p_t( const TVector3 &v_det, Double_t altitude_v, Double_t azimuth_v){
  TVector3 v_prot;
  v_prot.SetMagThetaPhi(1.0,TMath::Pi()/2.0-altitude_v,TMath::Pi() - azimuth_v);
  TVector3 v_prot_inv(v_prot.x(),v_prot.y(),v_prot.z());
  return TMath::ACos(v_prot_inv.Dot(v_det)/v_prot_inv.Mag()/v_det.Mag());
}

void anaTrgA::transform_SiPM_distadd( TGraph *gr_tr, const TGraph *gr){
  Double_t x,y;
  Double_t x_new,y_new; 
  Double_t ymax = 0.0;
  for(Int_t i = 0;i<gr->GetN();i++){
    gr->GetPoint(i,x,y);
    if(y>ymax)
      ymax = y;
  }
  Double_t x_min = -5.0*1.0e+6;
  Double_t x_max = 30.0*1.0e+6;
  Int_t nn = 1000;
  for(Int_t i = 0;i<nn;i++){
    //gr->GetPoint(i,x,y);
    x_new = x_min + (x_max - x_min)/(nn-1)*i;
    x = x_new/(1.0/0.920148*1.0e+6*7.5);
    y = gr->Eval(x);
    //x_new=x*1.0/0.920148*1.0e+6*7.5;
    y_new=y/ymax+20.0*TMath::Gaus(x_new,0.0,1.0e+6*7.5*0.12);
    y_new *= 0.0016;
    gr_tr->SetPoint(i,x_new,y_new);
  }
}

void anaTrgA::SiPM_dist(TString histOut){
  //
  const unsigned int nn_PMT_channels = nChannels;
  Int_t fadc_sum_offset = 15;
  Int_t fadc_MHz = 1024;
  Int_t fadc_offset = 300;
  Float_t fadc_sample_in_ns = 1000.0/fadc_MHz;
  Float_t time_offset = fadc_sum_offset*fadc_sample_in_ns;
  Float_t NGB_rate_in_MHz = 0.0;
  Float_t fadc_electronic_noise_RMS = 0.01;
  //
  TRandom3 *rnd = new TRandom3(123123);
  //
  vector<vector<Int_t>> wfcam(nn_PMT_channels, vector<Int_t>(nn_fadc_point));
  wfCamSim *wfc = new wfCamSim(rnd, "Template_CTA_SiPM.txt", "spe.dat",
			       nn_fadc_point, nn_PMT_channels, fadc_offset, fadc_sample_in_ns, NGB_rate_in_MHz, fadc_electronic_noise_RMS);
  wfc->print_wfCamSim_configure();
  //
  TGraph *gr_aml_gain_pedestal = new TGraph();
  transform_SiPM_distadd( gr_aml_gain_pedestal, wfc->get_gr_wf_ampl());
  //
  //----------------------
  TFile* rootFile = new TFile(histOut.Data(), "RECREATE", " Histograms", 1);
  rootFile->cd();
  if (rootFile->IsZombie()){
    cout<<"  ERROR ---> file "<<histOut.Data()<<" is zombi"<<endl;
    assert(0);
  }
  else
    cout<<"  Output Histos file ---> "<<histOut.Data()<<endl;
  //
  wfc->getTemplate()->Write();
  wfc->get_gr_wf_ampl()->Write();
  wfc->get_h1_wf_ampl_ADC()->Write();
  wfc->get_h1_wf_ampl()->Write();
  wfc->get_h1_adc_NGB_pedestal()->Write();
  wfc->get_h1_dadc_NGB_pedestal()->Write();
  //
  gr_aml_gain_pedestal->SetNameTitle("gr_aml_gain_pedestal","gr_aml_gain_pedestal");
  gr_aml_gain_pedestal->Write();
  //
  rootFile->Close();
}

void anaTrgA::Loop(TString histOut, Int_t binE, Int_t binTheta, Int_t binDist, Int_t npe_min, Int_t npe_max, Int_t nEv_max){
  //
  TVector3 v_det;
  v_det.SetXYZ(1.0*TMath::Sin(20.0/180.0*TMath::Pi()),0,1.0*TMath::Cos(20.0/180.0*TMath::Pi()));
  //
  evstHist *evH = new evstHist("evH","evH");
  evH->Print_hist_BinsInfo();
  evH->Print_hist_BinsInfo(binE, binTheta, binDist);
  //
  evH->get_Bin_Edge(evH->get_E_hist(),binE,_E_min, _E_max);
  evH->get_Bin_Edge(evH->get_theta_hist(),binTheta, _theta_deg_min, _theta_deg_max);
  evH->get_Bin_Edge(evH->get_v_r().at(0),binDist, _dist_min, _dist_max);
  //    
  _npe_min = npe_min;
  _npe_max = npe_max;
  _nEv_max = nEv_max;
  //
  print_cuts();
  //
  clock_t start, finish;
  clock_t start_trg, finish_trg;
  clock_t start_sim, finish_sim;
  start = clock();
  //
  TH1D *h1_digital_sum     = new TH1D("h1_digital_sum",     "h1_digital_sum",    1001,-0.5,1000.5);
  TH1D *h1_digital_sum_3ns = new TH1D("h1_digital_sum_3ns", "h1_digital_sum_3ns",1001,-0.5,1000.5);
  TH1D *h1_digital_sum_5ns = new TH1D("h1_digital_sum_5ns", "h1_digital_sum_5ns",1001,-0.5,1000.5);
  TH1D *h1_fadc_val        = new TH1D("h1_fadc_val",        "h1_fadc_val",       1001,-0.5,1000.5);
  //
  TH1D *h1_energy = new TH1D("h1_energy","h1_energy", 1000, 0.0, 100.0); 
  TH1D *h1_rcore = new TH1D("h1_rcore","h1_rcore", 1000, 0.0, 1000.0);
  TH1D *h1_theta_p_t_deg = new TH1D("h1_theta_p_t_deg","h1_theta_p_t_deg", 1000, 0.0, 10.0);
  TH1D *h1_npe = new TH1D("h1_npe","h1_npe", 1000, 0.0, 1000.0);
  //
  /////////////
  //
  //
  Double_t x0_LST01 = -70.93;
  Double_t y0_LST01 = -52.07;
  //
  Int_t nevsim = 0;
  //
  const unsigned int nn_PMT_channels = nChannels;
  Int_t fadc_sum_offset = 15;
  Int_t fadc_MHz = 1024;
  Int_t fadc_offset = 300;
  Float_t fadc_sample_in_ns = 1000.0/fadc_MHz;
  Float_t time_offset = fadc_sum_offset*fadc_sample_in_ns;
  //Float_t NGB_rate_in_MHz = 386.0;
  Float_t NGB_rate_in_MHz = 268.0;
  //Float_t NGB_rate_in_MHz = 0.0;
  //Float_t fadc_electronic_noise_RMS = 3.94;
  //Float_t fadc_electronic_noise_RMS = 0.01;
  Float_t fadc_electronic_noise_RMS = 3.8436441; //takes into account 3.0/sqrt(12)
  //
  TRandom3 *rnd = new TRandom3(123123);
  //
  ////////////////////////////////////  
  //static const Int_t nChannels = 7987;
  //static const Int_t nn_fadc_point = 75;
  //
  vector<vector<Int_t>> wfcam(nn_PMT_channels, vector<Int_t>(nn_fadc_point));
  //
  wfCamSim *wfc = new wfCamSim(rnd, "Template_CTA_SiPM.txt", "spe.dat",
			       nn_fadc_point, nn_PMT_channels, fadc_offset, fadc_sample_in_ns, NGB_rate_in_MHz, fadc_electronic_noise_RMS);
  wfc->print_wfCamSim_configure();
  //
  sipmCameraHist *sipm_cam = new sipmCameraHist("sipm_cam","sipm_cam","pixel_mapping.csv",0);
  triggerSim *trg_sim = new triggerSim(sipm_cam);    
  //
  finish = clock();
  cout<<"initialization time : "<<((finish - start)/CLOCKS_PER_SEC)<<" (sec)"<<endl;	
  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;
  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    //if(jentry%1000000 == 0)
    //cout<<jentry<<endl;
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry); nbytes += nb;
    //
    if(nevsim >= _nEv_max)
      break;
    //
    Double_t rcore = TMath::Sqrt((x0_LST01 - xcore)*(x0_LST01 - xcore) + (y0_LST01 - ycore)*(y0_LST01 - ycore));
    //
    Double_t theta_p_t = get_theta_p_t( v_det, altitude, azimuth);
    Double_t theta_p_t_deg = theta_p_t*180/TMath::Pi();
    //
    if(cut( nevsim, theta_p_t_deg, rcore)){
      h1_theta_p_t_deg->Fill(theta_p_t_deg);
      h1_npe->Fill(n_pe);
      h1_energy->Fill(energy*1000);
      h1_rcore->Fill(rcore);
      //
      start_sim = clock();
      wfc->simulate_cam_event(nn_fadc_point,
			      nn_PMT_channels,
			      wfcam,
			      ev_time,
			      time_offset,
			      n_pe,
			      pe_chID,
			      pe_time);
      finish_sim = clock();
      start_trg = clock();
      //
      trg_sim->get_trigger(wfcam,h1_digital_sum,h1_digital_sum_3ns,h1_digital_sum_5ns,h1_fadc_val);
      finish_trg = clock();
      cout<<nevsim
	  <<" "<<((finish_sim - start_sim)/(CLOCKS_PER_SEC/1000))<<" (msec)"
	  <<" "<<((finish_trg - start_trg)/(CLOCKS_PER_SEC/1000))<<" (msec)"<<endl;
      //
      nevsim++;
    }
  }
  //
  //----------------------
  TFile* rootFile = new TFile(histOut.Data(), "RECREATE", " Histograms", 1);
  rootFile->cd();
  if (rootFile->IsZombie()){
    cout<<"  ERROR ---> file "<<histOut.Data()<<" is zombi"<<endl;
    assert(0);
  }
  else
    cout<<"  Output Histos file ---> "<<histOut.Data()<<endl;
  //
  wfc->getTemplate()->Write();
  wfc->get_gr_wf_ampl()->Write();
  wfc->get_h1_wf_ampl_ADC()->Write();
  wfc->get_h1_wf_ampl()->Write();
  wfc->get_h1_adc_NGB_pedestal()->Write();
  wfc->get_h1_dadc_NGB_pedestal()->Write();
  //
  h1_digital_sum->Write();
  h1_digital_sum_3ns->Write();
  h1_digital_sum_5ns->Write();
  h1_fadc_val->Write();
  //
  h1_energy->Write();
  h1_rcore->Write();
  h1_theta_p_t_deg->Write();
  h1_npe->Write();
  //
  //cout<<"nevsim = "<<nevsim<<endl;
  //
  rootFile->Close();
}

Bool_t anaTrgA::cut( Int_t nevsim, Double_t theta_p_t_deg, Double_t rcore){
  if(nevsim<_nEv_max){
    if(energy>=_E_min/1000.0 && energy<_E_max/1000.0){
      if(rcore>=_dist_min && rcore<_dist_max){
	if(theta_p_t_deg>=_theta_deg_min && theta_p_t_deg<_theta_deg_max){
	  if(n_pe>=_npe_min && n_pe<_npe_max){
	    return true;
	  }
	}
      }
    }
  }
  return false;
}

const void anaTrgA::print_cuts(){
  cout<<"_E_min         "<<_E_min<<endl
      <<"_E_max         "<<_E_max<<endl
      <<"_theta_deg_min "<<_theta_deg_min<<endl
      <<"_theta_deg_max "<<_theta_deg_max<<endl
      <<"_dist_min      "<<_dist_min<<endl
      <<"_dist_max      "<<_dist_max<<endl
      <<"_npe_min       "<<_npe_min<<endl
      <<"_npe_max       "<<_npe_max<<endl
      <<"_nEv_max       "<<_nEv_max<<endl;
}