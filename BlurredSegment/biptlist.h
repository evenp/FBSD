#ifndef BIPT_LIST_H
#define BIPT_LIST_H

#include "pt2i.h"
#include <deque>

using namespace std;


/** 
 * @class BiPtList biptlist.h
 * \brief Bi-directional list of points.
 * \author {P. Even}
 */
class BiPtList
{
public:

  /**
   * Creates a extendable bi-directional list with one point.
   * @param pt Initial point of the list.
   */
  BiPtList (Pt2i pt);

  /**
   * Deletes the bi-directional list.
   */
  ~BiPtList ();

  /**
   * Returns the count of point in the bi-directional list.
   */
  inline int size () const { return (cpt); }

  /**
   * Returns the count of point on the back part of the bi-directional list.
   */
  inline int backSize () const { return (cpt - start - 1); }

  /**
   * Returns the count of point on the back part of the bi-directional list.
   */
  inline int frontSize () const { return (start); }

  /**
   * Returns the initial point of the bi-directional list.
   */
  inline Pt2i initialPoint () const { return (pts[start]); }

  /**
   * Returns the back end point of the bi-directional list.
   */
  inline Pt2i backPoint () const { return (pts.back ()); }

  /**
   * Returns the front end point of the bi-directional list.
   */
  inline Pt2i frontPoint () const { return (pts.front ()); }

  /**
   * Returns the height of the point to the line between the list end points..
   */
  inline AbsRat heightToEnds (const Pt2i &pt) const {
    return (pt.triangleRationalHeight (pts.front (), pts.back ())); }

  /**
   * Adds the point on front side.
   * @param pt The point to add.
   */
  void addFront (Pt2i pt);

  /**
   * Adds the point on back side.
   * @param pt The point to add.
   */
  void addBack (Pt2i pt);

  /**
   * Removes n point on front side.
   * @param n The count of points to remove.
   */
  void removeFront (int n);

  /**
   * Removes n point on back side.
   * @param n The count of points to remove.
   */
  void removeBack (int n);

  /**
   * Fills in the given parameters with the point min and max coordinates.
   * @param xmin Minimum X value.
   * @param ymin Minimum Y value.
   * @param xmax Maximum X value.
   * @param ymax Maximum Y value.
   */
  void findExtrema (int &xmin, int &ymin, int &xmax, int &ymax) const;

  /**
   * Returns front to back points in a vector.
   */
  vector<Pt2i> frontToBackPoints () const;

  /**
   * Returns a pointer to a empty vector.
   */
  vector<Pt2i> *vide () const;

  /**
   * Returns a pointer to a vector filled in with front points.
   * Front points are entered from segment edge to the initial point excluded.
   */
  vector<Pt2i> *frontPoints () const;

  /**
   * Returns a pointer to a vector filled in with back points.
   * Back points are entered from initial point excluded to segment edge.
   */
  vector<Pt2i> *backPoints () const;


private:

  /** List of points. */
  deque<Pt2i> pts;
  /** Index of the initial point. */
  int start;
  /** Length of the point list. */
  int cpt;
};
#endif
