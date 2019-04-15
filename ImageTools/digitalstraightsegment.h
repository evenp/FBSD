#ifndef DIGITAL_STRAIGHT_SEGMENT_H
#define DIGITAL_STRAIGHT_SEGMENT_H


#include "digitalstraightline.h"

using namespace std;


/** 
 * @class DigitalStraightSegment digitalstraightsegment.h
 * \brief Digital straight segment is a bounded digital straight line.
 * The digital straightline is defined by equation : c <= ax + by < c + nu
 *   where a >= 0 and nu > 0. Seemingly b is unconstrained when a = 0.
 * The bounds are horizontal lines (min <= y <= max) if the segment is vertical,
 *   vertical lines (min <= x <= max) if the segment is horizontal.
 * The bound points belong to the digital straight segment.
 * \author {P. Even}
 */
class DigitalStraightSegment : public DigitalStraightLine
{
public:

  /**
   * Creates a null-thick segment joining two points.
   * @param p1 First point on the line.
   * @param p2 Second point on the line.
   * @param type Digital line type : DSL_THIN, DSL_NAIVE or DSL_STANDARD.
   * @param xmin Segment min X coordinate.
   * @param ymin Segment min Y coordinate.
   * @param xmax Segment max X coordinate.
   * @param ymax Segment max Y coordinate.
   */
  DigitalStraightSegment (Pt2i p1, Pt2i p2, int type,
                          int xmin, int ymin, int xmax, int ymax);

  /**
   * Creates a digital straight segment from an antipodal pair.
   * @param p1 First segment end of the antipodal pair.
   * @param p1 Second segment end of the antipodal pair.
   * @param p3 Opposite point of the antipodal pair.
   * @param xmin Segment min X coordinate.
   * @param ymin Segment min Y coordinate.
   * @param xmax Segment max X coordinate.
   * @param ymax Segment max Y coordinate.
   */
  DigitalStraightSegment (Pt2i p1, Pt2i p2, Pt2i p3,
                          int xmin, int ymin, int xmax, int ymax);

  /**
   * Creates a digital straight segment from its end points and a width value.
   * @param p1 First end point of the segment.
   * @param p2 Second end point of the segment.
   * @param width Width value.
   */
  DigitalStraightSegment (Pt2i p1, Pt2i p2, int width);

  /**
   * \brief Returns a bounding point of the digital line
   * @param upper true for a upper bounding point, false for a lower one.
   */
  Pt2i getABoundingPoint (bool upper) const;

  /**
   * \brief Returns the points of the segment.
   * @param pts Vector of points to fill in.
   */
  void getPoints (vector<Pt2i> &pts) const;

  /**
   * \brief Returns an erosion of the segment.
   * @param num Erosion value numerator.
   * @param den Erosion value denominator.
   */
  DigitalStraightSegment *erosion (int num, int den) const;

  /**
   * \brief Returns a dilation of the segment.
   * @param num Dilation value numerator.
   * @param den Dilation value denominator.
   */
  DigitalStraightSegment *dilation (int num, int den) const;

  /**
   * \brief Returns a dilated segment of the given radius.
   * @param radius Dilation radius.
   */
  DigitalStraightSegment *dilation (int radius) const;

  /**
   * \brief Dilates the segment of the given radius.
   * @param radius Dilation radius.
   */
  void dilate (int radius);


protected:

  /**
   * \brief Bounding line lower coordinate.
   * If the segment is stictly horizontal, it enforces x >= xmin.
   * If the segment is vertical or diagonal, it enforces y >= ymin.
   * The bound belongs to the segment.
   */
  int min;
  /**
   * \brief Bounding line upper coordinate.
   * If the segment is stictly horizontal, it enforces x <= xmax.
   * If the segment is vertical or diagonal, it enforces y <= ymax.
   * The bound belongs to the segment.
   */
  int max;


  /**
   * Creates a segment from its characteristics.
   * @param va Slope X coordinate.
   * @param vb Slope Y coordinate.
   * @param vc Shift to origin.
   * @param vnu Arithmetical width.
   * @param vmin Bounding line lower coordinate.
   * @param vmax Bounding line upper coordinate.
   */
  DigitalStraightSegment (int va, int vb, int vc, int vnu, int vmin, int vmax);

  /**
   * \brief Adjusts the provided area on the segment limits.
   * @param xmin Left coordinate of the area.
   * @param ymin Bottom coordinate of the area.
   * @param width Width of the area.
   * @param height Height of the area.
   */
  void adjustWorkArea (int &xmin, int &ymin, int &width, int &height) const;
};
#endif
