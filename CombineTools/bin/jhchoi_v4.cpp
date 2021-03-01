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
  typedef vector<pair<int, string>> Categories;


  po::variables_map vm;

  //---init arguement---//
  string mass = "mH";
  string output_folder = "FullRun2";
  //string model = "_RelW002"; // Width model
  string model = "_c10brn00"; // 
  bool do2018semi=false;
  bool do2017semi=false;
  bool do2016semi=false;

  po::options_description config("configuration");
  config.add_options()
    ("mass,m", po::value<string>(&mass)->default_value(mass))
    ("do2018semi", po::value<bool>(&do2018semi)->default_value(do2018semi))
    ("do2017semi", po::value<bool>(&do2017semi)->default_value(do2017semi))
    ("do2016semi", po::value<bool>(&do2016semi)->default_value(do2016semi));
    

  po::store(po::command_line_parser(argc, argv).options(config).run(), vm);
  po::notify(vm);

  std::map<string, string> input_dir;
  string thisyear;
  string shapedir="/cms/ldap_home/jhchoi/HWW_Analysis/slc7/ForShape/CMSSW_10_2_19/src/SNuAnalytics/Combinato/HighMassH/CMSSW_10_2_13/src/CombineHarvester/CombineTools/input_shapes/";

  if (do2016semi){
    thisyear = "2016";
    input_dir["lqq6_bs"] = shapedir+"/Boosted_SR_"+thisyear+".root";
    input_dir["lqq6_top_bs"] = shapedir+"/Boosted_TOP_"+thisyear+".root";
    input_dir["lqq6_wj_bs"] = shapedir+"/Boosted_SB_"+thisyear+".root";

    input_dir["lqq6"] = shapedir+"/Resolved_SR_"+thisyear+".root";
    input_dir["lqq6_top"] = shapedir+"/Resolved_TOP_"+thisyear+".root";
    input_dir["lqq6_wj"] = shapedir+"/Resolved_SB_"+thisyear+".root";

    output_folder = "Full2016semi";
  }
  if (do2017semi){
    input_dir["lqq7_bs"] = shapedir+"/Boosted_SR_"+thisyear+".root";
    input_dir["lqq7_top_bs"] = shapedir+"/Boosted_TOP_"+thisyear+".root";
    input_dir["lqq7_wj_bs"] = shapedir+"/Boosted_SB_"+thisyear+".root";

    input_dir["lqq7"] = shapedir+"/Resolved_SR_"+thisyear+".root";
    input_dir["lqq7_top"] = shapedir+"/Resolved_TOP_"+thisyear+".root";
    input_dir["lqq7_wj"] = shapedir+"/Resolved_SB_"+thisyear+".root";

    output_folder = "Full2017semi";
  }
  if (do2018semi){
    input_dir["lqq8_bs"] = shapedir+"/Boosted_SR_"+thisyear+".root";
    input_dir["lqq8_top_bs"] = shapedir+"/Boosted_TOP_"+thisyear+".root";
    input_dir["lqq8_wj_bs"] = shapedir+"/Boosted_SB_"+thisyear+".root";

    input_dir["lqq8"] = shapedir+"/Resolved_SR_"+thisyear+".root";
    input_dir["lqq8_top"] = shapedir+"/Resolved_TOP_"+thisyear+".root";
    input_dir["lqq8_wj"] = shapedir+"/Resolved_SB_"+thisyear+".root";

    output_folder = "Full2018semi";
  }


  //channel
  VString chns;
  for (std::map<string, string>::iterator it=input_dir.begin(); it!=input_dir.end(); ++it){
    chns.push_back(it->first);
  }

  map<string, VString> bkg_procs;
  for (std::map<string, string>::iterator it=input_dir.begin(); it!=input_dir.end(); ++it){
    if(it->first.find("qq") != std::string::npos){ //Semilep
      bkg_procs[it->first] = {"DY", "MultiBoson", "WW","ggWW", "qqWWqq", "Top", "Wjets", "QCD"};
    }else if(it->first.find("em") != std::string::npos){ //Dilep em
      bkg_procs[it->first] = {"DY", "DYemb", "Fake_em", "Fake_me", "VVV", "VZ", "Vg", "WW", "WWewk", "VgS_H", "VgS_L", "ggWW", "qqWWqq", "WW2J", "top"};
    }else if(it->first.find("ee") != std::string::npos){ //Dilep ee
      bkg_procs[it->first] = {"DY",          "Fake_ee",            "VVV", "VZ", "Vg", "WW", "WWewk", "VgS_H", "VgS_L", "ggWW", "qqWWqq", "WW2J", "top"};
    }else if(it->first.find("mm") != std::string::npos){ //Dilep mm
      bkg_procs[it->first] = {"DY",          "Fake_mm",            "VVV", "VZ", "Vg", "WW", "WWewk", "VgS_H", "VgS_L", "ggWW", "qqWWqq", "WW2J", "top"};
    }
  }





  //! [part1]

  //! [part2]
  ch::CombineHarvester cb;
  cb.SetVerbosity(3);
  /*
  ch::Categories cats = {
    {1, "__BoostedGGF_SR_MEKDTAG_M1500_C0.01"}
  };

   */
  map<string,Categories> cats;

  for(auto year: std::vector<std::string> {"8", "7", "6"}){ // Semi-leptonic categories 
    //resolved region
    cats["lqq"+year+"_13TeV"] = {
      {1, "___ResolvedGGFDNN__SR_MEKDTAG_M400_C0.01"},
      {2, "___ResolvedGGFDNN__SR_UNTAGGED_M400_C0.01"},
      {3,"___ResolvedVBFDNN__SR_NoMEKDCut"}
    };

    cats["lqq"+year+"_top_13TeV"] = {
      {1, "___ResolvedGGFDNN__TOP_MEKDTAG_M400_C0.01"},
      {2, "___ResolvedGGFDNN__TOP_UNTAGGED_M400_C0.01"},
      {3,"___ResolvedVBFDNN__TOP_NoMEKDCut"}
    };

    cats["lqq"+year+"_wj_13TeV"] = {
      {1, "___ResolvedGGFDNN__SB_MEKDTAG_M400_C0.01"},
      {2, "___ResolvedGGFDNN__SB_UNTAGGED_M400_C0.01"},
      {3,"___ResolvedVBFDNN__SB_NoMEKDCut"}
    };

    //boosted region
    cats["lqq"+year+"_bs_13TeV"] = {
      {1, "__BoostedGGFDNN_SR_MEKDTAG_M1500_C0.01"},
      {2, "__BoostedGGFDNN_SR_UNTAGGED_M1500_C0.01"},
      {3, "__BoostedVBFDNN_SR_NoMEKDCut"}
    };

    cats["lqq"+year+"_top_bs_13TeV"] = {
      {1, "__BoostedGGFDNN_TOP_MEKDTAG_M1500_C0.01"},
      {2, "__BoostedGGFDNN_TOP_UNTAGGED_M1500_C0.01"},
      {3, "__BoostedVBFDNN_TOP_NoMEKDCut"}
    };

    cats["lqq"+year+"_wj_bs_13TeV"] = {
      {1, "__BoostedGGFDNN_SB_MEKDTAG_M1500_C0.01"},
      {2, "__BoostedGGFDNN_SB_UNTAGGED_M1500_C0.01"},
      {3, "__BoostedVBFDNN_SB_NoMEKDCut"}
    };


  }
  map<string, VString> masses;
  VString allmasses;

  for (std::map<string, string>::iterator it=input_dir.begin(); it!=input_dir.end(); ++it){
    if(it->first.find("qq") != std::string::npos && it->first.find("bs") != std::string::npos){ // Semi-leptonic boosted
      masses[it->first] ={"400", "450", "500", "550", "600", "650", "700", "750", "800", "900", "1000", "1500", "2000", "2500", "3000", "4000", "5000"};
    }
    else if(it->first.find("qq") != std::string::npos && it->first.find("bs") == std::string::npos){ 
      masses[it->first] = {"115", "120", "124", "125", "126", "130", "135", "140", "145", "150", "165", "170", "175", "180", "190", "200", "210", "230", "250", "270", "300", "350", "400", "450", "500", "550", "600","650", "700", "750", "800", "900", "1000", "1500", "2000", "2500", "3000", "4000", "5000"};
    }
  }
  map<string, VString> signal_types = {
    {"ggH", {"ggH_HWW", "ggH_HWWSBI"}},
    {"qqH", {"qqH_HWW", "qqH_HWWSBI"}}
  };

  for(auto chn : chns){
    cb.AddObservations({"*"}, {"hww"}, {"13TeV"}, {chn}, cats[chn+"_13TeV"]);
    cb.AddProcesses({"*"}, {"hww"}, {"13TeV"}, {chn}, bkg_procs[chn], cats[chn+"_13TeV"], false);
    cb.AddProcesses(masses[chn], {"hww"}, {"13TeV"}, {chn}, signal_types["ggH"], cats[chn+"_13TeV"], true);
    cb.AddProcesses(masses[chn], {"hww"}, {"13TeV"}, {chn}, signal_types["qqH"], cats[chn+"_13TeV"], true);
  }




  /*
  VString masses={"115", "120", "124", "125", "126", "130", "135", "140", "145", "150", "155", "160", "165", "170", "175", "180", "190", "200", "210", "230", "250", "270", "300", "350", "400", "450", "500", "550", "600", "650", "700", "750", "800", "900", "1000", "1500", "2000", "2500", "3000", "4000", "5000"};
  cb.AddObservations({"*"}, {"hww"}, {"13TeV"}, {"Boosted_SR"},          {{1,"__BoostedGGF_SR_MEKDTAG_M1500_C0.01"},{2,"__BoostedGGF_SR_UNTAGGED_M1500_C0.01"},{3,"__BoostedVBF_SR_NoMEKDCut"}}); //mass,analysis,era,cat(array)
  cb.AddProcesses(   {"*"}, {"hww"}, {"13TeV"}, {"Boosted_SR"}, {"Wjets","Top"}, {{1,"__BoostedGGF_SR_MEKDTAG_M1500_C0.01"},{2,"__BoostedGGF_SR_UNTAGGED_M1500_C0.01"},{3,"__BoostedVBF_SR_NoMEKDCut"}}, false);//mass,analysis,era,procs,cat

  cb.AddProcesses(   masses, {"hww"}, {"13TeV"}, {"Boosted_SR"}, {"ggH_HWW"}, {{1,"__BoostedGGF_SR_MEKDTAG_M1500_C0.01"},{2,"__BoostedGGF_SR_UNTAGGED_M1500_C0.01"},{3,"__BoostedVBF_SR_NoMEKDCut"}}, true);
  */
  //ggH_hww1000_c10brn00
  //! [part2]



  //! [part3]
  ///-------Systematics
  using ch::syst::SystMap;
  using ch::syst::SystMapFunc;
  using ch::syst::bin;

  // Add a traditional lnN systematic
  //cb.cp().bin({"Boosted_SR"}).AddSyst(cb, "DummySys", "lnN", SystMap<>::init(1.1)); //AddSyst(cb,nuisancename,type,value)
  //cb.cp().channel({"Boosted_SR"}).AddSyst(cb, "DummySys", "lnN", SystMap<>::init(1.1)); //AddSyst(cb,nuisancename,type,value)
  ///---------Systematics
  for (string chn : chns) {
    std::string discrim = "WW_mass";
    if (chn.find("top") != std::string::npos || chn.find("wj") != std::string::npos) discrim = "Event"; //CR
    
    cb.cp().channel({chn}).backgrounds().ExtractShapes(
						       input_dir[chn],
						       "$BIN/"+discrim+"/histo_$PROCESS",
						       "$BIN/"+discrim+"/histo_$PROCESS_$SYSTEMATIC");
    
    cb.cp().channel({chn}).process({"ggH_HWW"}).ExtractShapes(
							      input_dir[chn],
							      "$BIN/"+discrim+"/histo_ggH_hww$MASS"+model,
							      "$BIN/"+discrim+"/histo_ggH_hww$MASS"+model+"_$SYSTEMATIC");
    cb.cp().channel({chn}).process({"ggH_HWWSBI"}).ExtractShapes(
								 input_dir[chn],
								 "$BIN/"+discrim+"/histo_ggH_hww_SBI$MASS"+model,
								 "$BIN/"+discrim+"/histo_ggH_hww_SBI$MASS"+model+"_$SYSTEMATIC");
    cb.cp().channel({chn}).process({"qqH_HWW"}).ExtractShapes(
							      input_dir[chn],
							      "$BIN/"+discrim+"/histo_qqH_hww$MASS"+model,
							      "$BIN/"+discrim+"/histo_qqH_hww$MASS"+model+"_$SYSTEMATIC");
    cb.cp().channel({chn}).process({"qqH_HWWSBI"}).ExtractShapes(
								 input_dir[chn],
								 "$BIN/"+discrim+"/histo_qqH_hww_SBI$MASS"+model,
								 "$BIN/"+discrim+"/histo_qqH_hww_SBI$MASS"+model+"_$SYSTEMATIC");


    
  }


  //Extract shape from rootfile
    /*
  cb.cp().channel({"Boosted_SR"}).backgrounds().ExtractShapes(
				      input_file["Boosted_SR_"+thisyear+""],
				      "$BIN/WW_mass/histo_$PROCESS",
				      "$BIN/WW_mass/histo_$PROCESS_$SYSTEMATIC"
				      );//filepath, nominal histopath,syshistopath
  
  
  cb.cp().channel({"Boosted_SR"}).process({"ggH_HWW"}).ExtractShapes(
				      input_file["Boosted_SR_"+thisyear+""],
				      "$BIN/WW_mass/histo_ggH_hww$MASS_c10brn00",
				      "$BIN/WW_mass/histo_ggH_hww$MASS_c10brn00_$SYSTEMATIC"
				      );//filepath, nominal histopath,syshistopath
    */
  //! [part3]
  ch::SetStandardBinNames(cb);
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
    auto procs = cb.cp().bin({b}).process(ch::JoinStr({signal_types["ggH"], signal_types["qqH"]})).process_set();
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
  //cb.cp().process({"ggH_HWW"}).ExtractPdfs(cb,"hww","$BIN_$PROCESS_morph");//    cb.cp().signals().ExtractPdfs(cb, "htt", "$BIN_$PROCESS_morph");
  cb.cp().process(ch::JoinStr({signal_types["ggH"], signal_types["qqH"]})).ExtractPdfs(cb, "hww", "$BIN_$PROCESS_morph");


  //! [part4]
  cout <<"[jhchoi]PrintAll"<<endl;
  cb.PrintAll();

  string output_prefix = "output/";
  string root_suffix = "";

  if (do2016semi){root_suffix = root_suffix + "semi16";}
  if (do2017semi){root_suffix = root_suffix + "semi17";}
  if (do2018semi){root_suffix = root_suffix + "semi18";}

  if(output_folder.compare(0,1,"/") == 0) output_prefix="";//relative path

  ch::CardWriter writer(output_prefix + output_folder + "/$TAG/$BIN.txt",
                        output_prefix + output_folder + "/$TAG/hww_input"+root_suffix+".root");
  ch::CardWriter writercmb(output_prefix + output_folder + "/$TAG/combined.txt",
                           output_prefix + output_folder + "/$TAG/hww_input"+root_suffix+".root");

  writer.SetWildcardMasses({});
  writercmb.SetWildcardMasses({});
  writer.SetVerbosity(1);
  writercmb.SetVerbosity(1);
  writer.WriteCards("cmb", cb);
  writercmb.WriteCards("cmb", cb);

  cb.PrintAll();
  cout << " done\n";




  /*

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
  cb.PrintAll();
  cout << " done\n";

  cout <<"[jhchoi]End WriteCards"<<endl;
  //cb.PrintAll();
  */

  //! [part4]
}
