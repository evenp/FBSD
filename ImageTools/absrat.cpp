#include "absrat.h"


AbsRat::AbsRat ()
{
  num = 1;
  den = 0;
}


AbsRat::AbsRat (int numerator, int denominator)
{
  num = (numerator < 0 ? - numerator : numerator);
  den = (denominator < 0 ? - denominator : denominator);
}


AbsRat::AbsRat (const AbsRat &rat)
{
  num = rat.num;
  den = rat.den;
}


void AbsRat::attractsTo (const AbsRat &val, const AbsRat &ratio)
{
  if (val.den != 0)
    num = (ratio.den * num * val.den
           - (num * val.den - den * val.num) * ratio.num)
          / (ratio.den * val.den);
}


void AbsRat::sticksTo (const AbsRat &val)
{
  if (val.den != 0) num = (den * val.num) / val.den;
}


void AbsRat::mult (const AbsRat &val)
{
  num *= val.num;
  den *= val.den;
}
