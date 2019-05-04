#ifndef ANTIPODAL_H
#define ANTIPODAL_H

#include "chvertex.h"
#include <cstdlib>

using namespace std;


/** 
 * @class Antipodal antipodal.h
 * \brief Antipodal pair of a convex hull.
 * \author {P. Even}
 */
class Antipodal
{
public:

  /** Builds an empty (undetermined) horizontal antipodal pair. */
  Antipodal ();

  /** Sets the coordinates for a vertical antipodal pair. */
  inline void setVertical () { ix = 1; iy = 0; }

  /** Initializes the vertex/edge pair from three unordered vertices. */
  void init (CHVertex *vpt, CHVertex *ept1, CHVertex *ept2);

  /** Sets the vertex and the edge of the antipodal pair. */
  inline void setVertexAndEdge (CHVertex *pt, CHVertex *es, CHVertex *ee) {
                             vpt = pt; ept1 = es; ept2 = ee; }

  /** Sets the vertex of the antipodal pair. */
  inline void setVertex (CHVertex *pt) { vpt = pt; }

  /** Sets the edge of the antipodal pair. */
  inline void setEdge (CHVertex *pt1, CHVertex *pt2) { ept1 = pt1; ept2 = pt2; }

  /** Returns the vertex of the antipodal pair. */
  inline CHVertex *vertex () const { return vpt; }

  /** Returns the leaning edge start of the antipodal pair. */
  inline CHVertex *edgeStart () const { return ept1; }

  /** Returns the leaning edge end of the antipodal pair. */
  inline CHVertex *edgeEnd () const { return ept2; }

  /**
   * Returns the antipodal pair horizontal thickness.
   * It is computed as the vertex horizontal distance to the edge.
   */
  AbsRat thickness () const;

  /**
   * Computes the antipodal pair horizontal thickness.
   * It is the vertex horizontal distance to the edge.
   * @param num Numerator of the thickness rational value.
   * @param den Denominator of the thickness rational value.
   */
  //void thickness (int &num, int &den) const;

  /** Returns the remainder of the edge line equation for given vertex. */
  int remainder (CHVertex *v) const;

  /** Checks if the edge lies in the first quadrant (x = y). */
  bool edgeInFirstQuadrant () const;

  /** Gets the edge vector Y coordinate. */
  int getA () const;

  /** Gets the edge vector X coordinate. */
  int getB () const;

  /** Updates the antipodal pair after the insertion of a new vertex. */
  void update (CHVertex *pt);

  /** Returns a string that represents the antipodal pair. */
  // friend ostream& operator<< (ostream &os, const Antipodal &ap);


protected:

  /** First coordinate (X for horizonal pair, Y for vertical pair). */
  int ix;
  /** Second coordinate (Y for horizonal pair, X for vertical pair). */
  int iy;

  /** Leaning vertex. */
  CHVertex *vpt;
  /** Start of the leaning edge. */
  CHVertex *ept1;
  /** End of the leaning edge. */
  CHVertex *ept2;

};
#endif
