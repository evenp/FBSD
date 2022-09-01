#ifndef DIGITAL_STRAIGHT_LINE_H
#define DIGITAL_STRAIGHT_LINE_H


#include <vector>
#include "pt2i.h"

using namespace std;


/** 
 * @class DigitalStraightLine digitalstraightline.h
 * \brief Digital straight line.
 * The digital straightline is defined by equation : c <= ax + by < c + nu
 *   where a >= 0, b > 0 if a = 0, and nu > 0.
 * \author {P. Even}
 */
class DigitalStraightLine
{
public:

  /** Digital line type : Thin line (width = 1). */
  static const int DSL_THIN;
  /** Digital line type : Naive line (width = max (|a|,|b|)). */
  static const int DSL_NAIVE;
  /** Digital line type : Standard line (width = |a|+|b|). */
  static const int DSL_STANDARD;

  /**
   * Creates a digital straightline from its equation parameters.
   * @param a X value slope parameter of equation : c <= ax + by < c + nu
   * @param b Y value slope parameter of equation : c <= ax + by < c + nu
   * @param c Intercept parameter of equation : c <= ax + by < c + nu
   * @param nu Width parameter of equation : c <= ax + by < c + nu
   */
  DigitalStraightLine (int a, int b, int c, int nu);

  /**
   * Creates a digital straight line bounded by the line joining two points.
   * @param p1 First point on the bounding line.
   * @param p2 Second point on the bounding line.
   * @param type Digital line type : DSL_THIN, DSL_NAIVE or DSL_STANDARD.
   */
  DigitalStraightLine (Pt2i p1, Pt2i p2, int type);

  /**
   * Creates a digital straight line from an antipodal pair.
   * @param p1 First segment end of the antipodal pair.
   * @param p1 Second segment end of the antipodal pair.
   * @param p3 Opposite point of the antipodal pair.
   */
  DigitalStraightLine (Pt2i p1, Pt2i p2, Pt2i p3);

  /**
   * Creates a digital straight line parallel to the line joining two points.
   * Unknown use, care the side correctness in case of.
   * @param p1 First point.
   * @param p2 Second point.
   * @param type Digital line type : DSL_THIN, DSL_NAIVE or DSL_STANDARD
   * @param atRight Rightwards parallel offset to p1p2 line.
   */
  DigitalStraightLine (Pt2i p1, Pt2i p2, int type, int atRight);

  /**
   * Creates a similar digital straight line to an other one.
   * @param l The original line.
   */
  DigitalStraightLine (const DigitalStraightLine &l);

  /**
   * Deletes the digital straight line.
   */
  virtual ~DigitalStraightLine () { }

  /**
   * \brief Returns the parameters of the digital straight line equations.
   * @param a X slope parameter to provide.
   * @param b Y slope parameter to provide.
   * @param c intercept parameter to provide.
   * @param nu width parameter to provide.
   */
  inline void equation (int &a, int &b, int &c, int &nu) const
  {
    a = this->a;
    b = this->b;
    c = this->c;
    nu = this->nu;
  }

  /**
   * \brief Fills in the given array with the three medial axis parameters.
   * @param a X slope parameter to provide.
   * @param b Y slope parameter to provide.
   * @param c0 intercept parameter of the median axis.
   */
  inline void getMedialAxis (int &a, int &b, int &c0) const
  {
    a = this->a;
    b = this->b;
    c0 = c + nu / 2;
  }

  /**
   * \brief Returns the period of the digital straight line : max (|a|,|b|).
   */
  inline int period () const
  {
    int absb = b < 0 ? -b : b;
    return (a < absb ? absb : a);
  }

  /**
   * \brief Returns the standard width : |a| + |b|.
   */
  inline int standard () const
  {
    return (a + (b < 0 ? -b : b));
  }

  /**
   * \brief Returns the lower of the digital straight line : min (|a|,|b|).
   */
  inline int antiperiod () const
  {
    int absb = b < 0 ? -b : b;
    return (a < absb ? a : absb);
  }

  /**
   * \brief Returns the manhattan width of the digital straight line.
   */
  inline int width () const
  {
    return (nu);
  }

  /**
   * \brief Returns a support vector of the digital straight line.
   */
  inline Vr2i supportVector () const
  {
    return (Vr2i (-b, a));
  }

  /**
   * \brief Returns the signed manhattan distance to the given point.
   * Right values are provided for points on the right side (yp > line (yp))
   */
  int manhattan (Pt2i pix) const;

  /**
   * \brief Returns a bounding point of the digital straight line.
   * @param upper true for a upper bounding point, false for a lower one.
   */
  virtual Pt2i getABoundingPoint (bool upper) const;

  /**
   * \brief Fills in the vector with the requested bounding line points.
   *   The bounding line is naive if (nu < period).
   *   Restricts the points in the specified area.
   * @param bound The vector of points to fill in.
   * @param opposite true to get the opposite bounding line (ax+by=c+nu-1).
   * @param xmin X coordinate of the viewport.
   * @param ymin Y coordinate of the viewport.
   * @param width Width of the viewport.
   * @param height Height of the viewport.
   */
  void getBoundPoints (vector<Pt2i> &bound, bool opposite,
                       int xmin, int ymin, int width, int height) const;

  /**
   * \brief Fills in the provided vector with the digital line bounding points.
   * @param bound The vector of points to fill in.
   * @param xmin X coordinate of the viewport.
   * @param ymin Y coordinate of the viewport.
   * @param width Width of the viewport.
   * @param height Height of the viewport.
   */
  void getBounds (vector<Pt2i> &bound,
                  int xmin, int ymin, int width, int height) const;

  /**
   * \brief Checks if given point belongs to the digital line.
   * @param p Given point.
   */
  bool owns (const Pt2i &p) const;

  /**
   * \brief Checks if given segment entirely belongs to the digital line.
   * @param p1 Given segment start point.
   * @param p2 Given segment end point.
   */
  bool owns (const Pt2i &p1, const Pt2i &p2) const;

  /**
   * \brief Checks if given segment belongs even partially to the digital line.
   * @param p1 Given segment start point.
   * @param p2 Given segment end point.
   */
  bool crosses (const Pt2i &p1, const Pt2i &p2) const;

  /**
   * \brief Returns the center of the intersection with an other digital line.
   * @param l The other digital line.
   */
  const Pt2i centerOfIntersection (DigitalStraightLine *l) const;

  /**
   * \brief Returns the center of the intersection with line P1-P2.
   *   Care : returns (0,0) if p1 == p2 or if P1P2 is parallel to the line.
   * @param p1 = start point of the crossed line.
   * @param p2 = end point of the crossed line.
   */
  const Pt2i centerOfIntersection (Pt2i p1, Pt2i p2) const;

  /**
   * \brief Returns the squared Euclidean thickness of the digital line.
   */
  const AbsRat squaredEuclideanThickness () const {
    return (AbsRat (nu * nu, a * a + b * b)); }

  /**
   * \brief Returns the side shift of a point wrt the line.
   *    The side shift is obtained as the index of the line containing the
   * point when paving the Euclidean plane by the digital straight line.
   * @param pt Input point.
   */
  int sideShift (Pt2i pt) const;

  /**
   * \brief Returns the side shift of a point wrt the line.
   *    The side shift is obtained as the index of the line containing the
   * point when paving the Euclidean plane by the digital straight line.
   * Computation relies on long integers to prevent integer capacity overflow.
   * @param pt Input point.
   */
  int sideShiftSafe (Pt2i pt) const;

  /**
   * \brief Returns the "signed" distance of a point to the line.
   * @param pt Input point.
   */
  int rmd (Pt2i pt) const;


protected:

  /**
   * \brief X value of the slope parameter in equation : c <= ax + by < c + nu.
   *  a is greater or equal to 0.
   */
  int a;

  /**
   * \brief Y value of the slope parameter in equation : c <= ax + by < c + nu.
   * b is a positive value when the line is horizontal (a = 0).
   */
  int b;

  /**
   * \brief Intercept parameter in equation : c <= ax + by < c + nu.
   */
  int c;

  /**
   * \brief Width parameter in equation : c <= ax + by < c + nu.
   * nu is a positive value.
   */
  int nu;


  /**
   * \brief Adjusts the provided area on the straight line limits.
   * @param xmin Left coordinate of the area.
   * @param ymin Bottom coordinate of the area.
   * @param width Width of the area.
   * @param height Height of the area.
   */
  virtual void adjustWorkArea (int &xmin, int &ymin,
                               int &width, int &height) const;

  /**
   * \brief Returns the greater common divisor between two integer values.
   * @param a First integer.
   * @param b Second integer.
   */
  static int pgcd (int a, int b);
};
#endif
