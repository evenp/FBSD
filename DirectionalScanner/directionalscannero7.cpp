#include <cstdlib>
#include "directionalscannero7.h"



DirectionalScannerO7::DirectionalScannerO7 (
                          int xmin, int ymin, int xmax, int ymax,
                          int a, int b, int c,
                          int nbs, bool *steps, int sx, int sy)
                    : DirectionalScanner (xmin, ymin, xmax, ymax,
                                          nbs, steps, sx, sy)
{
  this->dla = a;
  this->dlb = b;
  this->dlc2 = c;

  lst1 = steps;
  rst1 = steps;
  lst2 = steps;
  rst2 = steps;
  fs = steps + nbs;
  ltransition = false;
  rtransition = false;
}



DirectionalScannerO7::DirectionalScannerO7 (
                          int xmin, int ymin, int xmax, int ymax,
                          int a, int b, int c1, int c2,
                          int nbs, bool *steps, int cx, int cy)
                    : DirectionalScanner (xmin, ymin, xmax, ymax,
                                          nbs, steps, cx, cy)
{
  this->dla = a;
  this->dlb = b;
  if (c2 < c1)
  {
    this->dlc2 = c1;
    c1 = c2;
  }
  else this->dlc2 = c2;

  // Looking for the central scan start position
  bool *st = steps + nbs;
  do
  {
    if (--st < steps) st = steps + nbs - 1;
    if (*st) lcy --;
    lcx --;
  }
  while (dla * lcx + dlb * lcy > c1);

  rcx = lcx;
  rcy = lcy;
/** ZZZ */
ccx = lcx;
ccy = lcy;
/** ZZZ */
  lst1 = steps;
  rst1 = steps;
  lst2 = steps;
  rst2 = steps;
  fs = steps + nbs;
  ltransition = false;
  rtransition = false;
}



DirectionalScannerO7::DirectionalScannerO7 (
                          int xmin, int ymin, int xmax, int ymax,
                          int a, int b,
                          int nbs, bool *steps, int cx, int cy, int length)
                    : DirectionalScanner (xmin, ymin, xmax, ymax,
                                          nbs, steps, cx, cy)
{
  this->dla = a;
  this->dlb = b;
  fs = steps + nbs;
  int w_2 = (length + 1) / 2;

  // Looking for the central scan start position
  bool *st = steps + nbs;
  for (int i = 0; i < w_2; i++)
  {
    if (--st < steps) st = steps + nbs - 1;
    if (*st) lcy --;
    lcx --;
  }

  // Looking for the upper leaning line
  st = steps;
  while (w_2-- > 0)
  {
    if (*st) cy++;
    cx++;
    if (++st >= fs) st = steps;
  }
  dlc2 = dla * cx + dlb * cy;

  rcx = lcx;
  rcy = lcy;
/** ZZZ */
ccx = lcx;
ccy = lcy;
/** ZZZ */
  lst1 = steps;
  rst1 = steps;
  lst2 = steps;
  rst2 = steps;
  ltransition = false;
  rtransition = false;
}



int DirectionalScannerO7::first (vector<Pt2i> &scan)
{
  int x = lcx, y = lcy;      // Current position coordinates
  bool *nst = steps;         // Current step in scan direction (jpts)

  while ((y < ymin || x < xmin) && dla * x + dlb * y <= dlc2)
  {
    if (*nst) y++;
    x++;
    if (++nst >= fs) nst = steps;
  }
  while (dla * x + dlb * y <= dlc2 && y < ymax && x < xmax)
  {
    scan.push_back (Pt2i (x, y));
    if (*nst) y++;
    x++;
    if (++nst >= fs) nst = steps;
  }
  return ((int) (scan.size ()));
}


int DirectionalScannerO7::nextOnLeft (vector<Pt2i> &scan)
{
  // Prepares the next scan
  scan.clear ();
  if (ltransition)
  {
    lcx --;
    if (--lst2 < steps) lst2 = fs - 1;
    ltransition = false;
  }
  else
  {
    if (--lst1 < steps) lst1 = fs - 1;
    lcy ++;
    if (*lst1)
    {
      if (--lst2 < steps) lst2 = fs - 1;
      if (*lst2)
      {
        if (++lst2 >= fs) lst2 = steps;
        ltransition = true;
      }
      else lcx --;
    }
  }

  // Computes the next scan
  int x = lcx;
  int y = lcy;
  bool *nst = lst2;
  while ((y < ymin || x < xmin) && dla * x + dlb * y <= dlc2)
  {
    if (*nst) y++;
    x++;
    if (++nst >= fs) nst = steps;
  }
  while (dla * x + dlb * y <= dlc2 && y < ymax && x < xmax)
  {
    scan.push_back (Pt2i (x, y));
    if (*nst) y++;
    x++;
    if (++nst >= fs) nst = steps;
  }
  return ((int) (scan.size ()));
}


int DirectionalScannerO7::nextOnRight (vector<Pt2i> &scan)
{
  // Prepares the next scan
  scan.clear ();
  if (rtransition)
  {
    rcy --;
    rtransition = false;
  }
  else
  {
    rcy --;
    if (*rst1)
    {
      rcx ++;
      if (*rst2)
      {
        rcy ++;
        rtransition = true;
      }
      if (++rst2 >= fs) rst2 = steps;
    }
    if (++rst1 >= fs) rst1 = steps;
  }

  // Computes the next scan
  int x = rcx;
  int y = rcy;
  bool *nst = rst2;
  while ((y < ymin || x < xmin) && dla * x + dlb * y <= dlc2)
  {
    if (*nst) y++;
    x++;
    if (++nst == fs) nst = steps;
  }
  while (dla * x + dlb * y <= dlc2 && y < ymax && x < xmax)
  {
    scan.push_back (Pt2i (x, y));
    if (*nst) y++;
    x++;
    if (++nst == fs) nst = steps;
  }
  return ((int) (scan.size ()));
}


Pt2i DirectionalScannerO7::locate (const Pt2i &pt) const
{
  int x = ccx, y = ccy;      // Current position coordinates
  bool *nst = steps;         // Current step in scan direction (jpts)
  int cx = pt.x () - x, cy = 0;
  bool *st1 = steps;
  bool *st2 = steps;
  
  if (cx >= 0)
  {
    // Climbs the first scan up
    while (x < pt.x ())
    {
      if (*nst) y++;
      x++;
      if (++nst >= fs) nst = steps;
    }
  }
  else
  {
    // Climbs the first scan down
    while (x < pt.x ())
    {
      x--;
      if (--nst < steps) nst = fs - 1;
      if (*nst) y--;
    }
  }
  cy = y - pt.y ();

  // Comes back to scan origin
  x = ccx;
  y = ccy;
  int ny = cy;
  bool trans = false;
  while (ny != 0)
  {
    // Jumps leftwards along scan bound
    if (cy < 0)
    {
      if (trans)
      {
        y --;
        trans = false;
      }
      else
      {
        if (--st1 < steps) st1 = fs - 1;
        y --;
        if (*st1)
        {
          x --;
          if (*st2)
          {
            y ++;
            trans = true;
          }
          if (++st2 >= fs) st2 = steps;
        }
      }
      ny ++;
    }
    else
    // Jumps rightwards along scan bound
    {
      if (trans)
      {
        x ++;
        if (--st2 < steps) st2 = fs - 1;
        trans = false;
      }
      else
      {
        y ++;
        if (*st1)
        {
          if (--st2 < steps) st2 = fs - 1;
          if (*st2)
          {
            if (++st2 >= fs) st2 = steps;
            trans = true;
          }
          else x ++;
        }
        if (++st1 >= fs) st1 = steps;
      }
      ny --;
    }
  }

  return (Pt2i (cy, pt.x () - x));
}
