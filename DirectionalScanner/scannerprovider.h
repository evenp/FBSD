#ifndef SCANNER_PROVIDER_H
#define SCANNER_PROVIDER_H

#include <cstdlib>
#include "directionalscanner.h"

using namespace std;



/** 
 * @class ScannerProvider scannerprovider.h
 * \brief Directional scanner provider.
 * Provides ad-hoc directional scanners in the relevant octant
 *   and according to static or dynamical needs.
 * \author {P. Even}
 */
class ScannerProvider
{
public:

  /**
   * @fn ScannerProvider()
   * \brief Builds a directional scanner provider.
   */
  ScannerProvider () : isOrtho (false),
                       xmin (0), ymin (0), xmax (100), ymax (100) { }
  
  /**
   * @fn void setSize (int sizex, int sizey)
   * \brief Sets the scanned area size.
   * @param sizex Scan area width.
   * @param sizey Scan area height.
   */
   void setSize (int sizex, int sizey) {
     xmax = xmin + sizex; ymax = ymin + sizey; }

  /**
   * @fn void setArea (int x0, int y0, int sizex, in sizey)
   * \brief Sets the scanned area size.
   * @param x0 Left column coordinate of the scan area.
   * @param y0 Lower line coordinate of the scan area.
   * @param sizex Scan area width.
   * @param sizey Scan area height.
   */
   void setArea (int x0, int y0, int sizex, int sizey) {
     xmin = x0, ymin = y0, xmax = x0 + sizex; ymax = y0 + sizey; }

  /**
   * @fn getScanner(Pt2i p1, Pt2i p2)
   * \brief Returns an incremental directional scanner.
   * Returns a directional scanner from two control points.
   * The scan strip is composed of parallel scan lines, the first one being
   *   defined by control points p1 and p2.
   * @param p1 Start control point.
   * @param p2 End control point.
   */
  DirectionalScanner *getScanner (Pt2i p1, Pt2i p2);
  
  /**
   * @fn getScanner(Pt2i p1, Pt2i p2, Pt2i v1, Pt2i v2)
   * \brief Returns an incremental directional scanner.
   * Returns a directional scanner from two points and direction v1 -> v2.
   * The scan strip is composed of parallel scan lines, centered on the middle
   *   of (p1,p2) and aligned on (v1,v2).
   * @param p1 start control point
   * @param p2 end control point
   * @param v1 direction start point
   * @param v2 direction end point
   */
  DirectionalScanner *getScanner (Pt2i p1, Pt2i p2, Pt2i v1, Pt2i v2);

  /**
   * @fn getScanner(Pt2i centre, Vr2i normal, int length)
   * \brief Returns an incremental directional scanner.
   * Returns a directional scanner from two points and direction v1 -> v2.
   * The scan strip is composed of parallel scan lines, centered on the middle
   *   of (p1,p2) and aligned on (v1,v2).
   * @param centre central point
   * @param normal scan strip normal vector
   * @param length length of a scan line
   */
  DirectionalScanner *getScanner (Pt2i centre, Vr2i normal, int length);

  /**
   * @fn getScanner(Pt2i centre, Vr2i normal, int length, bool controlable,
   *                int xmin, int ymin, nt xmax, int ymax)
   * \brief Returns an incremental directional scanner.
   * Returns a directional scanner from two points and direction v1 -> v2.
   * The scan strip is composed of parallel scan lines, centered on the middle
   *   of (p1,p2) and aligned on (v1,v2).
   * @param centre central point
   * @param normal scan strip normal vector
   * @param length length of a scan line
   * @param controlable controlability request (true for a dynamical scanner)
   */
  DirectionalScanner *getScanner (Pt2i centre, Vr2i normal,
                                  int length, bool controlable);

  /**
   * @fn setOrtho(bool status)
   * \brief Sets the orthogonal scanner modality.
   * @param status new status for the orthogonal scanner modality.
   */
  inline void setOrtho (bool status) { isOrtho = status; }


private:

  /** Orthogonal scanner modality. */
  bool isOrtho;

  /** Scan area lowest x coordinate. */
  int xmin;
  /** Scan area lowest y coordinate. */
  int ymin;
  /** Scan area highest x coordinate. */
  int xmax;
  /** Scan area highest y coordinate. */
  int ymax;
};

#endif
