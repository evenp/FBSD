#ifndef BLURRED_SEGMENT_TRACKER_H
#define BLURRED_SEGMENT_TRACKER_H

#include <vector>
#include "scannerprovider.h"
#include "blurredsegment.h"
#include "vmap.h"

using namespace std;


/** 
 * @class BSTracker bstracker.h
 * \brief Blurred segment tracker in grey level images.
 * \author {P. Even and B. Kerautret}
 */
class BSTracker
{
public:

  /** Minimal length of scan length to be processed. */
  static const int MIN_SCAN;

  
  /**
   * \brief Creates a blurred segment tracker.
   */
  BSTracker ();

  /**
   * \brief Deletes the blurred segment tracker.
   */
  ~BSTracker ();

  /**
   * \brief Clears off stored information.
   */
  void clear ();

  /**
   * \brief Builds and returns a blurred segment from only gradient maximum.
   * @param bsMaxWidth Blurred segment assigned maximal width.
   * @param p1 Initial stroke start point.
   * @param p2 Initial stroke end point.
   * @param swidth Set to 0 if no start point is provided.
   * @param pc Initial segment start point (if swidth is set).
   */
  BlurredSegment *fastTrack (int bsMaxWidth,
                             const Pt2i &p1, const Pt2i &p2,
                             int swidth = 0, const Pt2i &pc = Pt2i ());

  /**
   * \brief Builds and returns a blurred segment from local gradient maxima.
   * Finer detection using gradient ridges and direction input.
   * @param bsMaxWidth Initial assigned maximal width of the blurred segment.
   * @param center Central point of the scan.
   * @param scandir Scan direction
   * @param scanwidth Width of the scan strip.
   * @param gref Gradient vector reference to select candidates.
   */
  BlurredSegment *fineTrack (int bsMaxWidth,
                             const Pt2i &center, const Vr2i &scandir,
                             int scanwidth, const Vr2i &gref);

  /**
   * \brief Returns the pixel lack tolerence for exdending the blurred segment..
   */
  inline int pixelLackTolerence () const { return acceptedLacks; }

  /**
   * \brief Sets the pixel lack tolerence for extending the blurred segment.
   */
  inline void setPixelLackTolerence (int number)
  {
    if (number >= 0)
    {
      acceptedLacks = number;
      if (minRestart != 1) minRestart = number;
    }
  }

  /**
   * \brief Returns the count of accepted points to release a failure.
   */
  inline int getRestartOnLack () const { return minRestart; }

  /**
   * \brief Switches on or off the automatic restart after failure.
   */
  inline void switchAutoRestart ()
  {
    minRestart = (minRestart == 1 ? acceptedLacks : 1);
  }

  /**
   * \brief Returns whether the crosswise segment detection is set.
   */
  inline bool trackCrosswiseOn () const { return trackCrosswise; }

  /**
   * \brief Switches on or off the crosswise segment detection.
   */
  inline void switchTrackCrosswise () { trackCrosswise = ! trackCrosswise; }

  /**
   * \brief Sets the image data.
   */
  void setGradientMap (VMap *data);

  /**
   * \brief Returns the proximity test status.
   */
  inline bool proximityConstraintOn () const { return (! proxTestOff); }

  /**
   * \brief Switches the proximity test used for fast tracking.
   */
  inline void switchProximityConstraint () { proxTestOff = ! proxTestOff; }

  /**
   * \brief Returns the proximity threshold used for fast tracking.
   */
  inline int getProximityThreshold () const { return proxThreshold; }

  /**
   * \brief Increments the proximity threshold used for fast tracking.
   * @param bool Increment value.
   */
  inline void incProximityThreshold (bool inc) {
      proxThreshold += (inc ? 1 : -1);
      if (proxThreshold < 1) proxThreshold = 1; }

  /**
   * \brief Returns the registered upper bounds of the final scan lines.
   * @param side Upper bound if set to 1, lower bound otherwise.
   */
  inline vector<Pt2i> getScanBound (int side) const {
    return (side == 1 ? scanBound1 : scanBound2); }

  /**
   * \brief Returns the registered scan lines.
   */
  inline const vector <vector <Pt2i> > &getScans () const { return scanLine; }

  /**
   * \brief Returns whether the scan record modality is set.
   */
  inline bool scanRecordOn () { return recordScans; }

  /**
   * \brief Sets the scan record modality.
   * @param status Sets on if true, off otherwise.
   */
  inline void setScanRecord (bool status) { recordScans = status; }

  /**
   * \brief Returns if the dynamic scans are used.
   */
  inline bool dynamicScansOn () { return dynamicScans; }

  /**
   * \brief Toggles the dynamic scans use.
   */
  inline void toggleDynamicScans () { dynamicScans = ! dynamicScans; }

  /**
   * \brief Switches the scan extent limitation.
   */
  void switchScanExtent ();

  /**
   * \brief Returns the scan extent limit.
   */
  inline int maxScanExtent () { return (maxScan); }

  /**
   * \brief Sets the dynamic scans on or off.
   */
  inline void setDynamicScans (bool onOff) { dynamicScans = onOff; }

  /**
   * \brief Returns if the thickenning control is activated.
   */
  inline bool isThickenningOn () const { return thickenningOn; }

  /**
   * \brief Toggles the thickenning control.
   */
  inline void toggleThickenning () { thickenningOn = ! thickenningOn; }

  /**
   * \brief Returns the thickenning limit.
   */
  inline int getThickenningLimit () const { return thickenningLimit; }

  /**
   * \brief Increments the thickenning limit.
   */
  inline void incThickenningLimit (int val) {
    thickenningLimit += val; if (thickenningLimit < 1) thickenningLimit = 1; }

  /**
   * \brief Returns if the thinning is activated.
   */
  inline bool isThinningOn () const { return thinningOn; }

  /**
   * \brief Toggles the thinning strategy.
   */
  inline void toggleThinning () { thinningOn = ! thinningOn; }

  /**
   * \brief Switches on or off the orthogonal scanning modality.
   */
  void switchOrthoScans ();

  /**
   * \brief Switches on or off the orthogonal scanning modality.
   */
  inline bool orthoScansOn () { return orthoScan; }


private :

  // Segment detection default parameters.
  /** Default value for the accepted number of successive lacks. */
  static const int DEFAULT_ACCEPTED_LACKS;
  /** Default value for the proximity test used for fast tracking. */
  static const int DEFAULT_PROX_THRESHOLD;
  /** Default value for the maximal number of scans processed on each side. */
  static const int DEFAULT_MAX_SCAN;

  // Dynamical scan default parameters.
  /* Count of points before activating the fitting on the detected segment. */
  static const int DEFAULT_FITTING_DELAY;

  // Width thickenning default parameters.
  /* Maximal count of points since last minimal width growing. */
  static const int DEFAULT_THICKENNING_LIMIT;

  // Width thinning default parameters.
  /* Count of points before activating the width thinning. */
  static const int DEFAULT_THINNING_DELAY;
  /** Value which controls the width thinning speed.
   * It is the percentage of width offset withdrawn from the width tolerence.
   * Width offset = width tolerence - blurred segment width. */
  static const int DEFAULT_THINNING_SPEED;
  /** Minimal value of the percentage of width offset wrt segment width. */
  static const int DEFAULT_THINNING_REACH;
  /** Resolution of the max segment width when thinning. */
  static const int DEFAULT_THINNING_RESOLUTION;

  /** Segment stop information : no start point found. */
  static const int FAILURE_NO_START;
  /** Segment stop information : image bound reached on the right. */
  static const int FAILURE_IMAGE_BOUND_ON_RIGHT;
  /** Segment stop information : image bound reached on the left. */
  static const int FAILURE_IMAGE_BOUND_ON_LEFT;
  /** Segment stop information : lost orientation at dynamical reset start. */
  static const int FAILURE_LOST_ORIENTATION;


  /** Scanned map left bound. */
  int xmin;
  /** Scanned map lower bound. */
  int ymin;
  /** Scanned map width. */
  int width;
  /** Scanned map height. */
  int height;

  /** Number of awaited points after each failure. */
  int minRestart;
  /** Accepted number of successive lacks (wrt restart points). */
  int acceptedLacks;
  /** Status of the proximity constraint used for fast tracking. */
  bool proxTestOff;
  /** Proximity threshold used for fast tracking. */
  int proxThreshold;

  /** Orthogonal scanning modality status */
  bool orthoScan;
  /** Kind of scans used : dynamic if true. */
  bool dynamicScans;
  /** Minimal detection width before activating the dynamical scans. */
  int fittingDelay;
  /** Crosswise segment detection modality. */
  bool trackCrosswise;

  /** Segment thinning strategy. */
  bool thinningOn;
  /** Width thinning delay. */
  int thinningDelay;
  /** Width thinning speed : amount of thinning offset withdrawn. */
  AbsRat thinningSpeed;
  /** Minimal control width wrt detected segment width when thinning. */
  AbsRat thinningReach;

  /** Segment thickening control modality. */
  bool thickenningOn;
  /** Count of expansion without width growing to stop the thickenning. */
  int thickenningLimit;

  /** Gradient map. */
  VMap *gMap;

  /** Maximum number of scans. */
  int maxScan;

  /** Failure cause. */
  int fail;

  /** Candidates array for internal use. */
  int *cand;
  /** Index of the last successful scan on right side. */
  int rscan;
  /** Index of the last successful scan on left side. */
  int lscan;


  /** Directional scanner provider.
   * Automatically selects the appropriate octant. */
  ScannerProvider scanp;
  /** Upper bound of the scan. */
  vector<Pt2i> scanBound1;
  /** Lower bound of the scan. */
  vector<Pt2i> scanBound2;
  /** Scan lines. */
  vector <vector <Pt2i> > scanLine;
  /** Dynamical scanner record modality. */
  bool recordScans;
};
#endif
