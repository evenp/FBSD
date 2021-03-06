#ifndef DIRECTIONAL_SCANNER_O2_H
#define DIRECTIONAL_SCANNER_O2_H

#include "directionalscanner.h"

using namespace std;



/** 
 * @class DirectionalScannerO2 directionalscannero2.h
 * \brief Incremental directional scanner for the 2nd octant.
 * \author {P. Even and B. Kerautret}
 */
class DirectionalScannerO2 : public DirectionalScanner
{
public:
  
  /**
   * @fn DirectionalScanner02(int xmin, int ymin, int xmax, int ymax,
   *                          int a, int b, int mu2,
   *                          int nbs, bool *steps, int sx, int sy)
   * \brief Creates a directional scanner from pattern, start and upper bound.
   * The scan strip is composed of parallel scan lines, the first one being
   *   defined by a start point, a line pattern, and an upper bound.
   * @param xmin left border of the scan area
   * @param ymin low border of the scan area
   * @param xmax right border of the scan area
   * @param ymax up border of the scan area
   * @param a x coefficient of the discrete support line
   * @param b y coefficient of the discrete support line
   * @param c y intercept parameter of the upper bound
   * @param nbs size of the support line pattern
   * @param steps support line pattern
   * @param sx abscissae of the central scan start point
   * @param sy ordinate of the central scan start point
   */
  DirectionalScannerO2 (int xmin, int ymin, int xmax, int ymax,
                        int a, int b, int c,
                        int nbs, bool *steps, int sx, int sy);

  /**
   * @fn DirectionalScanner02(int xmin, int ymin, int xmax, int ymax,
   *                          int a, int b, int c1, int c2,
   *                          int nbs, bool *steps, int cx, int cy)
   * \brief Creates a directional scanner from pattern, center and bounds.
   * The scan strip is composed of parallel scan lines, the first one being
   *   defined by a center, a line pattern, upper and lower bounds.
   * @param xmin left border of the scan area
   * @param ymin low border of the scan area
   * @param xmax right border of the scan area
   * @param ymax up border of the scan area
   * @param a x coefficient of the discrete support line
   * @param b y coefficient of the discrete support line
   * @param c1 y intercept parameter of one of the bounds
   * @param c2 y intercept parameter of the other bound
   * @param nbs size of the support line pattern
   * @param steps support line pattern
   * @param cx abscissae of the central scan center
   * @param cy ordinate of the central scan center
   */
  DirectionalScannerO2 (int xmin, int ymin, int xmax, int ymax,
                        int a, int b, int c1, int c2,
                        int nbs, bool *steps, int cx, int cy);

  /**
   * @fn DirectionalScanner02(int xmin, int ymin, int xmax, int ymax,
   *                          int a, int b,
   *                          int nbs, bool *steps, int cx, int cy, int length)
   * \brief Creates a directional scanner from pattern, center and length.
   * The scan strip is composed of parallel scan lines, the first one being
   *   defined by a center, a line pattern, and a length value.
   * @param xmin left border of the scan area
   * @param ymin low border of the scan area
   * @param xmax right border of the scan area
   * @param ymax up border of the scan area
   * @param a x coefficient of the discrete support line
   * @param b y coefficient of the discrete support line
   * @param nbs size of the support line pattern
   * @param steps support line pattern
   * @param cx abscissae of the central scan center
   * @param cy ordinate of the central scan center
   * @param length length of a scan strip
   */
  DirectionalScannerO2 (int xmin, int ymin, int xmax, int ymax,
                        int a, int b,
                        int nbs, bool *steps,
                        int cx, int cy, int length);

  /**
   * @fn first(vector<Pt2i> &scan)
   * \brief Returns the central scan.
   * Fills in the vector with the central scan and returns its size.
   * @param scan vectors of points to be filled in.
   */
  int first (vector<Pt2i> &scan);

  /**
   * @fn nextOnLeft(vector<Pt2i> &scan)
   * \brief Returns the next scan on the left.
   * Fills in the vector with the next scan on left and returns its size.
   * @param scan vectors of points to be filled in.
   */
  int nextOnLeft (vector<Pt2i> &scan);

  /**
   * @fn nextOnRight(vector<Pt2i> &scan)
   * \brief Returns the next scan on the right.
   * Fills in the vector with the next scan on right and returns its size.
   * @param scan vectors of points to be filled in.
   */
  int nextOnRight (vector<Pt2i> &scan);

  /**
   * @fn Pt2i locate (const Pt2i &pt) const
   * \brief Returns the scanner coordinates of the givent point.
   * Scanner coordinates are the stripe index and the position in the stripe.
   * @param pt Image coordinates of the point.
   */
  virtual Pt2i locate (const Pt2i &pt) const;


private:

  /** Current step in strip direction */
  bool *lst1, *rst1;

  /** State of the scan */
  bool ltransition, rtransition;
};

#endif
