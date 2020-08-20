/*******************************************************************\
 
Module: A special command line object for Bruce's C Compiler
 
Author: Michael Tautschnig
 
Date: July 2016
 
\*******************************************************************/

/// \file
///  A special command line object for Bruce's C Compiler Author: Michael
///   Tautschnig Date: July 2016

#ifndef CPROVER_BISCUITS_H
#define CPROVER_BISCUITS_H

#include "goto_cc_cmdline.h"

class bcc_cmdlinet:public goto_cc_cmdlinet
{
public:
  // overload
  virtual bool parse(int, const char**);

  bcc_cmdlinet()
  {
  }
};

#endif // CPROVER_BISCUITS_H
