#ifndef CHVERTEX_H
#define CHVERTEX_H

#include "pt2i.h"
#include <cstdlib>


/** 
 * @class CHVertex chvertex.h
 * \brief Chained point with two adjacent points.
 * \author {P. Even}
 */
class CHVertex : public Pt2i
{
public:

  /** Builds a default vertex. */
  CHVertex ();

  /** Builds a vertex on given coordinates. */
  CHVertex (int x, int y);

  /** Builds a vertex at the position of the given point. */
  CHVertex (const Pt2i &p);
  ~CHVertex ();

  inline CHVertex *left () const { return lv; }
  inline CHVertex *right () const { return rv; }
  inline void setLeft (CHVertex *v) { lv = v; }
  inline void setRight (CHVertex *v) { rv = v; }

  /** Returns the vector product of vector (this, pt) and vector (vx, vy)
   */
  inline int vprod (CHVertex *pt, int vx, int vy) const {
                   return ((pt->xp - xp) * vy - vx * (pt->yp - yp)); }

  /** Returns the vector product of vector (this, p2) and vector (p3, p4)
   */
  inline int vprod (CHVertex *p2, CHVertex *p3, CHVertex *p4) const {
                   return ((p2->xp - xp) * (p4->yp - p3->yp)
                           - (p4->xp - p3->xp) * (p2->yp - yp)); }

  // friend ostream& operator<< (ostream &os, const CHVertex &v);


protected:

  /** Adjacent vertex to the left. */
  CHVertex *lv;
  /** Adjacent vertex to the right. */
  CHVertex *rv;
};
    
#endif
