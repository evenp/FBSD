#ifndef BLURRED_SEGMENT_PROTO_H
#define BLURRED_SEGMENT_PROTO_H


#include "blurredsegment.h"

using namespace std;


/** 
 * @class BlurredSegmentProto blurredsegmentproto.h
 * \brief A prototype of blurred segment, untill complete specification.
 * It is mostly based on a evolving list of points, its convex hull and
 * the successive states of the blurred segment construction.
 * \author {P. Even}
 */
class BlurredSegmentProto : public BlurredSegment
{
public:

  /**
   * Creates a blurred segment prototype.
   * @param maxWidth Maximal width of the blurred segment to build
   * @param pix Central point of the blurred segment to build
   */
  BlurredSegmentProto (int maxWidth, Pt2i pix);

  /**
   * Creates a blurred segment prototype with lists of points.
   * @param maxWidth Maximal width of the blurred segment to build.
   * @param center Central point of the blurred segment to build.
   * @param leftPts Points to add on left side
   * @param rightPts Points to add on right side.
   */
  BlurredSegmentProto (int maxWidth, Pt2i center,
                  const vector<Pt2i> &leftPts, const vector<Pt2i> &rightPts);

  /**
   * \brief Deletes the blurred segment prototype.
   */
  ~BlurredSegmentProto ();

  /**
   * \brief Returns the minimal vertical or horizontal width.
   */
  AbsRat minimalWidth () const;

  /**
   * \brief Returns the requested max width of the segment.
   */
  inline const AbsRat getMaxWidth () const { return maxWidth; }

  /**
   * \brief Sets the requested max width of the segment.
   * @param maxwidth The new vaue for the requested max width.
   */
  inline void setMaxWidth (const AbsRat &val) { maxWidth.set (val); }

  /**
   * \brief Returns the underlying digital straight line.
   */
  DigitalStraightLine *getLine () const;

  /**
   * Adds a new point on the left.
   * @param pix point to be added.
   * Returns true if the point is accepted.
   */
  bool addLeft (Pt2i pix);

  /**
   * Adds a new point on the right.
   * @param pix point to be added.
   * Returns true if the point is accepted.
   */
  bool addRight (Pt2i pix);

  /**
   * \brief Remove last points on the left side.
   * @param n Number of points to remove.
   */
  void removeLeft (int n);

  /**
   * \brief Remove last points on the right side.
   * @param n Number of points to remove.
   */
  void removeRight (int n);

  /**
   * \brief Returns the support vector of the blurred segment.
   */
  Vr2i getSupportVector ();

  /**
   * \brief Returns a stable blurred segment from available information.
   */
  BlurredSegment *endOfBirth ();


protected:

  /** Maximal width of the blurred segment. */
  AbsRat maxWidth;

  /** Maintained convex hull of the blurred segment. */
  ConvexHull *convexhull;

  /** Indicates if the blurred segment is constructed. */
  bool bsOK;
  /** Indicates if the points are aligned. */
  bool bsFlat;
  /** Indicates if the left point is defined. */
  bool leftOK;
  /** Indicates if the right point is defined. */
  bool rightOK;

  /** Flag indicating if the convex hull changed since last DSS extraction. */
  bool chChanged;


  /**
   * \brief Submits a new point to extend the blurred segment.
   * @param p Submitted point.
   * @param onleft Adding direction (true for LEFT, false for RIGHT).
   */
  bool addPoint (Pt2i p, bool onleft);
};
#endif
