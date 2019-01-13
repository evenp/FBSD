#ifndef PT2I_H
#define PT2I_H

#include "vr2i.h"
#include "absrat.h"
#include <vector>

using namespace std;



/** 
 * @class Pt2i pt2i.h
 * \brief Point in the digital plane.
 * \author {P. Even}
 */
class Pt2i
{
public:

  /**
   * @fn Pt2i ()
   * \brief Creates a point at origin.
   */
  Pt2i ();

  /**
   * @fn Pt2i (int x, int y)
   * \brief Creates a point using coordinates.
   * @param x X coordinate.
   * @param y Y coordinate.
   */
  Pt2i (int x, int y);

  /**
   * @fn Pt2i (Pt2i p)
   * \brief Creates a copy of a given point.
   * @param p Original.
   */
  Pt2i (const Pt2i &p);

  /**
   * @fn ~Pt2i ()
   * \brief Deletes the point.
   */
  ~Pt2i () { }

  /**
   * @fn int x ()
   * \brief Returns the point abscissae.
   */
  inline int x () const
  {
    return xp;
  }

  /**
   * @fn int y ()
   * \brief Returns the point ordinate.
   */
  inline int y () const
  {
    return yp;
  }

  /**
   * @fn int get (int n)
   * \brief Returns the nth coordinate.
   * @param n Coordinate index.
   */
  inline int get (int n) const
  {
    return (n ? yp : xp);
  }

  /**
   * @fn void set (int x, int y)
   * \brief Sets the point coordinates.
   * @param x New abscissae.
   * @param y New ordinate.
   */
  inline void set (int x, int y)
  {
    xp = x;
    yp = y;
  }

  /**
   * @fn void set (const Pt2i &p y)
   * \brief Sets the point coordinates.
   * @param point to recopy.
   */
  inline void set (const Pt2i &p)
  {
    xp = p.xp;
    yp = p.yp;
  }

  /**
   * @fn bool equals (Pt2i p)
   * \brief Checks equivalence to the given point.
   * @param p the given point.
   */
  inline bool equals (Pt2i p) const
  {
    return (p.xp == xp && p.yp == yp);
  }

  /**
   * @fn int manhattan (Pt2i p)
   * \brief Returns the manhattan distance to the given point.
   * @param p the given point.
   */
  inline int manhattan (Pt2i p) const
  {
    return (((p.xp > xp) ? p.xp - xp : xp - p.xp)
            + ((p.yp > yp) ? p.yp - yp : yp - p.yp));
  }

  /**
   * @fn int chessboard (Pt2i p)
   * \brief Returns the chessboard distance to the given point.
   * @param p the given point.
   */
  inline int chessboard (Pt2i p) const
  {
    int dx = (p.xp > xp ? p.xp - xp : xp - p.xp);
    int dy = (p.yp > yp ? p.yp - yp : yp - p.yp);
    return (dx > dy ? dx : dy);
  }

  /**
   * @fn int isConnectedTo (Pt2i p)
   * \brief Checks whether the point is connected (chessboard) to the given one.
   * @param p the given point.
   */
  inline bool isConnectedTo (Pt2i p) const
  {
    return ((p.xp - xp <= 1) && (xp - p.xp <= 1)
            && (p.yp - yp <= 1) && (yp - p.yp <= 1));
  }

  /**
   * @fn bool colinearTo (const Point2D &p1, const Point2D &p2)
   * Checks whether the point is colinear to tho other point.
   * @param p1 First distinct point.
   * @param p2 Second distinct point.
   */
  inline bool colinearTo (const Pt2i &p1, const Pt2i &p2) const
  {
    return ((p1.xp - xp) * (p2.yp - yp) == (p2.xp - xp) * (p1.yp - yp));
  }

  /**
   * @fn bool toLeft (const Point2D &p1, const Point2D &p2)
   * Checks whether the point is to the left of the segment (p1p2).
   * @param p1 Segment start point.
   * @param p2 Segment end point.
   */
  inline bool toLeft (const Pt2i &p1, const Pt2i &p2) const
  {
    return ((p1.xp - xp) * (p2.yp - yp) > (p2.xp - xp) * (p1.yp - yp));
  }

  /**
   * @fn bool toLeftOrOn (const Point2D &p1, const Point2D &p2)
   * Checks whether the point is to the left or on the segment (p1p2).
   * @param p1 Segment start point.
   * @param p2 Segment end point.
   */
  inline bool toLeftOrOn (const Pt2i &p1, const Pt2i &p2) const
  {
    return ((p1.xp - xp) * (p2.yp - yp) >= (p2.xp - xp) * (p1.yp - yp));
  }

  /**
   * @fn AbsRat triangleRationalHeight (const Pt2i &p1, const Pt2i &p2)
   * \brief Returns the X/Y height of the triangle with two other points.
   * @param p1 First distinct point.
   * @param p2 Second distinct point.
   */
  AbsRat triangleRationalHeight (const Pt2i &p1, const Pt2i &p2) const;

  /**
   * @fn AbsRat triangleXRationalHeight (const Pt2i &p1, const Pt2i &p2)
   * \brief Returns the vertical height of the triangle with two other points.
   * @param p1 First distinct point.
   * @param p2 Second distinct point.
   */
  AbsRat triangleXRationalHeight (const Pt2i &p1, const Pt2i &p2) const;

  /**
   * @fn AbsRat triangleYRationalHeight (const Pt2i &p1, const Pt2i &p2)
   * \brief Returns the horizontal height of the triangle with two other points.
   * @param p1 First distinct point.
   * @param p2 Second distinct point.
   */
  AbsRat triangleYRationalHeight (const Pt2i &p1, const Pt2i &p2) const;

  /**
   * @fn Vr2i vectorTo (Pt2i p)
   * \brief Returns the discrete vector to the given point.
   * @param p the given point.
   */
  Vr2i vectorTo (Pt2i p) const;

  /**
   * @fn Pt2i *drawing (Pt2i p, int *n)
   * \brief Returns the segment to the given point.
   * @param p the given point.
   * @param n size of the returned array.
   */
  Pt2i *drawing (const Pt2i p, int *n) const;

  /**
   * @fn Pt2i *drawing (Pt2i p, int *n)
   * \brief Fills the provided vector with the segment to the given point.
   * @param line Vector of points to fill in.
   * @param p Distant point.
   */
  void draw (vector<Pt2i> &line, Pt2i p) const;

  /**
   * @fn Pt2i *pathTo (Pt2i p, int *n)
   * \brief Returns the succession of local moves to reach the given point.
   * A local move is the vector from a point to one of his neighbours.
   * @param p the given point.
   * @param n size of the returned array.
   */
  Pt2i *pathTo (Pt2i p, int *n) const;

  /**
   * @fn bool *stepsTo (Pt2i p, int *n)
   * \brief Returns the location of the steps to reach the given point.
   * @param p the given point.
   * @param n size of the returned array.
   */
  bool *stepsTo (Pt2i p, int *n) const;

  /**
   * @fn vector<Pt2i> drawOrtho (const Pt2i p2, int offset) const
   * \brief Returns the segment orthogonal to the segment to p2.
   * @param p2 The distant point.
   * @param offset The orthogonal offset.
   */
  vector<Pt2i> drawOrtho (const Pt2i p2, int offset) const;


protected:

  /** Point abscissae. */
  int xp;
  /** Point ordinate. */
  int yp;
};

#endif
