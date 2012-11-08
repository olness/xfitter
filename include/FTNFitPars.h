/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  \author Wojtek Slominski, Jagiellonian Univ., Physics Dept.
  \date 2012-11-02
  \copyright Creative Commons license CC-BY-NC 3.0
_____________________________________________________________*/

#ifndef FTN_FITPARS_H_
#define FTN_FITPARS_H_

#include <iostream>
#include "FitPars_base.h"

// #define TST_SHOW(a) cout << #a" = '" << (a) <<"'"<< endl;


// oooooooooooooooooooooooooooooooooooooooo
class FTNFitPars_t : public FitPars_base_t {
  string M_tit;
  StringList M_cmd;
  
  public:
  // ================================================
  string ReadTitle(const char * fnsave) {
    M_tit.erase();
    TextReader_t tx(fnsave);
    tx.Open();
    tx.Read("");
    // tx.Close();
    int iLine = tx.Find("SET TIT*",0,false);
    if(iLine >= 0) M_tit = tx[iLine+1];
    return M_tit;
  }

  // ================================================
  string GetTitle() {return M_tit;}

  // ================================================
  void SetTitle(const string& tit) {M_tit = tit;}

  // ================================================
  void ReadCommands(const char * fnsave) {
    M_cmd.clear();
    TextReader_t tx(fnsave);
    tx.Open();
    tx.Read();
    int N = tx.NLines();
    int iLine = tx.Find("PARAM*",0,false);
    if(iLine < 0) return;
    while(iLine < N && !(tx[iLine].IsBlank())) iLine++;
    // cout << tx[iLine] << endl;}
    while(iLine < N && (tx[iLine].IsBlank())) iLine++;
    if(iLine == N) return;
    // return tx.Get(iLine);
    while(iLine < N) {if(!tx[iLine].IsBlank()) M_cmd.push_back(tx[iLine]); iLine++;}
  }

  // ================================================
  void Read(const char* fn, Xstring what="tpc") {
    // cout << "  what: " << what << endl;
    if(what.ToLower().Contains("t")) ReadTitle(fn);
    if(what.ToLower().Contains("p")) ReadMnSaved(fn);
    if(what.ToLower().Contains("c")) ReadCommands(fn);
  }

  // ================================================
  StringList GetCommands() {
    return M_cmd;
  }

  // ================================================
  void SetCommands(const Xstring& cmds) {
    M_cmd = cmds.Split(";\n");
  }

  // ================================================
  void Write(ostream &ostr=cout, Xstring what="tpc") {
    // cout << "  what: " << what << endl;
    if(what.ToLower().Contains("t")) ostr <<"SET TITLE\n"<< M_tit << endl;
    if(what.ToLower().Contains("p")) ShowF(ostr);
    if(what.ToLower().Contains("c")) ostr << M_cmd.Join("\n") << endl;
  }

  // ================================================
  void Write(const char* fn, const Xstring& what="tpc") {
    cout << "===> Writing \""<< what << "\" to '" << fn <<"'"<< endl;
    ofstream out(fn);
    Write(out, what);
    out.close();
  }
  
  // ==========================================================
  void GetMinuitParams();
  void SetExtraParams();

};

#endif
