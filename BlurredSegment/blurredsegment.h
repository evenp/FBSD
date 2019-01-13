#ifndef BLURRED_SEGMENT_H
#define BLURRED_SEGMENT_H


#include "convexhull.h"
#include "digitalstraightsegment.h"
#include "biptlist.h"
#include <cstdlib>

using namespace std;


/** 
 * @class BlurredSegment blurredsegment.h
 * \brief A list of 2D points lying inside a digital straight line.
 * \author {P. Even}
 */
class BlurredSegment
{
public:

  /**
   * Creates a blurred segment from nothing.
   */
  BlurredSegment ();

  /**
   * Creates a blurred segment from a list of points.
   * @param ptlist List of points of the blurred segment to build.
   * @param seg Bounding digital straight segment.
   * @param aps Start point of the antipodal edge.
   * @param ape End point of the antipodal edge.
   * @param apv Antipodal vertex.
   */
  BlurredSegment (BiPtList *ptlist, DigitalStraightSegment *seg,
                  const Pt2i &aps, const Pt2i &ape, const Pt2i &apv);

  /**
   * \brief Deletes the blurred segment.
   */
  virtual ~BlurredSegment ();

  /**
   * \brief Returns the minimal vertical or horizontal width.
   */
  virtual AbsRat minimalWidth () const;

  /**
   * \brief Returns if the segment has non null thickness (not aligned points).
   */
  inline bool isThick () const { return (dss->width () > 1); }

  /**
   * \brief Returns the underlying digital straight segment.
   */
  inline DigitalStraightSegment *getSegment () { return dss; }

  /**
   * \brief Returns the count of points of the blurred segment.
   */
  int size () const;

  /**
   * \brief Returns the start point of the blurred segment.
   */
  inline const Pt2i getCenter () const { return plist->initialPoint (); }

  /**
   * \brief Returns the colinear points at the left of the start point.
   */
  inline const vector<Pt2i> *getLeftLine () const { return plist->vide (); }

  /**
   * \brief Returns the colinear points at the right of the start point.
   */
  inline const vector<Pt2i> *getRightLine () const { return plist->vide (); }

  /**
   * \brief Returns the left points added to the blurred segment start point.
   */
  inline const vector<Pt2i> *getLeftPoints () const {
    return plist->frontPoints (); }

  /**
   * \brief Returns the right points added to the blurred segment start point.
   */
  inline const vector<Pt2i> *getRightPoints () const {
    return plist->backPoints (); }

  /**
   * \brief Returns the set of all the points on the blurred segment.
   * Points are ordered from the left end point up to the right end point.
   */
  vector<Pt2i> getAllPoints () const;

  /**
   * \brief Returns the set of points on the left part of the blurred segment.
   * Points are ordered from the furthest to the nearest to the start point.
   */
  vector<Pt2i> *getAllLeft () const;

  /**
   * \brief Returns the set of points on the left part of the blurred segment.
   * Points are ordered from the nearest to the furthest to the start point.
   */
  vector<Pt2i> *getAllRight () const;

  /**
   * \brief Returns a vector containing the start point of the blurred segment.
   */
  vector<Pt2i> getStartPt () const;

  /**
   * \brief Returns the last accepted point on the right side. 
   */
  const Pt2i getLastRight () const;

  /**
   * \brief Returns the last accepted point on the left side. 
   */
  const Pt2i getLastLeft () const;

  /**
   * \brief Returns the squarred length of the blurred segment.
   * Returns the squarred distance between end points.
   */
  int getSquarredLength () const;

  /**
   * \brief Returns the middle point of the blurred segment.
   * Returns the middle point between end points.
   */
  const Pt2i getMiddle () const;

  /**
   * \brief Returns the start point of the last antipodal edge. 
   */
  inline const Pt2i antipodalEdgeStart () const { return laps; }

  /**
   * \brief Returns the end point of the last antipodal edge. 
   */
  inline const Pt2i antipodalEdgeEnd () const { return lape; }

  /**
   * \brief Returns the last antipodal vertex. 
   */
  inline const Pt2i antipodalVertex () const { return lapv; }

  /**
   * \brief Returns the support vector of the blurred segment.
   */
  virtual Vr2i getSupportVector ();

  /**
   * \brief Returns the size of the segment bounding box in a vector 2D.
   */
  Vr2i boundingBoxSize () const;

  /**
   * \brief Returns the connected components of the blurred segment.
   */
  vector <vector <Pt2i> > connectedComponents () const;

  /**
   * \brief Returns the count of connected points in the blurred segment.
   */
  int countOfConnectedPoints () const;

  /**
   * \brief Returns the count of connected components in the blurred segment.
   */
  int countOfConnectedComponents () const;

  /**
   * \brief Returns the count of connected points of given minimal size.
   * @param min Minimal size of the connected components.
   */
  int countOfConnectedPoints (int min) const;

  /**
   * \brief Returns the count of connected components of given minimal size.
   * @param min Minimal size of the connected components.
   */
  int countOfConnectedComponents (int min) const;

  /**
   * \brief Returns the connected components of the blurred segment.
   */
  vector <vector <Pt2i> > getConnectedComponents () const;


protected:

  /** Bounding straight segment. */
  DigitalStraightSegment *dss;

  /** Bi-directional list of points. */
  BiPtList *plist;

  /** Start point of the last known antipodal edge. */
  Pt2i laps;
  /** End point of the last known antipodal edge. */
  Pt2i lape;
  /** Last known antipodal vertex. */
  Pt2i lapv;
};
#endif
