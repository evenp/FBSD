#ifndef CONVEXHULL
#define CONVEXHULL


#include <vector> 
#include "antipodal.h"

using namespace std;


/** 
 * @class ConvexHull convexhull.h
 * \brief Convex hull of a polyline.
 * \author {P. Even}
 */
class ConvexHull
{
public:

  /**
   * \brief Creates a convex hull from a triangle.
   * Be very careful with the points ordering : lpt, cpt, then rpt.
   * Ensure that the points are NOT COLINEAR (not tested here).
   * @param lpt : left end of the polyline.
   * @param cpt : center of the polyline.
   * @param rpt : right end of the polyline.
   */
  ConvexHull (const Pt2i &lpt, const Pt2i &cpt, const Pt2i &rpt);

  /**
   * \brief Deletes the convex hull.
   * Removes all the registred vertices.
   */
  ~ConvexHull ();

  /**
   * \brief Restores the convexhull features after a modification.
   */
  void restore ();

  
  /**
   * \brief Appends a new point at one side of the convex hull.
   */
  bool addPoint (const Pt2i &pix, bool toleft);

  /**
   * \brief Appends a new point at one side of the convex hull.
   * To be used with directional scans :
   *   In that case, added point can not be inside the hull.
   */
  bool addPointDS (const Pt2i &pix, bool toleft);

  /**
   * \brief Moves the last entered point and returns the success.
   */
  bool moveLastPoint (const Pt2i &pix);

  /**
   * \brief Checks whether the line to the given point crosses the hull.
   */
  bool inHull (const Pt2i &pix, bool toleft) const;


  /**
   * Returns the antipodal vertex.
   * @param s Antipodal edge start.
   * @param e Antipodal edge end.
   * @param v Antipodal vertex.
   */
  void antipodalEdgeAndVertex (Pt2i &s, Pt2i &e, Pt2i &v) const;

  /**
   * Returns the minimal vertical or horizontal thickness of the convex hull.
   */
  AbsRat rationalThickness () const;

  /**
   * Returns a string that represents the convex hull.
   */
  // friend ostream& operator<< (ostream &os, const ConvexHull &ch);

  /**
   * Returns the first vertex of the convex hull.
   */
  inline CHVertex *getFirstVertex () const { return (leftVertex); }

  /**
   * Returns the last vertex of the convex hull.
   */
  inline CHVertex *getLastVertex () const { return (rightVertex); }

  /**
   * Returns the horizontal antipodal vertex.
   */
  inline CHVertex *getAphVertex () const { return (aph.vertex ()); }

  /**
   * Returns the horizontal antipodal edge start.
   */
  inline CHVertex *getAphEdgeStart () const { return (aph.edgeStart ()); }

  /**
   * Returns the horizontal antipodal edge end.
   */
  inline CHVertex *getAphEdgeEnd () const { return (aph.edgeEnd ()); }

  /**
   * Returns the vertical antipodal vertex.
   */
  inline CHVertex *getApvVertex () const { return (apv.vertex ()); }

  /**
   * Returns the vertical antipodal edge start.
   */
  inline CHVertex *getApvEdgeStart () const { return (apv.edgeStart ()); }

  /**
   * Returns the vertical antipodal edge end.
   */
  inline CHVertex *getApvEdgeEnd () const { return (apv.edgeEnd ()); }


protected:

  /** Polyline left end point. */
  CHVertex *leftVertex;
  /** Polyline right end point. */
  CHVertex *rightVertex;
  /** Indicates if the last vertex was entered to the left. */
  bool lastToLeft;

  /** Antipodal pair in horizontal direction. */
  Antipodal aph;
  /** Antipodal pair in vertical direction. */
  Antipodal apv;

  /** Registration of previous horizontal antipodal pair. */
  CHVertex *old_aph_vertex, *old_aph_edge_start, *old_aph_edge_end;;
  /** Registration of previous vertical antipodal pair. */
  CHVertex *old_apv_vertex, *old_apv_edge_start, *old_apv_edge_end;
  /** Registration of previous polyline left end point. */
  CHVertex *old_left;
  /** Registration of previous polyline right end point. */
  CHVertex *old_right;
  /** Registration of previous connections. */
  CHVertex *lconnect, *ldisconnect, *rconnect, *rdisconnect;

  /** Collection of vertices for clearance. */
  vector<CHVertex*> gbg;


private:

  /**
   * Stores the convexhull features before a modification.
   */
  void preserve ();

  /**
   * Inserts a new point into the convex hull.
   */
  void insert (CHVertex *pt, bool toleft);

  /**
   * Inserts a new point into the convex hull.
   * To be used with directional scans :
   *   In that case, opposite ends of the polyline can never pass each other.
   */
  void insertDS (CHVertex *pt, bool toleft);
};
#endif
