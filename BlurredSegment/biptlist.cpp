#include "biptlist.h"


BiPtList::BiPtList (Pt2i pt)
{
  pts.push_back (pt);
  start = 0;
  cpt = 1;
}


BiPtList::~BiPtList ()
{
}


void BiPtList::addFront (Pt2i pt)
{
  pts.push_front (pt);
  start++;
  cpt++;
}


void BiPtList::addBack (Pt2i pt)
{
  pts.push_back (pt);
  cpt++;
}


void BiPtList::removeFront (int n)
{
  if (n >= frontSize ()) n = frontSize () - 1; // We keep at least one point
  for (int i = 0; i < n; i++) pts.pop_front ();
  cpt -= n;
  start -= n;
  if (start < 0) start = 0; // Theoretically impossible
}


void BiPtList::removeBack (int n)
{
  if (n >= backSize ()) n = backSize () - 1;  // We keep at least one point
  for (int i = 0; i < n; i++) pts.pop_back ();
  cpt -= n;
  if (start >= cpt) start = cpt - 1;  // Theoretically impossible
}


void BiPtList::findExtrema (int &xmin, int &ymin, int &xmax, int &ymax) const
{
  deque<Pt2i>::const_iterator it = pts.begin ();
  xmin = it->x ();
  ymin = it->y ();
  xmax = it->x ();
  ymax = it->y ();
  while (it != pts.end ())
  {
    if (xmin > it->x ()) xmin = it->x ();
    if (xmax < it->x ()) xmax = it->x ();
    if (ymin > it->y ()) ymin = it->y ();
    if (ymax < it->y ()) ymax = it->y ();
    it++;
  }
}


vector<Pt2i> BiPtList::frontToBackPoints () const
{
  vector<Pt2i> res;
  for (deque<Pt2i>::const_iterator it = pts.begin ();
       it != pts.end (); it++)
    res.push_back (*it);
  return (res);
}


vector<Pt2i> *BiPtList::vide () const
{
  return (new vector<Pt2i> ());
}


vector<Pt2i> *BiPtList::frontPoints () const
{
  // Fournis du bord au centre : pertinent ?
  vector<Pt2i> *res = new vector<Pt2i> ();
  deque<Pt2i>::const_iterator it = pts.begin ();
  for (int i = 0; i < start; i++) res->push_back (*it++);
  return res;
}


vector<Pt2i> *BiPtList::backPoints () const
{
  vector<Pt2i> *res = new vector<Pt2i> ();
  deque<Pt2i>::const_iterator it = pts.begin ();
  it += start + 1;
  for (int i = 0; i < cpt - start - 1; i++) res->push_back (*it++);
  return res;
}
