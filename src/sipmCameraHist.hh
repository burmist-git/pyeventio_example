#pragma once

//root
#include <TObject.h>
#include <TH2Poly.h>
#include <TGraph.h>
#include <TVector2.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TLine.h>

//c, c++
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <iomanip>

/*
//namespace line_info{
struct line_flower_info {
  Float_t x;
  Float_t y;
    Float_t phi;
    line_flower_info(){
      x = -999.0;
      y = -999.0;
      phi = -999.0;
    }  
    static void print_info_header(){
      std::cout<<std::setw(15)<<"x"
	       <<std::setw(15)<<"y"
	       <<std::setw(15)<<"phi"
	       <<std::endl;
    }
    void print_info(){
      std::cout<<std::setw(15)<<x
	       <<std::setw(15)<<y
	       <<std::setw(15)<<phi
	       <<std::endl;
    }
    static void print_info(const line_flower_info a){
      std::cout<<std::setw(15)<<a.x
	       <<std::setw(15)<<a.y
	       <<std::setw(15)<<a.phi
	       <<std::endl;
    }
    static void print_info(const std::vector<line_flower_info> &a){
      print_info_header();
      for( unsigned int i = 0; i < a.size(); i++)
	print_info(a.at(i));
    }
    static void bubbleSortB(std::vector<line_flower_info> &a){
      bool swapp = true;
      while(swapp){
	swapp = false;
	for( unsigned int i = 0; i < a.size()-1; i++){
	  if (a.at(i).phi>a.at(i+1).phi){
	    //
	    line_flower_info tmp;
	    tmp = a.at(i);
	    //
	    a.at(i) = a.at(i+1);
	    a.at(i+1) = tmp;
	    swapp = true;
	  }
	}
      }
    }
};
//}
*/

struct pixel_neighbors_info {
  Int_t pixel_id;
  Float_t distance_between_pixels;
  Float_t x;
  Float_t y;
  Float_t x0;
  Float_t y0;
  Float_t phi;
  Float_t phi_deg;
  pixel_neighbors_info(){
    pixel_id = -999;
    distance_between_pixels = -999.0;
    x = -999.0;
    y = -999.0;
    x0 = -999.0;
    y0 = -999.0;
    phi = -999.0;
    phi_deg = -999.0;
  }
  static void print_info_header(){
    std::cout<<std::setw(15)<<"pixel_id"
	     <<std::setw(15)<<"x"
      	     <<std::setw(15)<<"y"
	     <<std::setw(15)<<"dist_btw_pix"
      	     <<std::setw(15)<<"phi_deg"
	     <<std::endl;
  }
  void print_info(){
    std::cout<<std::setw(15)<<pixel_id
	     <<std::setw(15)<<x
      	     <<std::setw(15)<<y
	     <<std::setw(15)<<distance_between_pixels
      	     <<std::setw(15)<<phi_deg
	     <<std::endl;
  }
};

struct pixel_info {
  //
  Int_t pixel_id;
  Float_t x;
  Float_t y;
  Int_t drawer_id;
  //
  const Int_t n = 7;
  Double_t *xp = new Double_t[n];
  Double_t *yp = new Double_t[n];
  std::vector<pixel_neighbors_info> v_pixel_neighbors;
  std::vector<pixel_neighbors_info> v_pixel_neighbors_second;
  std::vector<pixel_neighbors_info> v_pixel_neighbors_third;
  std::vector<pixel_neighbors_info> v_pixel_flower;
  std::vector<pixel_neighbors_info> v_pixel_super_flower;
  std::vector<TLine> v_line_flower;
  pixel_neighbors_info self_neighbors_info;
  //
  pixel_info(){
    //
    pixel_id = -999;
    x = -999.0;
    y = -999.0;
    drawer_id = -999;
    //
    for(Int_t i = 0;i<n;i++){
      xp[i] = -999.0;
      yp[i] = -999.0;
    }
    //
  }
  void print_info(){
    std::cout<<std::setw(10)<<pixel_id
	     <<std::setw(10)<<x
      	     <<std::setw(10)<<y
	     <<std::setw(10)<<drawer_id
	     <<std::endl;
  }
  static void print_info_header(){
    std::cout<<std::setw(10)<<"pixel_id"
	     <<std::setw(10)<<"x"
      	     <<std::setw(10)<<"y"
	     <<std::setw(10)<<"drawer_id"
	     <<std::endl;
  }
  void rotatePix(Double_t rot_alpha_deg){
    if(rot_alpha_deg != 0.0){
      TVector2 v(x,y);
      x = v.Rotate(rot_alpha_deg/180.0*TMath::Pi()).X();
      y = v.Rotate(rot_alpha_deg/180.0*TMath::Pi()).Y();
    }
  }
  void build_Cell(Int_t cell_type_id, Double_t l, Float_t x_v, Float_t y_v){
    if(cell_type_id == 0){
      Double_t alpha   = 2.0*TMath::Pi()/6.0;
      Double_t alpha_2 = alpha/2.0;
      Double_t alpha0  = alpha_2;
      Double_t l_2 = l/2.0;
      Double_t r = l_2/TMath::Cos(alpha_2);
      Double_t theta = 0.0;
      for(Int_t i = 0;i<n;i++){
	theta = alpha0 + alpha*i;
	xp[i] = r*TMath::Cos(theta) + x_v;
	yp[i] = r*TMath::Sin(theta) + y_v;
      }
    }
    else{
      std::cout<<"  ---> ERROR : cell_type_id = "<<cell_type_id<<std::endl;
      assert(0);
    }
  }
  void build_Cell(Int_t cell_type_id, Double_t l){
    build_Cell(cell_type_id, l, x, y);
  }
  void find_pixel_neighbors( const std::vector<pixel_info> &pixel_vec, double pixel_pitch){
    find_pixel_neighbors( pixel_vec, pixel_pitch, NULL);
  }
  void find_pixel_neighbors( const std::vector<pixel_info> &pixel_vec, double pixel_pitch, TH1D *h1_distance_between_pixels){
    //
    self_neighbors_info.pixel_id = pixel_id;
    self_neighbors_info.distance_between_pixels = 0.0;
    self_neighbors_info.x = x;
    self_neighbors_info.y = y;
    self_neighbors_info.x0 = 0.0;
    self_neighbors_info.y0 = 0.0;
    self_neighbors_info.phi = 0.0;
    self_neighbors_info.phi_deg = 0.0;
    //    
    //std::vector<pixel_neighbors_info> v_pixel_neighbors;
    Float_t tollerance = 0.3;
    Float_t dl = pixel_pitch*tollerance;
    Float_t distance_between_pixels = 0.0;
    for( unsigned int i = 0; i < pixel_vec.size(); i++){
      //for( unsigned int i = 0; i < 10; i++){
      distance_between_pixels = get_dist_pixel(pixel_vec.at(i).x, pixel_vec.at(i).y);
      if(h1_distance_between_pixels != NULL)
	h1_distance_between_pixels->Fill(distance_between_pixels);
      if(distance_between_pixels>(pixel_pitch-dl) && distance_between_pixels<(pixel_pitch+dl)){
	//std::cout<<"distance_between_pixels  = "<<distance_between_pixels<<std::endl
	//	 <<"pixel_vec.at(i).pixel_id = "<<pixel_vec.at(i).pixel_id<<std::endl
	//	 <<"pixel_vec.at(i).x        = "<<pixel_vec.at(i).x<<std::endl
	//	 <<"pixel_vec.at(i).y        = "<<pixel_vec.at(i).y<<std::endl
	//	 <<"pixel_id                 = "<<pixel_id<<std::endl
	//	 <<"x                        = "<<x<<std::endl
	//	 <<"y                        = "<<y<<std::endl;	
	pixel_neighbors_info pix_neighb_inf;
	pix_neighb_inf.pixel_id = pixel_vec.at(i).pixel_id;
	pix_neighb_inf.x = pixel_vec.at(i).x;
	pix_neighb_inf.y = pixel_vec.at(i).y;
	pix_neighb_inf.distance_between_pixels = distance_between_pixels;
	pix_neighb_inf.x0 = pixel_vec.at(i).x - x;
	pix_neighb_inf.y0 = pixel_vec.at(i).y - y;
	TVector2 nei_v(pix_neighb_inf.x0,pix_neighb_inf.y0);
	pix_neighb_inf.phi = nei_v.Phi();
	pix_neighb_inf.phi_deg = pix_neighb_inf.phi*180.0/TMath::Pi();
	v_pixel_neighbors.push_back(pix_neighb_inf);
	v_pixel_flower.push_back(pix_neighb_inf);
      }      
      if(distance_between_pixels>(2*pixel_pitch-dl) && distance_between_pixels<(2*pixel_pitch+dl)){
	pixel_neighbors_info pix_neighb_inf;
	pix_neighb_inf.pixel_id = pixel_vec.at(i).pixel_id;
	pix_neighb_inf.x = pixel_vec.at(i).x;
	pix_neighb_inf.y = pixel_vec.at(i).y;
	pix_neighb_inf.distance_between_pixels = distance_between_pixels;
	pix_neighb_inf.x0 = pixel_vec.at(i).x - x;
	pix_neighb_inf.y0 = pixel_vec.at(i).y - y;
	TVector2 nei_v(pix_neighb_inf.x0,pix_neighb_inf.y0);
	pix_neighb_inf.phi = nei_v.Phi();
	pix_neighb_inf.phi_deg = pix_neighb_inf.phi*180.0/TMath::Pi();
	v_pixel_neighbors_second.push_back(pix_neighb_inf);
      }
      if(distance_between_pixels>0.062 && distance_between_pixels<0.074){
	pixel_neighbors_info pix_neighb_inf;
	pix_neighb_inf.pixel_id = pixel_vec.at(i).pixel_id;
	pix_neighb_inf.x = pixel_vec.at(i).x;
	pix_neighb_inf.y = pixel_vec.at(i).y;
	pix_neighb_inf.distance_between_pixels = distance_between_pixels;
	pix_neighb_inf.x0 = pixel_vec.at(i).x - x;
	pix_neighb_inf.y0 = pixel_vec.at(i).y - y;
	TVector2 nei_v(pix_neighb_inf.x0,pix_neighb_inf.y0);
	pix_neighb_inf.phi = nei_v.Phi();
	pix_neighb_inf.phi_deg = pix_neighb_inf.phi*180.0/TMath::Pi();
	v_pixel_neighbors_third.push_back(pix_neighb_inf);
      }
    }
    bubbleSort(v_pixel_neighbors_third);
  }
  void build_pixel_super_flower( const std::vector<pixel_info> &pixel_vec){
    for( unsigned int i = 0; i < v_pixel_flower.size(); i++){
      pixel_neighbors_info pix_neighb_inf = v_pixel_flower.at(i);
      v_pixel_super_flower.push_back(pix_neighb_inf);
    }
    std::vector<unsigned int> super_flower_pixel_v;
    //get_flower_pixel_v(super_flower_pixel_v);
    //get_flower_pixel_v(super_flower_pixel_v,true);
    get_flower_pixel_v(super_flower_pixel_v, false);
    for( unsigned int i = 0; i < super_flower_pixel_v.size(); i++){      
      pixel_neighbors_info pix_neighb_inf = pixel_vec.at(super_flower_pixel_v.at(i)).self_neighbors_info;
      v_pixel_super_flower.push_back(pix_neighb_inf);
      for( unsigned int j = 0; j < pixel_vec.at(super_flower_pixel_v.at(i)).v_pixel_flower.size(); j++){
	pixel_neighbors_info pix_neighb_inf = pixel_vec.at(super_flower_pixel_v.at(i)).v_pixel_flower.at(j);
	v_pixel_super_flower.push_back(pix_neighb_inf);
      }
    }

  static void print_info(const std::vector<line_flower_info> &a){
    print_info_header();
    for( unsigned int i = 0; i < a.size(); i++)
      print_info(a.at(i))
  }
  static void bubbleSort(std::vector<line_flower_info> &a){
    
    
  }
  Float_t get_dist_pixel(Float_t px, Float_t py){
    return TMath::Sqrt((px - x)*(px - x) + (py - y)*(py - y));
  }
  static void bubbleSort(std::vector<pixel_neighbors_info> &a){
    bool swapp = true;
    while(swapp){
      swapp = false;
      for( unsigned int i = 0; i < a.size()-1; i++){
	if (a.at(i).phi_deg>a.at(i+1).phi_deg){
	  //
	  pixel_neighbors_info tmp;
	  tmp = a.at(i);
	  //
	  a.at(i) = a.at(i+1);
	  a.at(i+1) = tmp;
	  swapp = true;
	}
      }
    }
  }
  static Float_t find_min_dist(const std::vector<pixel_neighbors_info> a){
    Float_t min_dist = 999.0;
    for( unsigned int i = 0; i < a.size(); i++)
      if(a.at(i).distance_between_pixels<min_dist)
	min_dist = a.at(i).distance_between_pixels;
    return min_dist;
  }
  //void get_flower_pixel_v(std::vector<unsigned int> &super_flower_pixel_v){
  //get_flower_pixel_v(super_flower_pixel_v, true);
  //}
  void get_flower_pixel_v(std::vector<unsigned int> &super_flower_pixel_v, Bool_t first_yes = true){
    //super_flower_pixel_v.push_back(13);
    //super_flower_pixel_v.push_back(15);
    //super_flower_pixel_v.push_back(23);
    //super_flower_pixel_v.push_back(31);
    //super_flower_pixel_v.push_back(39);
    //super_flower_pixel_v.push_back(47);
    Float_t min_dist = find_min_dist(v_pixel_neighbors_third);
    Float_t phi_0_deg = -999.9;
    Int_t counter = 0;
    for( unsigned int i = 0; i < v_pixel_neighbors_third.size(); i++){
      if(v_pixel_neighbors_third.at(i).distance_between_pixels>=min_dist*0.99 &&
	 v_pixel_neighbors_third.at(i).distance_between_pixels<=min_dist*1.01){
	if(first_yes){
	  if(counter == 0)
	    phi_0_deg = v_pixel_neighbors_third.at(i).phi_deg;
	}
	else{
	  if(counter == 1)
	    phi_0_deg = v_pixel_neighbors_third.at(i).phi_deg;
	}
	counter++;
      }
    }
    counter = 0;
    for( unsigned int i = 0; i < v_pixel_neighbors_third.size(); i++){
      Float_t phi_tmp = v_pixel_neighbors_third.at(i).phi_deg - phi_0_deg;
      if((phi_tmp>= 60.0*counter-1.0) && (phi_tmp<= 60.0*counter+1.0)){
	super_flower_pixel_v.push_back(v_pixel_neighbors_third.at(i).pixel_id);
	counter++;
      }
    }
  }
  void get_flower_contour_lines(const std::vector<pixel_info> &pixel_vec, double pixel_pitch){
    Double_t rp_tmp;
    /*
    std::vector<line_info::line_flower_info> v_line_flower_points_info;
    for( unsigned int i = 0; i < pixel_vec.at(0).v_pixel_flower.size(); i++){
      for(Int_t j = 0;j<n;j++){
	rp_tmp = TMath::Sqrt(pixel_vec.at(0).v_pixel_flower.at(i).xp[i]*pixel_vec.at(0).v_pixel_flower.at(i).xp[j] +
			     pixel_vec.at(0).v_pixel_flower.at(i).yp[i]*pixel_vec.at(0).v_pixel_flower.at(i).yp[j]);
	if(rp_tmp>pixel_pitch){
	  line_info::line_flower_info line_str;
	  line_str.x = pixel_vec.at(0).v_pixel_flower.at(i).xp[i]*pixel_vec.at(0).v_pixel_flower.at(i).xp[j];
	  line_str.y = pixel_vec.at(0).v_pixel_flower.at(i).xp[i]*pixel_vec.at(0).v_pixel_flower.at(i).yp[j];
	  TVector2 v(line_str.x,line_str.y);
	  line_str.phi = v.Phi();
	  v_line_flower_points_info.push_back(line_str);
	}
      }
    }
    //std::vector<TLine> v_line_flower;    
    //assert(0);
    */
  }
};

class sipmCameraHist: public TH2Poly {

public:
  
  sipmCameraHist(const char* name, const char* title, const char* mapping_csv_file, Double_t rot_alpha_deg);
  sipmCameraHist(const char* name, const char* title, const char* mapping_csv_file, Double_t rot_alpha_deg, TH1D *h1_distance_between_pixels);
  ~sipmCameraHist();
  void dump_mapping_info();
  void test();
  void test02();
  void test03();
  void test04();
  void test05();
  void test_drawer_id();
  void test_pixel_neighbors_id();
  void test_pixel_neighbors_id(Int_t pix_id);
  void test_pixel_neighbors_id(Int_t npixels_n, Int_t *pix_id);
  void test_pixel_neighbors_second_id();
  void test_pixel_neighbors_second_id(Int_t pix_id);
  void test_pixel_neighbors_second_id(Int_t npixels_n, Int_t *pix_id);
  void test_pixel_neighbors_third_id();
  void test_pixel_neighbors_third_id(Int_t pix_id);
  void test_pixel_neighbors_third_id(Int_t npixels_n, Int_t *pix_id);
  void test_pixel_super_flower();
  void test_pixel_super_flower(Int_t pix_id);
  void test_pixel_super_flower(Int_t npixels_n, Int_t *pix_id);
  void test_pixel_neighbors_bubbleSort(Int_t pix_id);
  void Clean();
  void count_signal(Double_t th_val, Int_t &nch, Int_t &npe);
  void Draw_cam(TString settings, TString pdf_out_file);
  void Draw_cam(TString settings,
		TString pdf_out_file,
		TString particle_type,
		Int_t wf_time_id,
		Int_t   event_id,
		Float_t energy,
		Float_t xcore,
		Float_t ycore,
		Float_t ev_time,
		Int_t nphotons,
		Int_t n_pe,
		Int_t n_pixels);
  //  
  TString _name;
  TString _title;
  
 private:

  static const unsigned int _n_pixels = 7987;
  static const unsigned int _n_drawers = 1141;
  double _pixel_size;
  double _pixel_pitch;
  std::vector<pixel_info> _pixel_vec;
  void load_mapping(const char* mapping_csv_file);
  Double_t _rot_alpha_deg;
  
};
