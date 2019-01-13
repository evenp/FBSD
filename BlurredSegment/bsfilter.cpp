#include "bsfilter.h"
#include "blurredsegmentproto.h"



BSFilter::BSFilter ()
{
  bsInitialSize = 0;
  bsFinalSize = 0;
}


BSFilter::~BSFilter ()
{
}


void BSFilter::clear ()
{
  leftIn.clear ();
  rightIn.clear ();
  out.clear ();
}


BlurredSegment *BSFilter::filter (BlurredSegment *bs)
{
  leftIn.clear ();
  rightIn.clear ();
  vector<Pt2i> *pts = bs->getAllLeft ();
  vector<Pt2i>::iterator it = pts->end ();
  while (it-- != pts->begin ()) leftIn.push_back (*it);
  pts = bs->getAllRight ();
  it = pts->begin ();
  while (it != pts->end ()) rightIn.push_back (*it++);

  int mw = 1;
  AbsRat sw = bs->minimalWidth ();
  if (sw.denominator () != 0) mw += sw.numerator () / sw.denominator ();
  BlurredSegmentProto resbs (mw, bs->getCenter (), leftIn, rightIn);
  bsFinalSize = resbs.size ();
  return (resbs.endOfBirth ());
}


vector<Pt2i> BSFilter::getAccepted ()
{
  vector<Pt2i> res;
  for (vector<Pt2i>::iterator it = leftIn.begin ();
       it != leftIn.end (); it ++)
    res.push_back (*it);
  for (vector<Pt2i>::iterator it = rightIn.begin ();
       it != rightIn.end (); it ++)
    res.push_back (*it);
  return res;
}
