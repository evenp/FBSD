#ifndef DIRECTIONAL_SCANNER_H
#define DIRECTIONAL_SCANNER_H

#include <cstdlib>
#include <vector>
#include "pt2i.h"

using namespace std;



/** 
 * @class DirectionalScanner directionalscanner.h
 * \brief Incremental directional scanner.
 * This scanner iterately provides parallel scan lines.
 * \author {P. Even and B. Kerautret}
 */
class DirectionalScanner
{
public:
  
  /**
   * @fn ~DirectionalScan()
   * \brief Deletes the scan strip.
   */
  virtual ~DirectionalScanner ();

  /**
   * @fn first(vector<Pt2i> &scan)
   * \brief Returns the central scan.
   * Fills in the vector with the central scan and returns its size.
   * @param scan vectors of points to be filled in.
   */
  virtual int first (vector<Pt2i> &scan) = 0;

  /**
   * @fn nextOnLeft(vector<Pt2i> &scan)
   * \brief Returns the next scan on the left.
   * Fills in the vector with the next scan on left and returns its size.
   * @param scan vectors of points to be filled in.
   */
  virtual int nextOnLeft (vector<Pt2i> &scan) = 0;

  /**
   * @fn nextOnRight(vector<Pt2i> &scan)
   * \brief Returns the next scan on the right.
   * Fills in the vector with the next scan on right and returns its size.
   * @param scan vectors of points to be filled in.
   */
  virtual int nextOnRight (vector<Pt2i> &scan) = 0;

  /**
   * @fn bindTo(int a, int b, int c)
   * \brief Binds the scan stripe to wrap the given digital line.
   * Resets the bounding lines parameters to center on the given line.
   * @param a New value for the 'a' parameter of the current scan stripe.
   * @param b New value for the 'b' parameter of the current scan stripe.
   * @param c New value for the medial axis of the current scan stripe.
   */
  virtual void bindTo (int a, int b, int c);

  /**
   * @fn Pt2i locate (const Pt2i &pt) const
   * \brief Returns the scanner coordinates of the givent point.
   * Scanner coordinates are the stripe index and the position in the stripe.
   * @param pt Image coordinates of the point.
   */
  virtual Pt2i locate (const Pt2i &pt) const;


protected:

  /** Scanable area. */
  int xmin, ymin, xmax, ymax;

  /** Coefficients of the discrete upper support line. */
  int dla, dlb, dlc2;

  /** Size of the discrete line pattern. */
  int nbs;

  /** Discrete line pattern and its end. */
  bool *steps, *fs;

  /** Start position of a scan for both directions. */
  int ccx, ccy, lcx, lcy, rcx, rcy;

  /** Current step in scan direction. */
  bool *lst2, *rst2;

  DirectionalScanner () { }

  /**
   * @fn DirectionalScanner(int xmini, int ymini, int xmaxi, int ymaxi,
   *                        int nb, bool* st, int sx, int sy)
   * \brief Creates an incremental directional scanner.
   * Creates a directional scanner from pattern and start.
   * The scan strip is composed of parallel scan lines, the first one being
   *   defined by a start point and a line pattern.
   * @param xmini left border of the scan area
   * @param ymini low border of the scan area
   * @param xmaxi right border of the scan area
   * @param ymaxi up border of the scan area
   * @param nb size of the support line pattern
   * @param st support line pattern
   * @param sx abscissae of the central scan start point
   * @param sy ordinate of the central scan start point
   */
  DirectionalScanner (int xmini, int ymini, int xmaxi, int ymaxi,
                      int nb, bool* st, int sx, int sy)
             : xmin (xmini), ymin (ymini), xmax (xmaxi), ymax (ymaxi),
               nbs (nb), steps (st),
               ccx (sx), ccy (sy), lcx (sx), lcy (sy), rcx (sx), rcy (sy) { }
};

#endif
