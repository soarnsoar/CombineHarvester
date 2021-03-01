#include <string>
#include <iostream>
#include <map>
#include <cstdlib>
#include "RooRealVar.h"
#include "RooAbsReal.h"
#include "RooWorkspace.h"
#include "boost/algorithm/string/predicate.hpp"
#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/regex.hpp"

#include "CombineHarvester/CombineTools/interface/CombineHarvester.h"
#include "CombineHarvester/CombineTools/interface/Systematics.h"
#include "CombineHarvester/CombineTools/interface/Process.h"
#include "CombineHarvester/CombineTools/interface/Observation.h"
#include "CombineHarvester/CombineTools/interface/Utilities.h"
#include "CombineHarvester/CombineTools/interface/CardWriter.h"
#include "CombineHarvester/CombineTools/interface/Algorithm.h"
#include "CombineHarvester/CombinePdfs/interface/CMSHistFuncFactory.h"
#include "CombineHarvester/CombinePdfs/interface/MorphFunctions.h"




using namespace std;
using boost::starts_with;
namespace po = boost::program_options;


///cms/ldap_home/jhchoi/HWW_Analysis/slc7/ForShape/CMSSW_10_2_19/src/SNuAnalytics/Combinato/HighMassH/CMSSW_10_2_13/src/CombineHarvester/CombineTools/input_shapes/Resolved_SB_2017.root
int main(int argc, char** argv) {
  //! [part1]
  // Define four categories labelled A, B, C and D, and
  // set the observed yields in a map.


  typedef vector<string> VString;
  po::variables_map vm;


  string mass = "mH";
  po::options_description config("configuration");
  config.add_options()
    ("mass,m", po::value<string>(&mass)->default_value(mass));

  po::store(po::command_line_parser(argc, argv).options(config).run(), vm);
  po::notify(vm);


  string Year="2016";

  string shapedir="/cms/ldap_home/jhchoi/HWW_Analysis/slc7/ForShape/CMSSW_10_2_19/src/SNuAnalytics/Combinato/HighMassH/CMSSW_10_2_13/src/CombineHarvester/CombineTools/input_shapes/";
  std::map<string, string> input_file;
  input_file["Boosted_SR_"+Year+""]=shapedir+"/Boosted_SR_"+Year+".root";
  input_file["Boosted_SB_"+Year+""]=shapedir+"/Boosted_SB_"+Year+".root";
  input_file["Boosted_TOP_"+Year+""]=shapedir+"/Boosted_TOP_"+Year+".root";

  input_file["Resolved_SR_"+Year+""]=shapedir+"/Resolved_SR_"+Year+".root";
  input_file["Resolved_SB_"+Year+""]=shapedir+"/Resolved_SB_"+Year+".root";
  input_file["Resolved_TOP_"+Year+""]=shapedir+"/Resolved_TOP_"+Year+".root";

  //! [part1]

  //! [part2]
  ch::CombineHarvester cb;
  cb.SetVerbosity(3);
  /*
  ch::Categories cats = {
    {1, "__BoostedGGF_SR_MEKDTAG_M1500_C0.01"}
  };

   */
  VString masses={"115", "120", "124", "125", "126", "130", "135", "140", "145", "150", "155", "160", "165", "170", "175", "180", "190", "200", "210", "230", "250", "270", "300", "350", "400", "450", "500", "550", "600", "650", "700", "750", "800", "900", "1000", "1500", "2000", "2500", "3000", "4000", "5000"};
  cb.AddObservations({"*"}, {"hww"}, {"13TeV"}, {"Boosted_SR"},          {{1,"__BoostedGGF_SR_MEKDTAG_M1500_C0.01"},{2,"__BoostedGGF_SR_UNTAGGED_M1500_C0.01"},{3,"__BoostedVBF_SR_NoMEKDCut"}}); //mass,analysis,era,cat(array)
  cb.AddProcesses(   {"*"}, {"hww"}, {"13TeV"}, {"Boosted_SR"}, {"Wjets","Top"}, {{1,"__BoostedGGF_SR_MEKDTAG_M1500_C0.01"},{2,"__BoostedGGF_SR_UNTAGGED_M1500_C0.01"},{3,"__BoostedVBF_SR_NoMEKDCut"}}, false);//mass,analysis,era,procs,cat

  cb.AddProcesses(   masses, {"hww"}, {"13TeV"}, {"Boosted_SR"}, {"ggH_HWW"}, {{1,"__BoostedGGF_SR_MEKDTAG_M1500_C0.01"},{2,"__BoostedGGF_SR_UNTAGGED_M1500_C0.01"},{3,"__BoostedVBF_SR_NoMEKDCut"}}, true);
  //ggH_hww1000_c10brn00
  //! [part2]

  //! [part3]
  using ch::syst::SystMap;
  using ch::syst::SystMapFunc;
  using ch::syst::bin;

  // Add a traditional lnN systematic
  //cb.cp().bin({"Boosted_SR"}).AddSyst(cb, "DummySys", "lnN", SystMap<>::init(1.1)); //AddSyst(cb,nuisancename,type,value)
  cb.cp().channel({"Boosted_SR"}).AddSyst(cb, "DummySys", "lnN", SystMap<>::init(1.1)); //AddSyst(cb,nuisancename,type,value)
  
  //Extract shape from rootfile
  cb.cp().channel({"Boosted_SR"}).backgrounds().ExtractShapes(
				      input_file["Boosted_SR_"+Year+""],
				      "$BIN/WW_mass/histo_$PROCESS",
				      "$BIN/WW_mass/histo_$PROCESS_$SYSTEMATIC"
				      );//filepath, nominal histopath,syshistopath
  
  
  cb.cp().channel({"Boosted_SR"}).process({"ggH_HWW"}).ExtractShapes(
				      input_file["Boosted_SR_"+Year+""],
				      "$BIN/WW_mass/histo_ggH_hww$MASS_c10brn00",
				      "$BIN/WW_mass/histo_ggH_hww$MASS_c10brn00_$SYSTEMATIC"
				      );//filepath, nominal histopath,syshistopath
  //! [part3]
  RooRealVar mH(mass.c_str(), mass.c_str(),115.,5000.);
  cout <<"<mass>=====>"<<mass<<endl;
  mH.setConstant(true);
  //map<string, RooAbsReal *> mass_var = {
  //  {"ggH_HWW", &mH}
  //};
  TFile fdebug("morphing_debug.root", "RECREATE");
  RooWorkspace ws("hww", "hww");
  auto bins = cb.bin_set();
  for (auto b : bins) {
    cout<<"bin="<<b<<endl;
    auto procs = cb.cp().bin({b}).process({"ggH_HWW"}).process_set();
    //auto procs = cb.cp().bin({b}).process_set();
    for (auto p : procs){
      cout<<"proc="<<p<<endl;
      ch::BuildRooMorphing(ws, cb, 
			   b, p,
      			   //*(mass_var[p]), "norm",
			   mH, "norm",
      			   //true,true);
			   true,false,false, &fdebug);
    } 
    /*
      std::string BuildRooMorphing(RooWorkspace& ws, CombineHarvester& cb,
      std::string const& bin, std::string const& process,
      RooAbsReal& mass_var, std::string norm_postfix,
      bool allow_morph, bool verbose, bool force_template_limit=false, TFile * file = nullptr);
      
    */
  }//end of bin loop
  fdebug.Close();
  cb.AddWorkspace(ws);
  cb.cp().process({"ggH_HWW"}).ExtractPdfs(cb,"hww","$BIN_$PROCESS_morph");//    cb.cp().signals().ExtractPdfs(cb, "htt", "$BIN_$PROCESS_morph");

  //! [part4]
  cout <<"[jhchoi]PrintAll"<<endl;
  cb.PrintAll();

  //TFile output("jhchoi_input.root", "RECREATE");
  //cb.WriteDatacard("jhchoi_v3.txt",output);
  cout <<"[jhchoi]CardWriter"<<endl;
  ch::CardWriter writer("cb_output/$TAG/jhchoi_v3.txt",
			"cb_output/$TAG/jhchoi_v3_input.root");
  writer.SetWildcardMasses({});
  cout <<"[jhchoi]writer.SetVerbosity"<<endl;
  writer.SetVerbosity(1);
  cout <<"[jhchoi]WriteCards"<<endl;

  writer.WriteCards("cmb", cb);
  cout <<"[jhchoi]End WriteCards"<<endl;
  //cb.PrintAll();


  //! [part4]
}
