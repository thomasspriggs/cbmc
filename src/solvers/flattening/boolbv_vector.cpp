/*******************************************************************\

Module:

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/


#include "boolbv.h"

bvt boolbvt::convert_vector(const exprt &expr)
{
  std::size_t width=boolbv_width(expr.type());

  if(width==0)
    return conversion_failed(expr);

  if(expr.type().id()==ID_vector)
  {
    const exprt::operandst &operands=expr.operands();

    bvt bv;
    bv.reserve(width);

    if(!operands.empty())
    {
      std::size_t op_width=width/operands.size();

      for(const exprt &operand : operands)
      {
        const bvt &tmp=convert_bv(operand);

        if(tmp.size()!=op_width)
          throw "convert_vector: unexpected operand width";

        for(const literalt &literal : tmp)
          bv.push_back(literal);
      }
    }

    return bv;
  }

  return conversion_failed(expr);
}
