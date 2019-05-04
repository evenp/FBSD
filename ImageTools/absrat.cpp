#include "absrat.h"


AbsRat::AbsRat ()
{
  numer = 1;
  denom = 0;
}


AbsRat::AbsRat (int numerator, int denominator)
{
  numer = (numerator < 0 ? - numerator : numerator);
  denom = (denominator < 0 ? - denominator : denominator);
}


AbsRat::AbsRat (const AbsRat &rat)
{
  numer = rat.numer;
  denom = rat.denom;
}


void AbsRat::attractsTo (const AbsRat &val, const AbsRat &ratio)
{
  if (val.denom != 0)
    numer = (ratio.denom * numer * val.denom
           - (numer * val.denom - denom * val.numer) * ratio.numer)
          / (ratio.denom * val.denom);
}


void AbsRat::sticksTo (const AbsRat &val)
{
  if (val.denom != 0) numer = (denom * val.numer) / val.denom;
}


void AbsRat::mult (const AbsRat &val)
{
  numer *= val.numer;
  denom *= val.denom;
}
