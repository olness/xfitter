/*!
 @file ReactionTheory.cc
 @date Thu Jan 21 2016
 @author Andrey Sapronov <sapronov@ifh.de>

 Contains implementations of ReactionTheory class member functions.
 */

#include <list>
#include <string>

#include "ReactionTheory.h"
#include "Reaction_FTDY_NC.h"

using std::list;
using std::string;

ReactionTheory::~ReactionTheory()
{
}

ReactionTheory::ReactionTheory(const ReactionTheory &rt) : _subtype(rt._subtype), 
							   _ro(rt._ro),
							   _binsFlags(rt._binsFlags),
							   _dsBins(rt._dsBins)
{
  _val = new valarray<double>(rt._val.size());
}

ReactionTheory &
ReactionTheory::operator=(const ReactionTheory &rt)
{
  _subtype = rt._subtype;
  _ro = rt._ro;
  _binsFlags = rt._binsFlags;
  _dsBins = rt._dsBins;

  _val = new valarray<double>(*(rt._val));

  return *this;
}