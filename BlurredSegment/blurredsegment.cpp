#include "blurredsegment.h"


BlurredSegment::BlurredSegment ()
{
  plist = NULL;
  dss = NULL;
}


BlurredSegment::BlurredSegment (BiPtList *ptlist, DigitalStraightSegment *seg,
                                const Pt2i &aps, const Pt2i &ape,
                                const Pt2i &apv)
{
  plist = ptlist;
  dss = seg;
  laps.set (aps);
  lape.set (ape);
  lapv.set (apv);
}


BlurredSegment::~BlurredSegment ()
{
  if (plist != NULL) delete plist;
  if (dss != NULL) delete dss;
}


AbsRat BlurredSegment::minimalWidth () const
{
  return (AbsRat (dss->width (), dss->period ()));
}


vector<Pt2i> BlurredSegment::getAllPoints () const
{
  return (plist->frontToBackPoints ());
}


vector<Pt2i> *BlurredSegment::getAllRight () const
{
  return (plist->backPoints ());
}


vector<Pt2i> *BlurredSegment::getAllLeft () const
{
  return (plist->frontPoints ());
}


int BlurredSegment::size () const
{
  return (plist->size ());
}


vector<Pt2i> BlurredSegment::getStartPt () const
{
  vector<Pt2i> res;
  res.push_back (plist->initialPoint ());
  return res;
}


const Pt2i BlurredSegment::getLastRight () const
{
  return (plist->backPoint ());
}


const Pt2i BlurredSegment::getLastLeft () const
{
  return (plist->frontPoint ());
}


int BlurredSegment::getSquarredLength () const
{
  Pt2i rp = plist->backPoint ();
  Pt2i lp = plist->frontPoint ();
  return ((rp.x () - lp.x ()) * (rp.x () - lp.x ())
          + (rp.y () - lp.y ()) * (rp.y () - lp.y ()));
}


const Pt2i BlurredSegment::getMiddle () const
{
  Pt2i rp = plist->backPoint ();
  Pt2i lp = plist->frontPoint ();
  return (Pt2i ((rp.x () + lp.x ()) / 2, (rp.y () + lp.y ()) / 2));
}


Vr2i BlurredSegment::getSupportVector ()
{
  return (dss->supportVector ());
}


Vr2i BlurredSegment::boundingBoxSize () const
{
  int xmin, ymin, xmax, ymax;
  plist->findExtrema (xmin, ymin, xmax, ymax);
  return (Vr2i (xmax - xmin, ymax - ymin));
}


vector <vector <Pt2i> > BlurredSegment::connectedComponents () const
{
  vector <vector <Pt2i> > ccs;
  vector <Pt2i> pts = getAllPoints ();
  if (pts.size () > 1)
  {
    vector <Pt2i> cc;
    bool started = false;
    vector <Pt2i>::const_iterator it = pts.begin ();
    Pt2i pix = *it++;
    while (it != pts.end ())
    {
      if (it->isConnectedTo (pix))
      {
        if (! started)
        {
          cc.push_back (pix);
          started = true;
        }
        cc.push_back (*it);
      }
      else
      {
        if (started)
        {
          ccs.push_back (cc);
          cc.clear ();
          started = false;;
        }
      }
      pix = *it++;
    }
  }
  return ccs;
}


int BlurredSegment::countOfConnectedPoints () const
{
  int count = 0;
  vector <Pt2i> pts = getAllPoints ();
  if (pts.size () > 1)
  {
    bool started = false;
    vector <Pt2i>::const_iterator it = pts.begin ();
    Pt2i pix = *it++;
    while (it != pts.end ())
    {
      if (it->isConnectedTo (pix))
      {
        if (started) count ++;
        else
        {
          count += 2;
          started = true;
        }
      }
      else
      {
        if (started) started = false;;
      }
      pix = *it++;
    }
  }
  return count;
}


int BlurredSegment::countOfConnectedComponents () const
{
  int count = 0;
  vector <Pt2i> pts = getAllPoints ();
  if (pts.size () > 1)
  {
    bool started = false;
    vector <Pt2i>::const_iterator it = pts.begin ();
    Pt2i pix = *it++;
    while (it != pts.end ())
    {
      if (it->isConnectedTo (pix))
      {
        if (! started)
        {
          count ++;
          started = true;
        }
      }
      else
      {
        if (started) started = false;;
      }
      pix = *it++;
    }
  }
  return count;
}


int BlurredSegment::countOfConnectedPoints (int min) const
{
  int count = 0;
  vector <Pt2i> pts = getAllPoints ();
  if (pts.size () > 1)
  {
    int cpt = 1;
    vector <Pt2i>::const_iterator it = pts.begin ();
    Pt2i pix = *it++;
    while (it != pts.end ())
    {
      if (it->isConnectedTo (pix))
      {
        if (++cpt == min) count += min;
        else if (cpt > min) count ++;
      }
      else cpt = 1;
      pix = *it++;
    }
  }
  return count;
}


int BlurredSegment::countOfConnectedComponents (int min) const
{
  int count = 0;
  vector <Pt2i> pts = getAllPoints ();
  if (pts.size () > 1)
  {
    int cpt = 1;
    vector <Pt2i>::const_iterator it = pts.begin ();
    Pt2i pix = *it++;
    while (it != pts.end ())
    {
      if (it->isConnectedTo (pix))
      {
        if (++cpt == min) count ++;
      }
      else cpt = 1;;
      pix = *it++;
    }
  }
  return count;
}


vector <vector <Pt2i> >
BlurredSegment::getConnectedComponents () const
{
  vector < vector <Pt2i> > res;
  vector <Pt2i> pts = getAllPoints ();
  if (pts.size () > 1)
  {
    vector <Pt2i>::const_iterator bit = pts.begin ();
    vector <Pt2i>::const_iterator eit = pts.end ();
    while (bit != eit)
    {
      vector <Pt2i> lres;
      Pt2i pix = *bit++;
      bool compose = true;
      do
      {
        lres.push_back (pix);
        compose = bit->isConnectedTo (pix);
        if (compose) pix = *bit++;
      }
      while (compose && bit != eit);
      if (compose) lres.push_back (pix);
      res.push_back (lres);
    }
  }
  return res;
}
