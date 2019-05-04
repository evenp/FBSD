#include "blurredsegmentproto.h"


BlurredSegmentProto::BlurredSegmentProto (int maxWidth, Pt2i pix)
{
  this->maxWidth.set (maxWidth);
  plist = new BiPtList (pix);
  leftOK = false;
  rightOK = false;
  bsFlat = false;
  bsOK = false;
  convexhull = NULL;
  chChanged = false;
  dss = NULL;
}


BlurredSegmentProto::BlurredSegmentProto (int maxWidth, Pt2i center,
                  const vector<Pt2i> &leftPts, const vector<Pt2i> &rightPts)
{
  this->maxWidth.set (maxWidth);
  plist = new BiPtList (center);
  leftOK = false;
  rightOK = false;
  bsFlat = false;
  bsOK = false;
  convexhull = NULL;
  chChanged = false;
  dss = NULL;

  vector<Pt2i>::const_iterator itr = rightPts.begin ();
  vector<Pt2i>::const_iterator itl = leftPts.begin ();
  bool scanningRight = true;
  bool scanningLeft = true;
  while (scanningRight || scanningLeft)
  {
    if (scanningRight)
    {
      if (itr == rightPts.end ()) scanningRight = false;
      else
        if (! addRight (*itr++)) scanningRight = false;
    }
    if (scanningLeft)
    {
      if (itl == leftPts.end ()) scanningLeft = false;
      else
        if (! addLeft (*itl++)) scanningLeft = false;
    }
  }
}


BlurredSegmentProto::~BlurredSegmentProto ()
{
  if (convexhull != NULL) delete convexhull;
}


AbsRat BlurredSegmentProto::strictThickness () const
{
  return (convexhull != NULL ? convexhull->thickness () : AbsRat (0, 1));
}


AbsRat BlurredSegmentProto::digitalThickness () const
{
  if (bsOK)
  {
    Pt2i s, e, v;
    convexhull->antipodalEdgeAndVertex (s, e, v);
    DigitalStraightLine l (s, e, v);
    return (AbsRat (l.width (), l.period ()));
  }
  return (AbsRat (1, 1));
}


DigitalStraightLine *BlurredSegmentProto::getLine () const
{
  if (bsOK)
  {
    Pt2i s, e, v;
    convexhull->antipodalEdgeAndVertex (s, e, v);
    return (new DigitalStraightLine (s, e, v));
  }
  if (bsFlat || leftOK || rightOK)
    return (new DigitalStraightLine (getLastLeft (), getLastRight (),
                                     DigitalStraightLine::DSL_THIN));
  return (NULL); // No line if only one point
}


bool BlurredSegmentProto::addLeft (Pt2i pix)
{
  if (bsOK)     // convexhull defined
  {
    bool res = addPoint (pix, true);
    return (res);
  }

  else if (bsFlat)    // leftOK && rightOK
  {
    AbsRat height = plist->heightToEnds (pix);
    if (height.greaterThan (maxWidth)) return false;
    if (height.numerator () != 0)
    {
      convexhull = new ConvexHull (pix, plist->frontPoint (),
                                        plist->backPoint ());
      bsOK = true;
    }
    plist->addFront (pix);
  }

  else if (leftOK)    // thus ! rightOK
  {
    AbsRat height = plist->heightToEnds (pix);
    if (height.greaterThan (maxWidth)) return false;
    if (height.numerator () == 0) bsFlat = true;
    else
    {
      convexhull = new ConvexHull (pix, plist->frontPoint (),
                                        plist->backPoint ());
      bsOK = true;
    }
    plist->addFront (pix);
  }

  else if (rightOK)
  {
    AbsRat height = plist->heightToEnds (pix);
    if (height.greaterThan (maxWidth)) return false;
    if (height.numerator () == 0) bsFlat = true;
    else
    {
      convexhull = new ConvexHull (pix, plist->frontPoint (),
                                        plist->backPoint ());
      bsOK = true;
    }
    plist->addFront (pix);
  }

  else   // only the central point is ok
  {
    plist->addFront (pix);
    leftOK = true;
  }

  chChanged = true;
  return true;
}


bool BlurredSegmentProto::addRight (Pt2i pix)
{
  if (bsOK)     // bs != NULL
  {
    bool res = addPoint (pix, false);
    return (res);
  }

  else if (bsFlat)    // leftOK && rightOK
  {
    AbsRat height = plist->heightToEnds (pix);
    if (height.greaterThan (maxWidth)) return false;
    if (height.numerator () != 0)
    {
      convexhull = new ConvexHull (plist->frontPoint (),
                                   plist->backPoint (), pix);
      bsOK = true;
    }
    plist->addBack (pix);
  }

  else if (rightOK)    // thus ! leftOK
  {
    AbsRat height = plist->heightToEnds (pix);
    if (height.greaterThan (maxWidth)) return false;
    if (height.numerator () == 0) bsFlat = true;
    else
    {
      convexhull = new ConvexHull (plist->frontPoint (),
                                   plist->backPoint (), pix);
      bsOK = true;
    }
    plist->addBack (pix);
  }

  else if (leftOK)
  {
    AbsRat height = plist->heightToEnds (pix);
    if (height.greaterThan (maxWidth)) return false;
    if (height.numerator () == 0) bsFlat = true;
    else
    {
      convexhull = new ConvexHull (plist->frontPoint (),
                                   plist->backPoint (), pix);
      bsOK = true;
    }
    plist->addBack (pix);
  }

  else   // only the central point is ok
  {
    plist->addBack (pix);
    rightOK = true;
  }

  chChanged = true;
  return true;
}


bool BlurredSegmentProto::addPoint (Pt2i p, bool onleft)
{
  bool inserted = convexhull->addPointDS (p, onleft);
  if ((strictThickness ()).greaterThan (maxWidth))
  {
    if (inserted) convexhull->restore ();
    return false;
  }
 
  if (onleft) plist->addFront (p);
  else plist->addBack (p);
  chChanged = true;
  return true;
}


void BlurredSegmentProto::removeLeft (int n)
{
  if (bsOK) plist->removeFront (n);
}


void BlurredSegmentProto::removeRight (int n)
{
  if (bsOK) plist->removeBack (n);
}
 

Vr2i BlurredSegmentProto::getSupportVector ()
{
  if (bsOK)
  {
    Pt2i s, e, v;
    convexhull->antipodalEdgeAndVertex (s, e, v);
    return (s.vectorTo (e));
  }
  if (bsFlat || leftOK || rightOK)
    return (getLastLeft().vectorTo (getLastRight ()));
  return (Vr2i (1, 0));    // hardly better with only one point !
}


BlurredSegment *BlurredSegmentProto::endOfBirth ()
{
  DigitalStraightSegment *seg = NULL;
  if (bsOK)
  {
    int xmin, ymin, xmax, ymax;
    plist->findExtrema (xmin, ymin, xmax, ymax);
    Pt2i s, e, v;
    convexhull->antipodalEdgeAndVertex (s, e, v);
    seg = new DigitalStraightSegment (s, e, v, xmin, ymin, xmax, ymax);
  }
  else if (bsFlat || rightOK || leftOK)
  {
    Pt2i llast = plist->frontPoint ();
    Pt2i rlast = plist->backPoint ();
    if (llast.equals (rlast)) // Strange, should not be flat, rightok or leftok
    {
      plist = NULL;
      return (NULL);
    }
    int xmin = llast.x ();
    if (rlast.x () < llast.x ()) xmin = rlast.x ();
    int ymin = llast.y ();
    if (rlast.y () < llast.y ()) ymin = rlast.y ();
    int xmax = llast.x ();
    if (rlast.x () > llast.x ()) xmax = rlast.x ();
    int ymax = llast.y ();
    if (rlast.y () > llast.y ()) ymax = rlast.y ();
    seg = new DigitalStraightSegment (llast, rlast,
                                      DigitalStraightLine::DSL_THIN,
                                      xmin, ymin, xmax, ymax);
  }
  else return (NULL);
  Pt2i aps (-1, -1), ape (-1, -1), apv (-1, -1);
  if (convexhull != NULL)
    convexhull->antipodalEdgeAndVertex (aps, ape, apv);
  BlurredSegment *bbs = new BlurredSegment (plist, seg, aps, ape, apv);
  plist = NULL;  // NECESSARY TO AVOID CONTENTS CLEARANCE !!!
  return (bbs);
}
