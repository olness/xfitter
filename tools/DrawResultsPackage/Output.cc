#include "Output.h"
#include <fstream>
#include <dirent.h>
#include <TObjString.h>
#include <stdlib.h>
#include <math.h>
#include "TGraphAsymmErrors.h"
#include "PdfTable.h"

Output::Output(const Char_t* directory) {

  fDirectory    = new TString(directory);
  fName = new TString(directory);
  for(Int_t ipdf = 0; ipdf < fNpdfs; ipdf++) {
    fPdfs[ipdf] = new TObjArray;
    fPdfs[ipdf]->SetOwner();
  }
  fNDataSets = 0;
  fNpoints = 0;
  fNQ2Files = 0;
  fPull = new TH1F("","",20, -3., 3.);
  fParametersCheck = false;
  fNuisanceCheck = false;
  fCovarianceCheck = false;
  fMessagesCheck = false;

  fMessages = new TObjArray; fMessages->SetOwner();
  fNFittedParameters = 0;
  fNNuisanceParameters = 0;
  for(Int_t i=0; i<fMaxParameters; i++) {
    fFittedParametersNames[i] = NULL;
    fNuisanceParNames[i] = NULL;
    fFittedParameters[i][0] = -99.;
    fFittedParameters[i][1] = -99.;
    fNuisancePar[i][0];
    fNuisancePar[i][1];
    for(Int_t j=0; j<fMaxParameters; j++) {
      fCorrPar[i][j] = -99.;
    }
  }
  fErrorCalculationMethod = new TString("none");
  fCorrelationCalculationMethod = new TString("none");
  fErrorTrustLevel = new TString("none");
  fConverged = kFALSE;
  fFinished = kFALSE;
  fChi2UncTotal = -1;
  fChi2CorTotal = -1;
}

Output::~Output(){
  delete fName;
  delete fDirectory;
  for(Int_t ipdf = 0; ipdf < fNpdfs; ipdf++) {
    delete fPdfs[ipdf];
  }
  delete fErrorCalculationMethod;
  delete fCorrelationCalculationMethod;
  delete fErrorTrustLevel;

  for(int i=0; i<fMaxParameters; i++) if(fFittedParametersNames[i]) delete fFittedParametersNames[i];
  for(int i=0; i<fMaxParameters; i++) if(fNuisanceParNames[i]) delete fNuisanceParNames[i];
  for (std::vector<DataSet*>::iterator i = fDataSets.begin(); i != fDataSets.end(); ++i)  delete *i;
  delete fPull;
  delete fMessages;
}

Int_t Output::Prepare(bool DrawBand) {
  if( this->CheckDirectory() ) {
    this->PrepareName();
    this->PreparePdf(DrawBand);
    this->PrepareDataSets();
    this->PrepareParameters();
  }
  else if( this->CheckFile() ) {
    this->PrepareMandyParameters();
  }
  else {
    cerr << "Can not open directory " << fDirectory->Data() << endl; 
    exit(1);
  }
}

void Output::PrepareMandyParameters() {
  ifstream input(fDirectory->Data());
  if(!input.is_open()) return;

  Char_t buffer[256];
  while(!input.eof()) {
    input.getline(buffer, 256);
    
    TString line(buffer);
    if(line.BeginsWith("NAME:")) {
      fName->Form(line.Data());
      fName->ReplaceAll("NAME:","");
    }

    TString temp;
    Int_t idx;

    TObjArray* array = line.Tokenize(' ');

    if(line.BeginsWith(" PARAM, VAL, ERR")) { // migrad errrors
      if(array->GetEntries() < 7) continue;
      switch(((TObjString*)array->At(4))->GetString().Atoi()) 
	{
	case 1:	 temp.Form("Buv");	idx = 4;    break;
	case 2:	 temp.Form("Cuv");	idx = 5;    break;
	case 3:	 temp.Form("Euv");	idx = 6;    break;
	case 5:	 temp.Form("Bdv");	idx = 7;    break;
	case 6:	 temp.Form("Cdv");	idx = 8;    break;
	case 7:	 temp.Form("Bprig");	idx = 3;    break;
	case 8:	 temp.Form("Aprig");	idx = 2;    break;
	case 9:	 temp.Form("Asea");	idx = 10;   break;
	case 10: temp.Form("Bdbar");	idx = 11;   break;
	case 11: temp.Form("Cubar");	idx = 9;    break;
	case 12: temp.Form("Cdbar");	idx = 12;   break;
	case 13: temp.Form("Bg");	idx = 0;    break;
	case 14: temp.Form("Cg");	idx = 1;    break;
	case 17: temp.Form("alphas");	idx = 13;   break;
	default: continue;
	}
      fFittedParametersNames[idx] = new TString(temp.Data());
      fFittedParameters[idx][0] = ((TObjString*)array->At(5))->GetString().Atof();
      fFittedParameters[idx][1] = ((TObjString*)array->At(6))->GetString().Atof();
    }

   if(line.BeginsWith(" PARAM, HESSE ERR=")) { // hesse errrors
      if(array->GetEntries() < 6) continue;
      switch(((TObjString*)array->At(3))->GetString().Atoi()) 
	{
	case 1:	 temp.Form("Buv");	idx = 4;    break;
	case 2:	 temp.Form("Cuv");	idx = 5;    break;
	case 3:	 temp.Form("Euv");	idx = 6;    break;
	case 5:	 temp.Form("Bdv");	idx = 7;    break;
	case 6:	 temp.Form("Cdv");	idx = 8;    break;
	case 7:	 temp.Form("Bprig");	idx = 3;    break;
	case 8:	 temp.Form("Aprig");	idx = 2;    break;
	case 9:	 temp.Form("Asea");	idx = 10;   break;
	case 10: temp.Form("Bdbar");	idx = 11;   break;
	case 11: temp.Form("Cubar");	idx = 9;    break;
	case 12: temp.Form("Cdbar");	idx = 12;   break;
	case 13: temp.Form("Bg");	idx = 0;    break;
	case 14: temp.Form("Cg");	idx = 1;    break;
	case 17: temp.Form("alphas");	idx = 13;   break;
	default: continue;
	}
      fFittedParametersNames[idx] = new TString(temp.Data());
      fFittedParameters[idx][0] = ((TObjString*)array->At(4))->GetString().Atof();
      fFittedParameters[idx][1] = ((TObjString*)array->At(5))->GetString().Atof();
      fNFittedParameters++;
   }
   
   if(array->GetEntries() >= 2 ) {
     if(((TObjString*)array->At(0))->GetString().IsDigit() &&
	((TObjString*)array->At(1))->GetString().IsFloat()) {
       
       switch(((TObjString*)array->At(0))->GetString().Atoi()) 
	 {
	 case 132: temp.Form("hera_proc_1"); idx = 0; break;
	 case 133: temp.Form("hera_proc_2"); idx = 1; break;
	 case 134: temp.Form("hera_proc_3"); idx = 2; break;
	 case 208: temp.Form("zeus96/97 JES");    idx = 11; break;
	 case 209: temp.Form("zeus96/97 norm");   idx = 12; break;
	 case 210: temp.Form("zeus98/00 JES");    idx = 13; break;
	 case 211: temp.Form("zeus98/00 norm");   idx = 14; break;
	 case 212: temp.Form("H1 hiq2jets Ee");   idx = 3; break;
	 case 213: temp.Form("H1 hiq2jets Et");   idx = 4; break;
	 case 214: temp.Form("H1 hiq2jets HFS");  idx = 5; break;
	 case 215: temp.Form("H1 loq2jets Ee");   idx = 7; break;
	 case 216: temp.Form("H1 loq2jets Et");   idx = 8; break;
	 case 217: temp.Form("H1 loq2jets HFS");  idx = 9; break;
	 case 218: temp.Form("H1 loq2jets mod");  idx = 6; break;
	 case 219: temp.Form("H1 loq2jets norm"); idx = 10; break;
	   //case 220: temp.Form("?"); idx = 2; break;
	   //case 221: temp.Form("?"); idx = 2; break;
	   //case 224: temp.Form("BH norm"); idx = 2; break;
	   
	 default: continue;
	 }
       fNuisanceParNames[idx] = new TString(temp.Data());
       fNuisancePar[idx][0] = ((TObjString*) array->At(1))->GetString().Atof();
       fNuisancePar[idx][1] = 0.;
       fNNuisanceParameters = fMaxParameters;
     }
   }
   
   delete array;
   
  }
  fParametersCheck = true;
  fNuisanceCheck = true;
  
  input.close();
}

Bool_t Output::CheckFile() {
  ifstream input(fDirectory->Data());
  if(!input.is_open()) return false;
  input.close();
  return true;
}


Bool_t Output::CheckDirectory() {
  DIR *pDir;
  bool bExists = false;
  pDir = opendir (fDirectory->Data());
  if (pDir != NULL) {
    bExists = true;    
    (void) closedir (pDir);
  }
  return bExists;
}


void Output::PrepareParameters() {

  TString* filename = new TString;
  TString str;
  char buffer[256];
  filename->Form("%s/parsout_0",fDirectory->Data());
  
  ifstream infile(filename->Data());

  if(!infile.is_open()) { cout << "Output::Parameters: can not open file %s" << filename->Data()<<endl; return;}

  int idx = 0;
  while(!infile.eof()) {
    infile.getline(buffer, 256);
    for(int i=0; i<256; i++) if(buffer[i]=='%') buffer[i]='#';
    str.Form(buffer);
    
    TObjArray* array = str.Tokenize(" ");
    int NColumns = array->GetEntries();
    if(NColumns == 4) {
      if(idx>=fMaxParameters-2) {cout << "Output: fMaxParameters too low"<< endl; exit(1);}

      if(!((TObjString*) array->At(0))->GetString().IsDigit()) continue;
      if(!((TObjString*) array->At(2))->GetString().IsFloat()) continue;
      if(!((TObjString*) array->At(3))->GetString().IsFloat()) continue;
      if(((TObjString*) array->At(3))->GetString().Atof() > 0.0000001) { // this is minimised parameter
	fFittedParametersNames[idx] = new TString(((TObjString*) array->At(1))->GetString().Data());
	fFittedParameters[idx][0] = ((TObjString*) array->At(2))->GetString().Atof();
	fFittedParameters[idx][1] = ((TObjString*) array->At(3))->GetString().Atof();
	idx++;  
	fNFittedParameters++;
      }
    }
    delete array;
    fParametersCheck = true;
  }

  filename->Form("%s/Results.txt",fDirectory->Data());
  ifstream infile2(filename->Data());
  if(!infile2.is_open()) { cout << "Output::PrepareDataSets: can not open file %s" << filename->Data()<<endl; return;}
  while(!infile2.eof()) {
    infile2.getline(buffer, 256);
    for(int i=0; i<256; i++) if(buffer[i]=='%') buffer[i]='#';
    str.Form(buffer); 
    if(str.Contains("After minimisation"))  {
      TObjArray* array = str.Tokenize(" ");
      if( ((TObjString*) array->At(2))->GetString().IsFloat()) 
        fChi2UncTotal = ((TObjString*) array->At(2))->GetString().Atof();
      delete array;
    }
    else if(str.Contains("Correlated Chi2"))  {
      TObjArray* array = str.Tokenize(" ");
      if( ((TObjString*) array->At(2))->GetString().IsFloat()) 
        fChi2CorTotal = ((TObjString*) array->At(2))->GetString().Atof();
      delete array;
    }
    else if(str.Contains("Systematic shifts"))  {

      int idx = 0;
      
      infile2.getline(buffer, 256);
      str.Form(buffer); 
      TObjArray* array = str.Tokenize(" ");
      int NColumns = array->GetEntries();
      while(NColumns == 5) {
	if(!((TObjString*) array->At(0))->GetString().IsDigit()) {cout << "not a digit!" <<endl; break; }
	fNuisanceParNames[idx] = new TString( ((TObjString*) array->At(1))->GetString().Data() );
	fNuisancePar[idx][0] = ((TObjString*) array->At(2))->GetString().Atof();
	fNuisancePar[idx][1] = ((TObjString*) array->At(4))->GetString().Atof();

	infile2.getline(buffer, 256);
	str.Form(buffer); 	
	array = str.Tokenize(" ");
	NColumns = array->GetEntries();
	idx++;
	fNNuisanceParameters++;
	if(idx >= fMaxParameters) {
	  cerr << "ERROR in DrawResults"<<endl<<"Please increase fMaxParameters parameter in the tools/DrawResultsPackage/include/Output.h file" << endl;
	  exit(1);
	}
      }
      delete array;
    }
  }

  fNuisanceCheck = true;
  delete filename;

}

Double_t Output::GetFittedParameter(Int_t idx, Bool_t error) {
  if(idx >= fMaxParameters) return -99.;
  if(error) return fFittedParameters[idx][1];
  return fFittedParameters[idx][0];
}
Double_t Output::GetNuisancePar(Int_t idx, Bool_t error) {
  if(idx >= fMaxParameters) return -99.;
  if(error) return fNuisancePar[idx][1];
  return fNuisancePar[idx][0];
}

void Output::PrepareName() {
  TString filename("");
  char buffer[256];
  filename.Form("%s/fit.name",fDirectory->Data());
  
  ifstream infile(filename.Data());
  if (!infile.is_open()) {
    fName->Form(fDirectory->Data());
  }
  else {
    infile.getline(buffer, 256);
    fName->Form(buffer);
  }
  if(!fName->CompareTo(fDirectory->Data())
     || !fName->CompareTo("dummy")) {
    cout << "You can add custom legend label for your plots by editing "
	 << filename.Data() << " file"<<endl; 
    fName->Form(fDirectory->Data());
  }
}

Int_t Output::PreparePdf(bool DrawBand) {

  // Loop over Q2 values, read PDF tables
  for (Int_t iq2=0; iq2<200; iq2++) {

    TString filename("");
    filename.Form("%s/pdfs_q2val_%02d.txt",fDirectory->Data(), iq2+1);

    PdfTable* table = (!DrawBand)? new PdfTable(filename.Data()) : new PdfErrorTables(fDirectory->Data(),iq2+1,kTRUE);

    Int_t    nx = table->GetNx();
    if (nx > 0 ) {    
      Double_t q2 = table->GetQ2();
      fNQ2Files++;                   // Total number of Q2 
      fQ2Value[iq2] = q2;
      //  Prepare graphs:

      fPdfs [(Int_t)kGluon]->AddLast(table->GetPDFGraph("g"));
      fPdfs [(Int_t)kU]    ->AddLast(table->GetPDFGraph("U"));
      fPdfs [(Int_t)kD]    ->AddLast(table->GetPDFGraph("D"));
      fPdfs [(Int_t)kUv]   ->AddLast(table->GetPDFGraph("u_val"));
      fPdfs [(Int_t)kDv]   ->AddLast(table->GetPDFGraph("d_val"));
      fPdfs [(Int_t)kUb]   ->AddLast(table->GetPDFGraph("Ubar"));
      fPdfs [(Int_t)kDb]   ->AddLast(table->GetPDFGraph("Dbar"));
      fPdfs [(Int_t)kSea]  ->AddLast(table->GetPDFGraph("sea"));
      fPdfs [(Int_t)kS]    ->AddLast(table->GetPDFGraph("str"));
      fPdfs [(Int_t)kC]    ->AddLast(table->GetPDFGraph("chm"));
      fPdfs [(Int_t)kB]    ->AddLast(table->GetPDFGraph("bot"));
      
      delete table;

    }


    else {
      // No more Q2 file to read
      break;
    }

  }
  
  
  /*
  Double_t x, gluon, U, D, d_Ubar, d_Dbar, umin, dmin, sea, u_sea, d_sea, str, chm, bot;
  TString* filename = new TString;
  for(Int_t iq2=0; iq2<1; iq2++) {
    filename->Form("%s/pdfs_q2val_%02d.txt",fDirectory->Data(), iq2+1);
    ifstream infile(filename->Data());
    if(!infile.is_open()) break;

    
    // Read Q2 value
    Int_t nx;
    Double_t xmin,xmax;
    infile >> fQ2Value[iq2] >> nx >> xmin >> xmax;
    
    fNQ2Files++;

    fNpoints = nx;

    Double_t * eUp = NULL;
    Double_t * eDn = NULL;
    Double_t * Cent = NULL;
    Double_t * erUp = NULL;
    Double_t * erDn = NULL;

    if (nBand == 0) {
      for(Int_t ipdf = 0; ipdf < fNpdfs; ipdf++)
	((TObjArray*) fPdfs[ipdf])->AddLast(new TGraph(fNpoints));
    }
    else {
      eUp  = new Double_t [ nx*NColumn*nBand];
      eDn  = new Double_t [ nx*NColumn*nBand];

      Cent = new Double_t [ nx*NColumn];
      erUp = new Double_t [ nx*NColumn];
      erDn = new Double_t [ nx*NColumn];
      
      Double_t tmp;
      Int_t itmp;

      // Read the central
      filename->Form("%s/pdfs_q2val_%02d.txt",fDirectory->Data(), iq2+1);
      ifstream infile_c(filename->Data());
      infile_c >> tmp >> itmp >> tmp >> tmp;
      
      for ( Int_t ix=0; ix<nx; ix++) {
	for ( Int_t ipdf=0; ipdf < NColumn; ipdf++ ) {
	  Int_t iref = ipdf+ix*NColumn;
	  infile_c >> Cent[iref];
	}
	// Prepare Uv and Dv
	Int_t irefUv = 6 + ix*NColumn;
	Int_t irefDv = 7 + ix*NColumn;
	
	Int_t irefusea = 9 + ix*NColumn;
	Int_t irefdsea = 10 + ix*NColumn;
	
	Int_t irefchr  = 12 + ix*NColumn;
	Int_t irefbot  = 13 + ix*NColumn;
	Cent[irefUv] = Cent[irefUv] - Cent[irefusea] - Cent[irefchr];
	Cent[irefDv] = Cent[irefDv] - Cent[irefdsea] - Cent[irefbot];
      }
      
      for(Int_t ipdf = 0; ipdf < fNpdfs; ipdf++) {
	((TObjArray*) fPdfs[ipdf])->AddLast(new TGraphAsymmErrors(fNpoints));
	// Read the bands

	

	for (Int_t iband=0; iband<nBand; iband++) {
	    filename->Form("%s/pdfs_q2val_s%02dm_%02d.txt",fDirectory->Data(), iband+1, iq2+1);
	    ifstream infile_m(filename->Data());
	    infile_m >> tmp >> itmp >> tmp >> tmp;

	    filename->Form("%s/pdfs_q2val_s%02dp_%02d.txt",fDirectory->Data(), iband+1, iq2+1);
	    ifstream infile_p(filename->Data());
	    infile_p >> tmp >> itmp >> tmp >> tmp;

	    for ( Int_t ix=0; ix<nx; ix++) {
	      for ( Int_t ipdf=0; ipdf < NColumn; ipdf++ ) {
		Int_t iref = ipdf+ix*NColumn+(NColumn*nx)*iband;
		infile_m >> eDn[iref];
		infile_p >> eUp[iref];
	      }
	    }


	    for ( Int_t ix=0; ix<nx; ix++) {
	      Int_t irefUv = 6 + ix*NColumn +(NColumn*nx)*iband;
	      Int_t irefDv = 7 + ix*NColumn +(NColumn*nx)*iband;

	      Int_t irefusea = 9 + ix*NColumn +(NColumn*nx)*iband;
	      Int_t irefdsea = 10 + ix*NColumn +(NColumn*nx)*iband;

	      Int_t irefchr  = 12 + ix*NColumn +(NColumn*nx)*iband;
	      Int_t irefbot  = 13 + ix*NColumn +(NColumn*nx)*iband;

	      eDn[irefUv] = eDn[irefUv] - eDn[irefusea] - eDn[irefchr];
	      eDn[irefDv] = eDn[irefDv] - eDn[irefdsea] - eDn[irefbot];

	      eUp[irefUv] = eUp[irefUv] - eUp[irefusea] - eUp[irefchr];
	      eUp[irefDv] = eUp[irefDv] - eUp[irefdsea] - eUp[irefbot];

	    }

	  }       
      }
      // Calculate the errors:
      for (Int_t ipdf=0; ipdf<NColumn; ipdf++) {
	for (Int_t ix=0; ix<nx; ix++) {
	  Double_t Dn = 0.;
	  Double_t Up = 0.;
	  Double_t cv = Cent[ipdf+ix*NColumn];

	  for (Int_t iband=0; iband<nBand; iband++) {
	    Int_t iref = ipdf+ix*NColumn+(NColumn*nx)*iband;

	    Double_t d1 = eDn[iref] - cv;
	    Double_t d2 = eUp[iref] - cv;
	    Double_t d = d1>d2 ?  d1 : d2;
	    if (d>0) {
	      Up += d*d;
	    }

	    d1 = cv - eDn[iref];
	    d2 = cv - eUp[iref];
	    d = d1>d2 ?  d1 : d2;
	    if (d>0) {
	      Dn += d*d;
	    }
	  }
	  erUp[ipdf+ix*NColumn] = sqrt(Up);
	  erDn[ipdf+ix*NColumn] = sqrt(Dn);
	}
      }
    }


    for (Int_t i = 0; i < fNpoints; i++){
      infile >> x >> gluon >> U >> D >> d_Ubar >> d_Dbar >> umin >> dmin >> sea >> u_sea >> d_sea >> str >> chm >> bot;
      SetPdfPoint((Int_t)kGluon, iq2, i, x, gluon);
      SetPdfPoint((Int_t)kU    , iq2, i, x, U);
      SetPdfPoint((Int_t)kD    , iq2, i, x, D);
      SetPdfPoint((Int_t)kUv   , iq2, i, x, U - u_sea - chm);
      SetPdfPoint((Int_t)kDv   , iq2, i, x, D - d_sea - str - bot);
      SetPdfPoint((Int_t)kUb   , iq2, i, x, d_Ubar);
      SetPdfPoint((Int_t)kDb   , iq2, i, x, d_Dbar);
      SetPdfPoint((Int_t)kSea  , iq2, i, x, sea);
      SetPdfPoint((Int_t)kS    , iq2, i, x, str);
      SetPdfPoint((Int_t)kC    , iq2, i, x, chm);
      SetPdfPoint((Int_t)kB    , iq2, i, x, bot);

      if (nBand>0) {
	Int_t iref = i*NColumn;
	SetPdfError((Int_t)kGluon, iq2, i, x, erUp[iref+1],erDn[iref+1]);
	SetPdfError((Int_t)kU    , iq2, i, x, erUp[iref+2],erDn[iref+2]);
	SetPdfError((Int_t)kD    , iq2, i, x, erUp[iref+3],erDn[iref+3]);
	SetPdfError((Int_t)kUv   , iq2, i, x, erUp[iref+6],erDn[iref+6]);
	SetPdfError((Int_t)kDv   , iq2, i, x, erUp[iref+7],erDn[iref+7]);
	SetPdfError((Int_t)kUb   , iq2, i, x, erUp[iref+4],erDn[iref+4]);
	SetPdfError((Int_t)kDb   , iq2, i, x, erUp[iref+5],erDn[iref+5]);
	SetPdfError((Int_t)kSea  , iq2, i, x, erUp[iref+8],erDn[iref+1]);
	SetPdfError((Int_t)kS    , iq2, i, x, erUp[iref+11],erDn[iref+11]);
	SetPdfError((Int_t)kC    , iq2, i, x, erUp[iref+12],erDn[iref+12]);
	SetPdfError((Int_t)kB    , iq2, i, x, erUp[iref+13],erDn[iref+13]);
      }
    }
  }  delete filename;
  */
}



Int_t Output::GetNsets() {
  return fDataSets.size();
}

TGraph* Output::GetPdf(Output::pdf ipdf, Int_t Q2bin) {
  if(ipdf >= fNpdfs) {cout << "GetPdf, wrong ipdf: "<< ipdf << " "<<fNpdfs << endl; exit(1);}
  if(Q2bin >= fPdfs[ipdf]->GetEntries()) {cout << "GetPdf, wrong iq2: "<< Q2bin << " "<< ipdf<< endl; exit(1);}

  return ((TGraph*)fPdfs[ipdf]->At(Q2bin));
}

void Output::SetPdfPoint(Int_t ipdf, Int_t iq2, Int_t ipoint, Double_t x, Double_t y) {
  if(ipdf >= fNpdfs) {cout << "SetPdfPoint, wrong ipdf: "<< ipdf << endl; return;}
  if(iq2 >= fPdfs[ipdf]->GetEntries()) {cout << "SetPdfPoint, wrong iq2: "<< iq2 << endl; return;}
  ((TGraph*)fPdfs[ipdf]->At(iq2))->SetPoint(ipoint, x, y);
}

void Output::SetPdfError(Int_t ipdf, Int_t iq2, Int_t ipoint, Double_t x, Double_t Up, Double_t Dn) {
  if(ipdf >= fNpdfs) {cout << "SetPdfPoint, wrong ipdf: "<< ipdf << endl; return;}
  if(iq2 >= fPdfs[ipdf]->GetEntries()) {cout << "SetPdfPoint, wrong iq2: "<< iq2 << endl; return;}
  ((TGraphAsymmErrors*)fPdfs[ipdf]->At(iq2))->SetPointEYlow(ipoint,  Dn);
  ((TGraphAsymmErrors*)fPdfs[ipdf]->At(iq2))->SetPointEYhigh(ipoint,  Up);
}


Int_t Output::PrepareDataSets() {

  TString* filename = new TString;
  filename->Form("%s/fittedresults.txt",fDirectory->Data());

  Double_t v1, v2, v3, data, uncorrerr, toterr, theory, theory_mod,pull;
  Int_t dataset;
  Char_t buffer[120];
  TString V1, V2, V3, str, Name;

  ifstream infile(filename->Data());
  if(!infile.is_open()) { cout << "Output::PrepareDataSets: can not open file %s" << filename->Data()<<endl; return 1;}
  infile.getline(buffer, 120);  // number of data sets
  str.Form(buffer); 
  fNDataSets = str.Atoi();
  //fDataSets
  //  fDataSets = new DataSet[fNDataSets];

  infile.getline(buffer, 120);
  str.Form(buffer); 
  dataset = str.Atoi();
  // v1     v2    v3    data     +- uncorr.err   +-toterr      theory  theory_mod     pull     dataset
  while(!infile.eof()) {
    infile.getline(buffer, 120);
    Name.Form(buffer);
    infile.getline(buffer, 120);
    //    infile.getline(buffer, 120); 
    str.Form(buffer);
    TObjArray* array = str.Tokenize(" ");
    if(array->GetEntries() < 3) {cout << "something is wrong in fittedresults.txt" << endl; delete array; continue;}
    
    V1.Form(((TObjString*)array->At(0))->GetString().Data());
    V2.Form(((TObjString*)array->At(1))->GetString().Data());
    V3.Form(((TObjString*)array->At(2))->GetString().Data());
    delete array;

    DataSet* NewDataSet = NULL;

    if(!NewDataSet) {
      NewDataSet = new DataSet(dataset, Name.Data(), V1.Data(), V2.Data(), V3.Data());
      fDataSets.push_back(NewDataSet);
    }

    while(1) {
      infile.getline(buffer, 120);
      str.Form(buffer);
      TObjArray* array = str.Tokenize(" ");
      if(array->GetEntries() == 1) {dataset = ((TObjString*)array->At(0))->GetString().Atoi(); delete array; break;}
      if(array->GetEntries() != 10) {delete array; break;}

      
      v1        = ((TObjString*)array->At(0))->GetString().Atof();
      v2        = ((TObjString*)array->At(1))->GetString().Atof();
      v3        = ((TObjString*)array->At(2))->GetString().Atof();
      data      = ((TObjString*)array->At(3))->GetString().Atof();
      uncorrerr = ((TObjString*)array->At(4))->GetString().Atof();
      toterr    = ((TObjString*)array->At(5))->GetString().Atof();
      theory    = ((TObjString*)array->At(6))->GetString().Atof();
      theory_mod  = ((TObjString*)array->At(7))->GetString().Atof();
      pull      = ((TObjString*)array->At(8))->GetString().Atof();
      dataset   = ((TObjString*)array->At(9))->GetString().Atoi();	
      delete array;

      // cout << "haha" << dataset << " "<< data << endl;

      NewDataSet->AddPoint(v1, v2, v3, data, uncorrerr, toterr, theory, theory_mod, pull);

      fPull->Fill(pull);
    }
  }    
  
  delete filename;
  return 0;
}

 const Double_t Output::GetQ2Value(Int_t iQ2bin) {
  if ( (iQ2bin < 0) || (iQ2bin> GetNQ2Files())) {
    cout << "Wrong Q2 bin" << iQ2bin<< " stop "<<endl;
    exit(1);
  };
  return fQ2Value[iQ2bin];
}




