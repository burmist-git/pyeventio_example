#ifndef triggerSim_hh
#define triggerSim_hh

//my
#include "dbscan.hh"

//C, C++
#include <vector>
#include <array>

//root
#include <TROOT.h>

class sipmCameraHist;
class dbscan;
class TH1D;
class TH2D;
class TString;

class triggerSim {

public :
  //
  triggerSim(const sipmCameraHist* simphist);
  ~triggerSim();

  //std::vector<std::array<int, 2>> get_trigger(const std::vector<std::vector<int>> &wf);
  std::vector<std::vector<int>> get_trigger_test(const std::vector<std::vector<int>> &wf);
  std::vector<std::vector<int>> get_trigger_test();
  std::vector<std::vector<unsigned int>> get_trigger(const std::vector<std::vector<int>> &wf,
						     TH1D *h1_digital_sum = NULL,
						     TH1D *h1_digital_sum_3ns = NULL,
						     TH1D *h1_digital_sum_5ns = NULL,
						     TH1D *h1_fadc_val = NULL);
  static void print_trigger_vec(const std::vector<std::array<int, 2>> &trg_vector);
  static void print_trigger_vec(const std::vector<std::vector<unsigned int>> &trg_vector);
  static void print_trigger_vec_to_csv(const std::vector<std::vector<unsigned int>> &trg_vector, const sipmCameraHist *sipm_cam, TString out_file_name, bool if_short_format = true);

  void plot_and_save_to_hist_root(TString outrootFile, vector<Double_t> &k_dist_graph);

  inline const vector<cluster_info>& get_dbclusters() const {return _dbclusters_v;};

  void const fill_fadc_val_vs_time( const std::vector<std::vector<int>> &wf, TH2D *h2) const;
  
  unsigned int const get_n_skip_edge_points(unsigned int val) const {return _n_skip_edge_points;};
  inline void set_n_skip_edge_points(unsigned int val){_n_skip_edge_points = val;};

  inline const Int_t get_dbscan_run_time_musec() const {return _dbscan_run_time_musec;};

  inline void set_k_dist_graph_flag(bool val){_k_dist_graph_flag = val;};
  
private:

  int get_flower_digital_sum(const unsigned int ch_i, const unsigned int wf_j, const std::vector<std::vector<int>> &wf, Int_t w_l, Int_t w_r, Bool_t norm_yes);
  int get_digital_sum(const unsigned int ch_i, const unsigned int wf_j, const std::vector<std::vector<int>> &wf, Int_t w_l, Int_t w_r, Bool_t norm_yes, Int_t sum_type);

  std::vector<std::vector<unsigned int>> build_spatial_time_cluster(const std::vector<std::vector<unsigned int>> &trg_vector);
  std::vector<std::vector<unsigned int>> build_time_cluster(const std::vector<std::vector<unsigned int>> &trg_vector);
  std::vector<std::vector<unsigned int>> build_spatial_time_cluster_dbscan(const std::vector<std::vector<unsigned int>> &trg_vector);
  
  const sipmCameraHist* _simphist;

  dbscan *_dbs;
  vector<cluster_info> _dbclusters_v;
  
  Int_t _trg_counter;

  Int_t _dbscan_run_time_musec;

  unsigned int _n_skip_edge_points;

  bool _k_dist_graph_flag;
  
};

#endif
