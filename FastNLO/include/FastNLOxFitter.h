// Author: Daniel Britzger
// DESY, 20/04/2012

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  fastNLO_reader_2.1.0                                                //
//  D. Britzger, T. Kluge, K. Rabbertz, F. Stober, M. Wobisch           //
//                                                                      //
//  The projects web page can be found at:                              //
//    http://projects.hepforge.org/fastnlo                              //
//                                                                      //
//  If you use this code, please cite:                                  //
//    T. Kluge, K. Rabbertz and M. Wobisch, hep-ph/0609285              //
//    D. Britzger, T. Kluge, K. Rabbertz, F. Stober, M. Wobisch,        //
//       arXiv:1109.1310                                                //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef FASTNLOXFITTER
#define FASTNLOXFITTER

#include "fastnlotk/fastNLOReader.h"



class FastNLOxFitter : public fastNLOReader {

public:
   FastNLOxFitter(std::string name);

protected:
   // inherited functions
   double EvolveAlphas(double Q ) const ;
   bool InitPDF();
   std::vector<double> GetXFX(double xp, double muf) const ;
};
#endif
