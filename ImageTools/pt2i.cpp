#include "pt2i.h"


Pt2i::Pt2i ()
{
  xp = 0;
  yp = 0;
}


Pt2i::Pt2i (int x, int y)
{
  xp = x;
  yp = y;
}


Pt2i::Pt2i (const Pt2i &p)
{
  xp = p.xp;
  yp = p.yp;
}


Vr2i Pt2i::vectorTo (Pt2i p) const
{
  return (Vr2i (p.xp - xp, p.yp - yp));
}


AbsRat Pt2i::triangleRationalHeight (const Pt2i &p1, const Pt2i &p2) const
{
  AbsRat xh = triangleXRationalHeight (p1, p2);
  AbsRat yh = triangleYRationalHeight (p1, p2);
  return (xh.lessThan (yh) ? xh : yh);
}


AbsRat Pt2i::triangleXRationalHeight (const Pt2i &p1, const Pt2i &p2) const
{
  int ax, ay, bx, by, cx, cy;
  if (xp < p1.xp)
    if (xp < p2.xp)
    {
      ax = xp;
      ay = yp;
      if (p1.xp < p2.xp)
      {
        bx = p1.xp;
        by = p1.yp;
        cx = p2.xp;
        cy = p2.yp;
      }
      else
      {
        bx = p2.xp;
        by = p2.yp;
        cx = p1.xp;
        cy = p1.yp;
      }
    }
    else
    {
      ax = p2.xp;
      ay = p2.yp;
      bx = xp;
      by = yp;
      cx = p1.xp;
      cy = p1.yp;
    }
  else
    if (xp < p2.xp)
    {
      ax = p1.xp;
      ay = p1.yp;
      bx = xp;
      by = yp;
      cx = p2.xp;
      cy = p2.yp;
    }
    else {
      cx = xp;
      cy = yp;
      if (p1.xp < p2.xp)
      {
        ax = p1.xp;
        ay = p1.yp;
        bx = p2.xp;
        by = p2.yp;
      }
      else
      {
        ax = p2.xp;
        ay = p2.yp;
        bx = p1.xp;
        by = p1.yp;
      }
    }
  return (AbsRat ((bx - ax) * (cy - ay) - (by - ay) * (cx - ax), cx - ax));
}


AbsRat Pt2i::triangleYRationalHeight (const Pt2i &p1, const Pt2i &p2) const
{
  int ax, ay, bx, by, cx, cy;
  if (yp < p1.yp)
    if (yp < p2.yp)
    {
      ax = xp;
      ay = yp;
      if (p1.yp < p2.yp)
      {
        bx = p1.xp;
        by = p1.yp;
        cx = p2.xp;
        cy = p2.yp;
      }
      else
      {
        bx = p2.xp;
        by = p2.yp;
        cx = p1.xp;
        cy = p1.yp;
      }
    }
    else
    {
      ax = p2.xp;
      ay = p2.yp;
      bx = xp;
      by = yp;
      cx = p1.xp;
      cy = p1.yp;
    }
  else
    if (yp < p2.yp)
    {
      ax = p1.xp;
      ay = p1.yp;
      bx = xp;
      by = yp;
      cx = p2.xp;
      cy = p2.yp;
    }
    else {
      cx = xp;
      cy = yp;
      if (p1.yp < p2.yp)
      {
        ax = p1.xp;
        ay = p1.yp;
        bx = p2.xp;
        by = p2.yp;
      }
      else
      {
        ax = p2.xp;
        ay = p2.yp;
        bx = p1.xp;
        by = p1.yp;
      }
    }
  return (AbsRat ((bx - ax) * (cy - ay) - (by - ay) * (cx - ax), cy - ay));
}


Pt2i *Pt2i::drawing (const Pt2i p, int *n) const
{
  int x1, y1, x2, y2;
  if (xp > p.xp)
  {
    x1 = p.xp;
    x2 = xp;
    y1 = p.yp;
    y2 = yp;
  }
  else
  {
    x1 = xp;
    x2 = p.xp;
    y1 = yp;
    y2 = p.yp;
  }
  int dx = x2 - x1;
  int dy = y2 - y1;
  int e, i = 0;
  Pt2i *pts;

  if (dy > 0)
  {
    // Octant 1
    if (dx >= dy)
    {
      *n = dx + 1;
      pts = new Pt2i[dx + 1];
      e = dx - 1; // car le point limite est trace en dessous de la droite
      dx *= 2;
      dy *= 2;
      while (x1 < x2)
      {
        pts[i++] = Pt2i (x1, y1);
        x1 ++;
        e -= dy;
        if (e < 0)
        {
          y1 ++;
          e += dx;
        }
      }
      pts[i] = Pt2i (x1, y1);
    }

    // Octant 2
    else
    {
      *n = dy + 1;
      pts = new Pt2i[dy + 1];
      e = dy; // car le point limite est trace a droite de la droite
      dx *= 2;
      dy *= 2;
      while (y1 < y2)
      {
        pts[i++] = Pt2i (x1, y1);
        y1 ++;
        e -= dx;
        if (e < 0)
        {
          x1 ++;
          e += dy;
        }
      }
      pts[i] = Pt2i (x1, y1);
    }
  }

  else
  {
    // Octant 8
    if (dx >= -dy)
    {
      *n = 1 + dx;
      pts = new Pt2i[dx + 1];
      e = dx - 1; // car le point limite est trace en dessous de la droite
      dx *= 2;
      dy *= 2;
      while (x1 < x2)
      {
        pts[i++] = Pt2i (x1, y1);
        x1 ++;
        e += dy;
        if (e < 0)
        {
          y1 --;
          e += dx;
        }
      }
      pts[i] = Pt2i (x1, y1);
    }

    // Octant 7
    else
    {
      *n = 1 - dy;
      pts = new Pt2i[1 - dy];
      e = - dy; // car le point limite est trace a gauche de la droite
      dx *= 2;
      dy *= 2;
      while (y1 > y2)
      {
        pts[i++] = Pt2i (x1, y1);
        y1 --;
        e -= dx;
        if (e < 0)
        {
          x1 ++;
          e -= dy;
        }
      }
      pts[i] = Pt2i (x1, y1);
    }
  }
  return (pts);
}


void Pt2i::draw (vector<Pt2i> &line, Pt2i p) const
{
  int x1, y1, x2, y2;
  if (xp > p.xp)
  {
    x1 = p.xp;
    x2 = xp;
    y1 = p.yp;
    y2 = yp;
  }
  else
  {
    x1 = xp;
    x2 = p.xp;
    y1 = yp;
    y2 = p.yp;
  }
  int dx = x2 - x1;
  int dy = y2 - y1;
  int e;

  if (dy > 0)
  {
    // Octant 1
    if (dx >= dy)
    {
      e = dx - 1; // car le point limite est trace en dessous de la droite
      dx *= 2;
      dy *= 2;
      while (x1 < x2)
      {
        line.push_back (Pt2i (x1, y1));
        x1 ++;
        e -= dy;
        if (e < 0)
        {
          y1 ++;
          e += dx;
        }
      }
      line.push_back (Pt2i (x1, y1));
    }

    // Octant 2
    else
    {
      e = dy; // car le point limite est trace a droite de la droite
      dx *= 2;
      dy *= 2;
      while (y1 < y2)
      {
        line.push_back (Pt2i (x1, y1));
        y1 ++;
        e -= dx;
        if (e < 0)
        {
          x1 ++;
          e += dy;
        }
      }
      line.push_back (Pt2i (x1, y1));
    }
  }

  else
  {
    // Octant 8
    if (dx >= -dy)
    {
      e = dx - 1; // car le point limite est trace en dessous de la droite
      dx *= 2;
      dy *= 2;
      while (x1 < x2)
      {
        line.push_back (Pt2i (x1, y1));
        x1 ++;
        e += dy;
        if (e < 0)
        {
          y1 --;
          e += dx;
        }
      }
      line.push_back (Pt2i (x1, y1));
    }

    // Octant 7
    else
    {
      e = - dy; // car le point limite est trace a gauche de la droite
      dx *= 2;
      dy *= 2;
      while (y1 > y2)
      {
        line.push_back (Pt2i (x1, y1));
        y1 --;
        e -= dx;
        if (e < 0)
        {
          x1 ++;
          e -= dy;
        }
      }
      line.push_back (Pt2i (x1, y1));
    }
  }
}


Pt2i *Pt2i::pathTo (Pt2i p, int *n) const
{
  int x1, y1, x2, y2, delta;
  if (xp > p.xp)
  {
    x1 = p.xp;
    x2 = xp;
    y1 = p.yp;
    y2 = yp;
    delta = -1;
  }
  else
  {
    x1 = xp;
    x2 = p.xp;
    y1 = yp;
    y2 = p.yp;
    delta = 1;
  }
  int dx = x2 - x1;
  int dy = y2 - y1;
  int e, i = 0;
  Pt2i *pts;

  if (dy > 0)
  {
    // Octant 1
    if (dx >= dy)
    {
      *n = dx;
      pts = new Pt2i[dx];
      e = dx - 1; // car le point limite est trace en dessous de la droite
      if (delta < 0) e++; // .. au dessus
      dx *= 2;
      dy *= 2;
      while (x1 < x2)
      {
        x1 ++;
        e -= dy;
        if (e < 0)
        {
          y1 ++;
          e += dx;
          pts[i++] = Pt2i (delta, delta);
        }
        else pts[i++] = Pt2i (delta, 0);
      }
    }

    // Octant 2
    else
    {
      *n = dy;
      pts = new Pt2i[dy];
      e = dy; // car le point limite est trace a droite de la droite
      if (delta < 0) e--; // .. a gauche
      dx *= 2;
      dy *= 2;
      while (y1 < y2)
      {
        y1 ++;
        e -= dx;
        if (e < 0)
        {
          x1 ++;
          e += dy;
          pts[i++] = Pt2i (delta, delta);
        }
        else pts[i++] = Pt2i (0, delta);
      }
    }
  }

  else
  {
    // Octant 8
    if (dx >= -dy)
    {
      *n = dx;
      pts = new Pt2i[dx];
      e = dx - 1; // car le point limite est trace en dessous de la droite
      if (delta < 0) e++; // .. au dessus
      dx *= 2;
      dy *= 2;
      while (x1 < x2)
      {
        x1 ++;
        e += dy;
        if (e < 0)
        {
          y1 --;
          e += dx;
          pts[i++] = Pt2i (delta, -delta);
        }
        else pts[i++] = Pt2i (delta, 0);
      }
    }
    // Octant 7
    else
    {
      *n = -dy;
      pts = new Pt2i[-dy];
      e = - dy; // car le point limite est trace a gauche de la droite
      if (delta < 0) e--; // .. a droite
      dx *= 2;
      dy *= 2;
      while (y1 > y2)
      {
        y1 --;
        e -= dx;
        if (e < 0)
        {
          x1 ++;
          e -= dy;
          pts[i++] = Pt2i (delta, -delta);
        }
        else pts[i++] = Pt2i (0, -delta);
      }
    }
  }
  return (pts);
}


bool *Pt2i::stepsTo (Pt2i p, int *n) const
{
  bool negx = p.xp < xp;
  bool negy = p.yp < yp;
  int x2 = (xp > p.xp) ? xp - p.xp : p.xp - xp;
  int y2 = (yp > p.yp) ? yp - p.yp : p.yp - yp;
  int dx = x2, dy = y2, e = 0;
  if (y2 > x2)
  {
    dx = y2;
    dy = x2;
    x2 = y2;
    if (negx == negy) e++;
  }
  else if (negx != negy) e++;
  e += dx - 1;
  dx *= 2;
  dy *= 2;

  int x = 0;
  *n = x2;
  bool *paliers = new bool[x2];
  while (x < x2)
  {
    e -= dy;
    if (e < 0)
    {
      e += dx;
      paliers[x++] = true;
    }
    else paliers[x++] = false;
  }
  return (paliers); 
}


vector<Pt2i> Pt2i::drawOrtho (const Pt2i p2, int offset) const
{
  vector<Pt2i> pts;

  int x1 = xp;
  int y1 = yp;
  int x2 = p2.x ();
  int y2 = p2.y ();
  int dx = x2 - x1;
  int dy = y2 - y1;
  int e;

  int num = dx * dy;
  if (num < 0) num = - num;
  int den = dx * dx + dy * dy;
  int nabs = (offset < 0 ? -offset : offset);
  int steps = (offset * num) / den;
  if ((nabs * num) % den >= den / 2) steps += (offset < 0 ? -1 : 1);
  int floors;

  if (dx > 0 && dy > 0)   // Quadrant 1
  {
    if (dx >= dy) // Octant 1
    {
      e = dx - 1;
      dx = dx * 2;
      dy = dy * 2;
      if (offset < 0) floors = (steps * dy - e) / dx;
      else floors = (e - 1 + steps * dy) / dx;
      e += floors * dx - steps * dy;
      x1 += steps;
      y1 -= offset - floors;
      x2 += steps;
      // y2 -= offset - floors;   // inutile
      while (x1 < x2)
      {
        pts.push_back (Pt2i (x1, y1));
        x1 ++;
        e = e - dy;
        if (e < 0)
        {
          y1 ++;
          e = e + dx;
        }
      }
      pts.push_back (Pt2i (x1, y1));
    }
    else if (dx < dy) // Octant 2
    {
      e = dy;
      dx = dx * 2;
      dy = dy * 2;
      if (offset < 0) floors = (1 - e + steps * dx) / dy;
      else floors = (e + steps * dx) / dy;
      e -= floors * dy - steps * dx;
      x1 += offset - floors;
      y1 -= steps;
      // x2 += offset - floors;  // inutile
      y2 -= steps;  // inutile
      while (y1 < y2)
      {
        pts.push_back (Pt2i (x1, y1));
        y1 ++;
        e -= dx;
        if (e < 0)
        {
          x1 ++;
          e += dy;
        }
      }
      pts.push_back (Pt2i (x1, y1));
    }
  }

  else if (dx > 0 && dy < 0) // Quadrant 4
  {
    if (dx >= -dy) // Octant 8
    {
      e = dx - 1;
      dx = dx * 2;
      dy = dy * 2;
      if (offset < 0) floors = (e - 1 + steps * dy) / dx;
      else floors = (steps * dy - e) / dx;   // < 0
      e += floors * dx - steps * dy;
      x1 -= steps;
      y1 -= offset + floors;
      x2 -= steps;
      // y2 -= offset + floors;  // inutile
      while (x1 < x2)
      {
        pts.push_back (Pt2i (x1, y1));
        x1 ++;
        e += dy;
        if (e < 0)
        {
          y1 --;
          e += dx;
        }
      }
      pts.push_back (Pt2i (x1, y1));
    }
    else // Octant 7
    {
      e = dy;
      dx = dx * 2;
      dy = dy * 2;
      if (offset < 0) floors = - (e + steps * dx) / dy;
      else floors = (e + 1 - steps * dx) / dy;
      e += floors * dy + steps * dx;
      x1 -= offset - floors;
      y1 -= steps;
      // x2 -= offset - floors;  // inutile
      y2 -= steps;
      while (y1 > y2)
      {
        pts.push_back (Pt2i (x1, y1));
        y1 --;
        e += dx;
        if (e > 0)
        {
          x1 ++;
          e += dy;
        }
      }
      pts.push_back (Pt2i (x1, y1));
    }
  }
  else if (dy == 0 && dx > 0)
    while (x1 <= x2) pts.push_back (Pt2i (x1++, y1 - offset));

  if (dx < 0 && dy > 0)    // Quadrant 2
  {
    if (-dx >= dy) // Octant 4
    {
      e = dx - 1;
      dx = dx * 2;
      dy = dy * 2;
      if (offset < 0) floors = - (steps * dy + e) / dx;
      else floors = (e + 1 - steps * dy) / dx;  // > 0
      e -= steps * dy + floors * dx;
      x1 += steps;
      y1 += offset - floors;
      x2 += steps;
      // y2 += offset - floors;   // inutile
      while (x1 > x2)
      {
        pts.push_back (Pt2i (x1, y1));
        x1 --;
        e += dy;
        if (e >= 0)
        {
          y1 ++;
          e += dx;
        }
      }
      pts.push_back (Pt2i (x1, y1));
    }
    else if (-dx < dy) // Octant 3
    {
      e = dy;
      dx = dx * 2;
      dy = dy * 2;
      if (offset < 0) floors = (1 - e - steps * dx) / dy;
      else floors = (e - steps * dx) / dy;
      e += floors * dy + steps * dx;
      x1 += offset - floors;
      y1 += steps;
      // x2 += offset - floors;   // inutile
      y2 += steps;
      while (y1 < y2)
      {
        pts.push_back (Pt2i (x1, y1));
        y1 ++;
        e += dx;
        if (e <= 0)
        {
          x1 --;
          e += dy;
        }
      }
      pts.push_back (Pt2i (x1, y1));
    }
  }

  else if (dx < 0 && dy < 0)    // Quadrant 3
  {
    if (dx <= dy) // Octant 5
    {
      e = dx - 1;
      dx = dx * 2;
      dy = dy * 2;
      if (offset < 0) floors = (steps * dy - e - 1) / dx;
      else floors = (steps * dy + e) / dx;
      e += floors * dx - steps * dy;
      x1 -= steps;
      y1 += offset - floors;
      x2 -= steps;
      // y2 += offset - floors;  // inutile
      while (x1 > x2)
      {
        pts.push_back (Pt2i (x1, y1));
        x1 --;
        e -= dy;
        if (e >= 0)
        {
          y1 --;
          e += dx;
        }
      }
      pts.push_back (Pt2i (x1, y1));
    }
    else // Octant 6
    {
      e = dy;
      dx = dx * 2;
      dy = dy * 2;
      if (offset < 0) floors = (e - steps * dx) / dy;  // > 0
      else floors = - (steps * dx + e + 1) / dy;
      e += floors * dy + steps * dx;
      x1 -= offset + floors;
      y1 += steps;
      y2 += steps;
     // x2 -= offset - floors;  // calcul inutile
      while (y1 > y2)
      {
        pts.push_back (Pt2i (x1, y1));
        y1 --;
        e -= dx;
        if (e >= 0)
        {
          x1 --;
          e += dy;
        }
      }
      pts.push_back (Pt2i (x1, y1));
    }
  }
  else if (dy == 0 && dx < 0)
    while (x1 >= x2) pts.push_back (Pt2i (x1--, y1 + offset));
  else if (dx == 0 && dy > 0)
    while (y1 <= y2) pts.push_back (Pt2i (x1 + offset, y1++));
  else if (dx == 0 && dy < 0)
    while(y1 >= y2) pts.push_back (Pt2i (x1 - offset, y1--));
  return pts;
}
