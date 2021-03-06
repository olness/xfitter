#pragma once

#include "ReactionBaseDISNC.h"

/*
 *   @class' ReactionFONLL_DISNC
 *
 *  @brief A wrapper class for FONLL_DISNC reaction 
 *
 *  Based on the ReactionTheory class. Reads options produces 3d cross section.
 *
 *  @version 0.1
 *  @date 2017-11-29
 */

class ReactionFONLL_DISNC : public ReactionBaseDISNC
{
 public:
  ReactionFONLL_DISNC() {};
  //~ReactionFONLL_DISNC() {};
  //~ReactionFONLL_DISNC(const ReactionFONLL_DISNC &) {};
  //ReactionFONLL_DISNC & operator = (const ReactionAFONLL_DISNC &r) { return *(new ReactionFONLL_DISNC(r)); };

  virtual string getReactionName() const { return "FONLL_DISNC"; };
  int initAtStart(const string &);
  virtual void initAtIteration() override;

 protected:
  virtual void F2  BASE_PARS override;
  virtual void FL  BASE_PARS override;
  virtual void xF3 BASE_PARS override;
  
 private:
  map <int,valarray<double>> _f2fonll;
  map <int,valarray<double>> _flfonll;
  map <int,valarray<double>> _f3fonll;
};

