//my
#include "ana.hh"
#include "sipmCameraHist.hh"

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

void ana::load_spe(TString file_name, TGraph *gr, TH1D *h1, Double_t &Prompt_max, Double_t &Ampl_Prompt_max){
  cout<<file_name<<endl;
  ifstream fFile(file_name.Data());
  string mot;
  Double_t Ampl;
  Double_t Prompt;
  Ampl_Prompt_max = 0.0;
  Prompt_max = 0.0;
  Double_t Prompt_x_AP;
  Double_t Ampl_min;
  Double_t Ampl_max;
  Double_t d_Ampl;
  //
  if(fFile.is_open()){
    while(fFile>>mot){
      if(mot == "AP)")
	break;
    }
    fFile>>mot>>mot>>mot>>mot;
    while(fFile>>Ampl>>Prompt>>Prompt_x_AP){
      gr->SetPoint(gr->GetN(),Ampl,Prompt);
      if(Prompt_max<Prompt){
	Prompt_max = Prompt;
	Ampl_Prompt_max = Ampl;
      }
    }
    fFile.close();
  }
  //
  gr->GetPoint(0,Ampl_min,Prompt);
  gr->GetPoint((gr->GetN()-1),Ampl_max,Prompt);
  d_Ampl = (Ampl_max - Ampl_min)/(gr->GetN()-1);
  //
  h1->SetBins(gr->GetN(),Ampl_min-d_Ampl/2.0,Ampl_max+d_Ampl/2.0);
  //
  for(Int_t i = 0;i<gr->GetN();i++){
    gr->GetPoint(i,Ampl,Prompt);
    h1->SetBinContent(i+1,Prompt);
  }
}

void ana::load_Template(TString file_name, TGraph *gr, Double_t t_max_shift, Double_t ampl, Double_t pedestal){
  cout<<file_name<<endl;
  ifstream fFile(file_name.Data());
  string mot;
  Double_t t,a;
  Double_t tnew, anew;
  Double_t t_max,a_max;
  t_max=0.0;
  a_max=0.0;
  if(fFile.is_open()){
    while(fFile>>mot){
      if(mot == "[A.U.]")
	break;
    }
    while(fFile>>t>>a){
      gr->SetPoint(gr->GetN(),t,a);
      if(a_max<a){
	t_max=t;
	a_max=a;
      }
    }
    fFile.close();
  }
  for(Int_t i = 0;i<gr->GetN();i++){
    gr->GetPoint( i, t, a);
    tnew = t + (t_max_shift - t_max);
    anew = pedestal + a*ampl/a_max;
    gr->SetPoint( i, tnew, anew);
  }
}

void ana::generate_gif_for_event(TString pathPref){
  TString ev_dir_name = pathPref;
  ev_dir_name += event_id; 
  mkdir(ev_dir_name.Data(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  //std::vector<sipmCameraHist*> sipm_cam_v; 
  ofstream merge_gif;
  TString merge_gif_name=ev_dir_name;
  merge_gif_name += "/mrg.sh";
  //cout<<"merge_gif_name "<<merge_gif_name<<endl;
  merge_gif.open(merge_gif_name.Data());
  //
  merge_gif<<"convert -delay 15 -loop 1000 ";
  for(Int_t i = 20;i<(nn_fadc_point-20);i++){
    TString sipm_cam_name = "sipm_cam_";
    TString gif_name = ev_dir_name;
    gif_name += "/sipm_cam_";
    gif_name += i;
    gif_name += ".gif";
    TString gif_name_short = "sipm_cam_";
    gif_name_short += i;
    gif_name_short += ".gif";
    sipm_cam_name += i;
    sipmCameraHist *sipm_cam = new sipmCameraHist(sipm_cam_name.Data(),sipm_cam_name.Data(),"pixel_mapping.csv",-10);
    sipm_cam->SetMinimum(0.0);
    sipm_cam->SetMaximum(TMath::Power(2,14));
    for(Int_t j = 0;j<nChannels;j++){
      sipm_cam->SetBinContent(j+1,wf[j][i]);
    }
    merge_gif<<gif_name_short<<" ";
    sipm_cam->Draw_cam("ZCOLOR",gif_name.Data(),"gamma",i,event_id,energy,xcore,ycore,ev_time,nphotons,n_pe,n_pixels);
    //
    delete sipm_cam;
  }
  TString outtotGifName = "sipm_cam_ev";
  outtotGifName += event_id;
  outtotGifName += ".gif";
  merge_gif<<  outtotGifName.Data();
  merge_gif.close();
}

void ana::Loop(TString histOut){
  //
  //sipmCameraHist *sipm_cam = new sipmCameraHist("sipm_cam","sipm_cam","pixel_mapping.csv",-10);
  //sipmCameraHist *sipm_cam = new sipmCameraHist("sipm_cam","sipm_cam","pixel_mapping.csv",0);
  //sipm_cam->dump_mapping_info();
  //sipm_cam->test();
  //sipm_cam->test_drawer_id();
  //sipm_cam->test02();
  //sipm_cam->test03();
  //
  //assert(0);
  //
  TGraph *gr_template = new TGraph();
  gr_template->SetNameTitle("gr_template","gr_template");
  load_Template("Template.txt", gr_template,36.1,8.0,300.0);
  //
  TGraph *gr_spe = new TGraph();
  gr_spe->SetNameTitle("gr_spe","gr_spe");
  TH1D *h1_spe = new TH1D();
  h1_spe->SetNameTitle("h1_spe","h1_spe");
  Double_t Prompt_max;
  Double_t Ampl_Prompt_max;
  load_spe("spe.dat", gr_spe, h1_spe, Prompt_max, Ampl_Prompt_max);
  //
  cout<<"Prompt_max      = "<<Prompt_max<<endl
      <<"Ampl_Prompt_max = "<<Ampl_Prompt_max<<endl;
  //
  Int_t fadc_sum_offset=15;
  //Float_t fadc_amplitude=8.25;
  Int_t fadc_MHz=1024;
  Float_t fadc_sample_in_ns=1000.0/fadc_MHz;
  Float_t time_offset=fadc_sum_offset*fadc_sample_in_ns;
  //
  Float_t fadc_pedestal = 300;
  //Int_t fadc_bins=nn_fadc_point;
  //
  Float_t wf_time_arr[nn_fadc_point];
  for(Int_t j = 0;j<nn_fadc_point;j++){
    wf_time_arr[j] = j*fadc_sample_in_ns;
  }
  //
  vector<TGraph*> v_gr_0pe;
  vector<TGraph*> v_gr_1pe;
  vector<TGraph*> v_gr_2pe;
  vector<TGraph*> v_gr_3pe;
  //
  TGraph *gr_0pe_av = new TGraph();
  gr_0pe_av->SetNameTitle("gr_0pe_av","gr_0pe_av");
  TGraph *gr_1pe_av = new TGraph();
  gr_1pe_av->SetNameTitle("gr_1pe_av","gr_1pe_av");
  TGraph *gr_2pe_av = new TGraph();
  gr_2pe_av->SetNameTitle("gr_2pe_av","gr_2pe_av");
  TGraph *gr_3pe_av = new TGraph();
  gr_3pe_av->SetNameTitle("gr_3pe_av","gr_3pe_av");
  //
  Int_t n_wf_samples = 1000;
  //
  //TGraph *gr_wf[nChannels];
  //tGraphInit(gr_wf, "gr_wf", "gr_wf");
  //
  TH1D *h1_n_pe = new TH1D("h1_n_pe","h1_n_pe",1000,0.0,1000000);
  TH1D *h1_chID = new TH1D("h1_chID","h1_chID",nChannels,-0.5,(nChannels-1)+0.5);
  TH1D *h1_vVal = new TH1D("h1_vVal","h1_vVal",10000,-0.5,10000);
  //
  TH1D *h1_pe_time_shift = new TH1D("h1_pe_time_shift","h1_pe_time_shift",1000,-1.0,nn_fadc_point);
  TH1D *h1_pe_time_shift_cut_0pe = new TH1D("h1_pe_time_shift_cut_0pe","h1_pe_time_shift_cut_0pe",1000,-1.0,nn_fadc_point);
  TH1D *h1_pe_time_shift_cut_xpe = new TH1D("h1_pe_time_shift_cut_xpe","h1_pe_time_shift_cut_xpe",1000,-1.0,nn_fadc_point);
  TH1D *h1_pe_time_shift_cut_1pe = new TH1D("h1_pe_time_shift_cut_1pe","h1_pe_time_shift_cut_1pe",1000,-1.0,nn_fadc_point);  
  TH1D *h1_pe_time_shift_cut_2pe = new TH1D("h1_pe_time_shift_cut_2pe","h1_pe_time_shift_cut_2pe",1000,-1.0,nn_fadc_point);
  TH1D *h1_pe_time_shift_cut_3pe = new TH1D("h1_pe_time_shift_cut_3pe","h1_pe_time_shift_cut_3pe",1000,-1.0,nn_fadc_point);  
  //
  TH1D *h1_d_v = new TH1D("h1_d_v","h1_d_v",80, -40.0,40.0);
  TH1D *h1_v = new TH1D("h1_v","h1_v",400, -200.0,200.0);  
  //
  Int_t chID_arr[nChannels];
  //
  Float_t pe_time_shift;
  //
  Int_t iCH = 0;
  //
  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"nentries = "<<nentries<<endl;
  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    if(jentry%100 == 0)
      cout<<jentry<<endl;
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    //if (ientry > 10) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;
    //
    h1_n_pe->Fill(n_pe);
    //////////////////
    if(n_pe>10000){
      //if(energy<0.05){
      generate_gif_for_event("./ev_");
    }
    //////////////////
    for(Int_t j = 0;j<nChannels;j++)
      chID_arr[j] = 0;
    for(Int_t j = 0;j<n_pe;j++){
      h1_chID->Fill(pe_chID[j]);
      //
      chID_arr[pe_chID[j]] = chID_arr[pe_chID[j]] + 1;
      //
      pe_time_shift=pe_time[j] - ev_time + time_offset;
      h1_pe_time_shift->Fill(pe_time_shift);
      //
    }
    for(Int_t i = 0;i<nChannels;i++){
      for(Int_t j = 0;j<nn_fadc_point;j++){
	h1_vVal->Fill(wf[i][j]);
      }
    }
    //////////////////////
    //
    //0 p.e. wf
    for(Int_t j = 0;j<nChannels;j++){
      if(v_gr_0pe.size()<(unsigned int)n_wf_samples){
	if(chID_arr[j] == 0){
	  //
	  TGraph *gr0pe = new TGraph();
	  TString nameTitle0pe = "gr0pe_";
	  nameTitle0pe += v_gr_0pe.size();
	  gr0pe->SetNameTitle(nameTitle0pe.Data(),nameTitle0pe.Data());
	  for( Int_t ifadc = 0; ifadc<nn_fadc_point; ifadc++)
	    gr0pe->SetPoint(ifadc,wf_time_arr[ifadc],wf[j][ifadc]);
	  v_gr_0pe.push_back(gr0pe);
	  //
	  if(v_gr_0pe.size()==1)
	    for( Int_t ifadc = 0; ifadc<nn_fadc_point; ifadc++)
	      gr_0pe_av->SetPoint(ifadc,wf_time_arr[ifadc],0);
	  //
	}
      }
    }
    //////////////////////
    //
    for(Int_t j = 0;j<n_pe;j++){
      pe_time_shift=pe_time[j] - ev_time + time_offset;
      iCH = pe_chID[j];
      if(chID_arr[iCH] == 0){
	h1_pe_time_shift_cut_0pe->Fill(pe_time_shift);
      }
      if(v_gr_1pe.size()<(unsigned int)n_wf_samples){
	if((chID_arr[iCH] == 1) && (pe_time_shift>34.6) && (pe_time_shift<34.8)){
	  h1_pe_time_shift_cut_1pe->Fill(pe_time_shift);
	  //
	  TGraph *gr1pe = new TGraph();
	  TString nameTitle1pe = "gr1pe_";
	  nameTitle1pe += v_gr_1pe.size();
	  gr1pe->SetNameTitle(nameTitle1pe.Data(),nameTitle1pe.Data());
	  for( Int_t ifadc = 0; ifadc<nn_fadc_point; ifadc++)
	    gr1pe->SetPoint(ifadc,wf_time_arr[ifadc],wf[iCH][ifadc]);
	  v_gr_1pe.push_back(gr1pe);
	  //
	  if(v_gr_1pe.size()==1)
	    for( Int_t ifadc = 0; ifadc<nn_fadc_point; ifadc++)
	      gr_1pe_av->SetPoint(ifadc,wf_time_arr[ifadc],0);
	  //
	}
      }
      if(v_gr_2pe.size()<(unsigned int)n_wf_samples){
	if(chID_arr[iCH] == 2){
	  h1_pe_time_shift_cut_2pe->Fill(pe_time_shift);
	  //
	  TGraph *gr2pe = new TGraph();
	  TString nameTitle2pe = "gr2pe_";
	  nameTitle2pe += v_gr_2pe.size();
	  gr2pe->SetNameTitle(nameTitle2pe.Data(),nameTitle2pe.Data());
	  for( Int_t ifadc = 0; ifadc<nn_fadc_point; ifadc++)
	    gr2pe->SetPoint(ifadc,wf_time_arr[ifadc],wf[iCH][ifadc]);
	  v_gr_2pe.push_back(gr2pe);
	  //
	  if(v_gr_2pe.size()==1)
	    for( Int_t ifadc = 0; ifadc<nn_fadc_point; ifadc++)
	      gr_2pe_av->SetPoint(ifadc,wf_time_arr[ifadc],0);
	  //
	}
      }
      if(v_gr_3pe.size()<(unsigned int)n_wf_samples){
	if(chID_arr[iCH] == 3){
	  h1_pe_time_shift_cut_3pe->Fill(pe_time_shift);
	  //
	  TGraph *gr3pe = new TGraph();
	  TString nameTitle3pe = "gr3pe_";
	  nameTitle3pe += v_gr_3pe.size();
	  gr3pe->SetNameTitle(nameTitle3pe.Data(),nameTitle3pe.Data());
	  for( Int_t ifadc = 0; ifadc<nn_fadc_point; ifadc++)
	    gr3pe->SetPoint(ifadc,wf_time_arr[ifadc],wf[iCH][ifadc]);
	  v_gr_3pe.push_back(gr3pe);
	  //
	  if(v_gr_3pe.size()==1)
	    for( Int_t ifadc = 0; ifadc<nn_fadc_point; ifadc++)
	      gr_3pe_av->SetPoint(ifadc,wf_time_arr[ifadc],0);
	  //
	}
      }
    }
    //////////////////////
    //if(n_pe>380000){
    //
    //
    //for(Int_t i = 0;i<nChannels;i++){
    //h1_vVal->Fill(wf[i][j]);
    //gr_wf[i]->SetPoint( gr_wf[i]->GetN(), j, wf[i][j]);
    //}
    //}
    //}
    // print_ev_info( jentry, 100, 0);
  }
  //
  Double_t t;
  Double_t a;
  Double_t a_tot;
  // 0 p.e. --------------
  for(unsigned int ii = 0; ii<v_gr_0pe.size();ii++){
    for( Int_t ifadc = 0; ifadc<nn_fadc_point; ifadc++){
      v_gr_0pe.at(ii)->GetPoint(ifadc,t,a);
      gr_0pe_av->GetPoint(ifadc,t,a_tot);
      gr_0pe_av->SetPoint(ifadc,t,a + a_tot);
    }
  }
  for( Int_t ifadc = 0; ifadc<nn_fadc_point; ifadc++){
    gr_0pe_av->GetPoint(ifadc,t,a_tot);
    gr_0pe_av->SetPoint(ifadc,t,a_tot/v_gr_0pe.size());
  }
  // 1 p.e. --------------
  for(unsigned int ii = 0; ii<v_gr_1pe.size();ii++){
    for( Int_t ifadc = 0; ifadc<nn_fadc_point; ifadc++){
      v_gr_1pe.at(ii)->GetPoint(ifadc,t,a);
      gr_1pe_av->GetPoint(ifadc,t,a_tot);
      gr_1pe_av->SetPoint(ifadc,t,a + a_tot);
    }
  }
  for( Int_t ifadc = 0; ifadc<nn_fadc_point; ifadc++){
    gr_1pe_av->GetPoint(ifadc,t,a_tot);
    gr_1pe_av->SetPoint(ifadc,t,a_tot/v_gr_1pe.size());
  }
  // 2 p.e. --------------
  for(unsigned int ii = 0; ii<v_gr_2pe.size();ii++){
    for( Int_t ifadc = 0; ifadc<nn_fadc_point; ifadc++){
      v_gr_2pe.at(ii)->GetPoint(ifadc,t,a);
      gr_2pe_av->GetPoint(ifadc,t,a_tot);
      gr_2pe_av->SetPoint(ifadc,t,a + a_tot);
    }
  }
  for( Int_t ifadc = 0; ifadc<nn_fadc_point; ifadc++){
    gr_2pe_av->GetPoint(ifadc,t,a_tot);
    gr_2pe_av->SetPoint(ifadc,t,a_tot/v_gr_1pe.size());
  }
  // 3 p.e. --------------
  for(unsigned int ii = 0; ii<v_gr_3pe.size();ii++){
    for( Int_t ifadc = 0; ifadc<nn_fadc_point; ifadc++){
      v_gr_3pe.at(ii)->GetPoint(ifadc,t,a);
      gr_3pe_av->GetPoint(ifadc,t,a_tot);
      gr_3pe_av->SetPoint(ifadc,t,a + a_tot);
    }
  }
  for( Int_t ifadc = 0; ifadc<nn_fadc_point; ifadc++){
    gr_3pe_av->GetPoint(ifadc,t,a_tot);
    gr_3pe_av->SetPoint(ifadc,t,a_tot/v_gr_1pe.size());
  }
  //----------------------
  //
  Double_t t1;
  Double_t a1;
  Double_t t2;
  Double_t a2;
  for(unsigned int ii = 0; ii<v_gr_0pe.size();ii++){
    for( Int_t ifadc = 1; ifadc<nn_fadc_point; ifadc++){
      v_gr_0pe.at(ii)->GetPoint(ifadc-1,t1,a1);
      v_gr_0pe.at(ii)->GetPoint(ifadc,t2,a2);
      h1_d_v->Fill(a2-a1);
    }
    for( Int_t ifadc = 0; ifadc<nn_fadc_point; ifadc++){
      v_gr_0pe.at(ii)->GetPoint(ifadc,t,a);
      h1_v->Fill(a - fadc_pedestal);
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
  //for(Int_t i = 0;i<nChannels;i++){
  //gr_wf[i]->Write();
  //}
  cout<<"v_gr_0pe.size() "<<v_gr_0pe.size()<<endl;
  for(unsigned int ii = 0; ii<v_gr_0pe.size();ii++){
    v_gr_0pe.at(ii)->Write();
  }
  cout<<"v_gr_1pe.size() "<<v_gr_1pe.size()<<endl;
  for(unsigned int ii = 0; ii<v_gr_1pe.size();ii++){
    v_gr_1pe.at(ii)->Write();
  }
  cout<<"v_gr_2pe.size() "<<v_gr_2pe.size()<<endl;
  for(unsigned int ii = 0; ii<v_gr_2pe.size();ii++){
    v_gr_2pe.at(ii)->Write();
  }
  cout<<"v_gr_3pe.size() "<<v_gr_3pe.size()<<endl;
  for(unsigned int ii = 0; ii<v_gr_3pe.size();ii++){
    v_gr_3pe.at(ii)->Write();
  }
  //
  h1_vVal->Write();
  h1_chID->Write();
  h1_n_pe->Write();
  //
  //h1_pe_time_shift_cut_0pe->Write();
  h1_pe_time_shift_cut_1pe->Write();
  h1_pe_time_shift_cut_2pe->Write();
  h1_pe_time_shift_cut_3pe->Write();
  //
  gr_0pe_av->Write();
  gr_1pe_av->Write();
  gr_2pe_av->Write();
  gr_3pe_av->Write();
  //
  h1_pe_time_shift->Write();
  //
  gr_template->Write();
  gr_spe->Write();
  h1_spe->Write();
  //
  h1_d_v->Write();
  h1_v->Write();
  //
  rootFile->Close();
}

void ana::print_ev_info(Long64_t jentry, Int_t max_pix_info, Int_t chid){
  cout<<"---------------------------------------"<<endl;
  cout<<"jentry  "<<jentry<<endl;
  cout<<"event_id "<<event_id<<endl
      <<"energy   "<<energy<<endl
      <<"xcore    "<<xcore<<endl
      <<"ycore    "<<ycore<<endl
      <<"ev_time  "<<ev_time<<endl
      <<"nphotons "<<nphotons<<endl
      <<"n_pe     "<<n_pe<<endl
      <<"n_pixels "<<n_pixels<<endl;
  Int_t n = n_pe;
  n = (n_pe <= max_pix_info ? n : max_pix_info);
  //
  //cout<<" n            "<<n<<endl
  //<<" n_pe         "<<n_pe<<endl
  //<<" max_pix_info "<<max_pix_info<<endl;
  //
  cout<<"pe_chID : ";
  for(Int_t i = 0;i<n;i++)
    cout<<pe_chID[i]<<" ";
  cout<<endl;
  cout<<"pe_time : ";
  for(Int_t i = 0;i<n;i++)
    cout<<pe_time[i]<<" ";
  cout<<endl;
  cout<<"wf      : ";
  for(Int_t i = 0;i<nn_fadc_point;i++)
    cout<<wf[chid][i]<<" ";
  cout<<endl;
}
