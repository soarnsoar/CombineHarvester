#include <string>
#include <map>
#include <set>
#include <iostream>
#include <utility>
#include <vector>
#include <cstdlib>
#include "CombineHarvester/CombineTools/interface/CombineHarvester.h"
#include "CombineHarvester/CombineTools/interface/Observation.h"
#include "CombineHarvester/CombineTools/interface/Process.h"
#include "CombineHarvester/CombineTools/interface/Utilities.h"
#include "CombineHarvester/CombineTools/interface/Systematics.h"
#include "CombineHarvester/CombineTools/interface/BinByBin.h"


using namespace std;

int main() {
  //! [part1]
  // First define the location of the "auxiliaries" directory where we can
  // source the input files containing the datacard shapes
  //string aux_shapes = string(getenv("CMSSW_BASE")) + "/src/auxiliaries/shapes/";
  string aux_shapes = "/cms/ldap_home/jhchoi/HWW_Analysis/slc7/ForShape/CMSSW_10_2_19/src/SNuAnalytics/Combinato/HighMassH/20210113_Regroup/MEKD_0.01_1pb_onlynorm_inCR_signalincr/workspace/2016/Datacards_2016/Datacard_M1000/__BoostedGGF_SR_MEKDTAG_M1500_C0.01/WW_mass/shapes/";

  // Create an empty CombineHarvester instance that will hold all of the
  // datacard configuration and histograms etc.
  ch::CombineHarvester cb;
  // Uncomment this next line to see a *lot* of debug information
  // cb.SetVerbosity(3);

  // Here we will just define two categories for an 8TeV analysis. Each entry in
  // the vector below specifies a bin name and corresponding bin_id.
  //ch::Categories cats = {
  //   {1, "muTau_1jet_medium"},
  //    {2, "muTau_vbf_loose"}
  // };

  ch::Categories cats = {
    {1, "__BoostedGGF_SR_MEKDTAG_M1500_C0.01"}
  };
  // ch::Categories is just a typedef of vector<pair<int, string>>
  //! [part1]


  //! [part2]
  //vector<string> masses = ch::MassesFromRange("120-135:5");
  // Or equivalently, specify the mass points explicitly:
  //vector<string> masses = {"120", "125", "130", "135"};
  vector<string> masses = {"1000"};
  //! [part2]

  //! [part3]
  //
  cout <<"<cb.AddObservations>"<<endl;
  cb.AddObservations({"*"}, {"hww"}, {"13TeV"}, {"BoostGGFSR"}, cats); //modify Observation class to get process nname "Data" instead of data_obs
  cout <<"FIN.<cb.AddObservations>"<<endl;
  //! [part3]

  //! [part4]
  //vector<string> bkg_procs = {"ZTT", "W", "QCD", "TT"};
  //cb.AddProcesses({"*"}, {"htt"}, {"8TeV"}, {"mt"}, bkg_procs, cats, false);

  vector<string> bkg_procs = {"TT","Wjets"};
  //cb.AddProcesses({"*"}, {"htt"}, {"8TeV"}, {"mt"}, bkg_procs, cats, false);

  //vector<string> sig_procs = {"ggH", "qqH"};
  //cb.AddProcesses(masses, {"htt"}, {"8TeV"}, {"mt"}, sig_procs, cats, true);

  vector<string> sig_procs = {"ggH_hww1000_c10brn00", "qqH_hww1000_c10brn00"};
  cb.AddProcesses(masses, {"hww"}, {"13TeV"}, {"BoostGGFSR"}, sig_procs, cats, true);
  //! [part4]

  cout <<"Fin.<.AddProcesses>"<<endl;
  //Some of the code for this is in a nested namespace, so
  // we'll make some using declarations first to simplify things a bit.
  using ch::syst::SystMap;
  using ch::syst::era;
  using ch::syst::bin_id;
  using ch::syst::process;


  //! [part5]
  cb.cp().signals()
    .AddSyst(cb, "lumi_13TeV_2016", "lnN", SystMap<era>::init
	     ({"13TeV"}, 1.022)
	     );
  //! [part5]

  //! [part6]
  cb.cp().process({"ggH_hww1000_c10brn00"})
      .AddSyst(cb, "pdf_gg", "lnN", SystMap<>::init(1.097));
  cout <<"Fin.<.AddSyst>"<<endl;
  /*
  cb.cp().process(ch::JoinStr({sig_procs, {"ZTT", "TT"}}))
      .AddSyst(cb, "CMS_eff_m", "lnN", SystMap<>::init(1.02));

  cb.cp()
      .AddSyst(cb,
        "CMS_scale_j_$ERA", "lnN", SystMap<era, bin_id, process>::init
        ({"8TeV"}, {1},     {"ggH"},        1.04)
        ({"8TeV"}, {1},     {"qqH"},        0.99)
        ({"8TeV"}, {2},     {"ggH"},        1.10)
        ({"8TeV"}, {2},     {"qqH"},        1.04)
        ({"8TeV"}, {2},     {"TT"},         1.05));

  cb.cp().process(ch::JoinStr({sig_procs, {"ZTT"}}))
      .AddSyst(cb, "CMS_scale_t_mutau_$ERA", "shape", SystMap<>::init(1.00));
  */
  //! [part6]

  //! [part7]
  cout <<"<ExtractShapeBkg>"<<endl;
  cb.cp().backgrounds().ExtractShapes(
      aux_shapes + "/histos___BoostedGGF_SR_MEKDTAG_M1500_C0.01.root",
      "histo_$PROCESS",
      "histo_$PROCESS_$SYSTEMATIC");
  cout <<"<ExtractShapeSig>"<<endl;
  cb.cp().signals().ExtractShapes(
      aux_shapes + "/histos___BoostedGGF_SR_MEKDTAG_M1500_C0.01.root",
      "histo_$PROCESS",
      "histo_$PROCESS_$SYSTEMATIC");
  //! [part7]

  //! [part8]
  //auto bbb = ch::BinByBinFactory()
  //  .SetAddThreshold(0.1)
  // .SetFixNorm(true);

  //bbb.AddBinByBin(cb.cp().backgrounds(), cb);

  // This function modifies every entry to have a standardised bin name of
  // the form: {analysis}_{channel}_{bin_id}_{era}
  // which is commonly used in the htt analyses
  cout <<"<SetStandardBinNames>"<<endl;
  ch::SetStandardBinNames(cb);
  //! [part8]

  //! [part9]
  // First we generate a set of bin names:
  cout <<"bin set>"<<endl;
  set<string> bins = cb.bin_set();
  // This method will produce a set of unique bin names by considering all
  // Observation, Process and Systematic entries in the CombineHarvester
  // instance.

  // We create the output root file that will contain all the shapes.
  TFile output("jhchoi_input.root", "RECREATE");

  // Finally we iterate through each bin,mass combination and write a
  // datacard.
  for (auto b : bins) {
    for (auto m : masses) {
      cout << ">> Writing datacard for bin: " << b << " and mass: " << m
           << "\n";
      // We need to filter on both the mass and the mass hypothesis,
      // where we must remember to include the "*" mass entry to get
      // all the data and backgrounds.
      cb.cp().bin({b}).mass({m, "*"}).WriteDatacard(
          b + "_" + m + ".txt", output);
    }
  }
  //! [part9]

}
