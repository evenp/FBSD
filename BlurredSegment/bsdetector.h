#ifndef BLURRED_SEGMENT_DETECTOR_H
#define BLURRED_SEGMENT_DETECTOR_H

#include "bstracker.h"
#include "bsfilter.h"
#include <iostream>

using namespace std;


/** 
 * @class BSDetector bsdetector.h
 * \brief Blurred segment detector in grey level images.
 * \author {P. Even}
 */
class BSDetector
{
public:

  /** Version number */
  static const std::string VERSION;

  /** Identifier for the final detection step. */
  static const int STEP_FINAL;
  /** Identifier for the initial detection step. */
  static const int STEP_INITIAL;
  /** Identifier for the preliminary detection step. */
  static const int STEP_PRELIM;

  /** Extraction result : void input. */
  static const int RESULT_VOID;
  /** Extraction result : successful extraction. */
  static const int RESULT_UNDETERMINED;
  /** Extraction result : successful extraction. */
  static const int RESULT_OK;
  /** Extraction result : no initial detection (bsini == NULL). */
  static const int RESULT_PRELIM_NO_DETECTION;
  /** Extraction result : too few points at initial detection. */
  static const int RESULT_PRELIM_TOO_FEW;
  /** Extraction result : no initial detection (bsini == NULL). */
  static const int RESULT_INITIAL_NO_DETECTION;
  /** Extraction result : too few points at initial detection. */
  static const int RESULT_INITIAL_TOO_FEW;
  /** Extraction result : unsuccessful density test at initial detection. */
  static const int RESULT_INITIAL_TOO_SPARSE;
  /** Extraction result : unsuccessful filter test at initial detection. */
  static const int RESULT_INITIAL_TOO_MANY_OUTLIERS;
  /** Extraction result : initial detection of a closely oriented segment. */
  static const int RESULT_INITIAL_CLOSE_ORIENTATION;
  /** Extraction result : no final detection (bsf == NULL). */
  static const int RESULT_FINAL_NO_DETECTION;
  /** Extraction result : too few points at final detection. */
  static const int RESULT_FINAL_TOO_FEW;
  /** Extraction result : unsuccessful connectivity test at final detection. */
  static const int RESULT_FINAL_TOO_SPARSE;
  /** Extraction result : unsuccessful spread test at final detection. */
  static const int RESULT_FINAL_TOO_SMALL;
  /** Extraction result : unsuccessful fragmentation test at final detection. */
  static const int RESULT_FINAL_TOO_FRAGMENTED;
  /** Extraction result : unsuccessful filter test at final detection. */
  static const int RESULT_FINAL_TOO_MANY_OUTLIERS;

  
  /**
   * \brief Creates a segment detector.
   */
  BSDetector ();

  /**
   * \brief Deletes the segment detector.
   */
  ~BSDetector ();

  /**
   * \brief Returns the minimal vertical or horizontal width.
   */
  inline int result () const { return resultValue; }

  /**
   * \brief Sets the gradient map.
   */
  void setGradientMap (VMap *data);

  /**
   * \brief Detects all blurred segments in the picture.
   * Parses X direction first, the Y direction.
   */
  void detectAll ();

  /**
   * \brief Detects all blurred segments in the picture.
   * Parses simultaneously the X and Y directions.
   */
  void detectAllWithBalancedXY ();

  /**
   * \brief Detects blurred segments between two input points.
   * @param p1 First input point.
   * @param p2 Second input point.
   */
  void detectSelection (const Pt2i &p1, const Pt2i &p2);

  /**
   * \brief Runs the last detection again.
   */
  void redetect ();

  /**
   * \brief Detects a blurred segment between two input points.
   * Step 1: For each scan line, one candidate is selected
   *         based on the gradient norm only (no direction test).
   * Step 2: Outliers pruning based on parameter space pre-filtering.
   * Step 3: For each scan line, local candidates are detected
   *         on top of gradient ridges with closest direction.
   *         The first candidates that prolongates the segment are retained.
   * Step 4: Outliers pruning based on parameter space filtering.
   * Note : Multi-detection along a stroke requires an initial start point.
   * Returns the detection status (RESULT_OK if successfull).
   * @param p1 First input point.
   * @param p2 Second input point.
   * @param centralp Set to true if the central point is provided.
   * @param pc Initial central point.
   */
  int detect (const Pt2i &p1, const Pt2i &p2,
              bool centralp = false, const Pt2i &pc = Pt2i ());

  /**
   * \brief Detects a blurred segment between two input points in static mode.
   *    Static mode means no adaptive scans and no assigned thickness control.
   * Step 1: For each scan line, one candidate is selected
   *         based on the gradient norm only (no direction test).
   * Step 2: For each scan line, local candidates are detected
   *         on top of gradient ridges with closest direction.
   *         The first candidates that prolongates the segment are retained.
   *         The directional scan is oriented on the segment of step 1.
   * Step 3: id. step 2.
   *         The directional scan is oriented on the segment of step 2.
   * Note : Multi-detection along a stroke requires an initial start point.
   * Returns the detection status (RESULT_OK if successfull).
   * @param p1 First input point.
   * @param p2 Second input point.
   * @param centralp Set to true if the central point is provided.
   * @param pc Initial central point.
   */
  int staticDetect (const Pt2i &p1, const Pt2i &p2,
                    bool centralp = false, const Pt2i &pc = Pt2i ());

  /**
   * \brief Returns the detected blurred segment at given step.
   * @param step Detection step.
   */
  BlurredSegment *getBlurredSegment (int step = STEP_FINAL) const;
    
  /**
   * \brief Returns the list of detected blurred segments at final step.
   */
  inline const vector<BlurredSegment *> getBlurredSegments () const {
    return (mbsf); }

  /**
   * \brief Avoids the deletion of the last extracted blurred segment.
   */
  inline void preserveFormerBlurredSegment () { bsf = NULL; }

  /**
   * \brief Avoids the deletion of the last extracted blurred segments.
   */
  inline void preserveFormerBlurredSegments () { mbsf.clear (); }

  /**
   * \brief Returns the assigned maximal thickness to detector.
   */
  inline int assignedThickness () const { return inThick; }

  /**
   * \brief Sets the assigned maximal thickness to detector.
   * @param val New assigned thickness value.
   */
  inline void setAssignedThickness (int val) { if (val > 0) inThick = val; }

  /**
   * \brief Returns the output blurred segment minimal size.
   */
  inline int initialSizeMinValue () const { return initialMinSize; }

  /**
   * \brief Sets the output blurred segment minimal size.
   */
  inline void setInitialMinSize (int value) {
    initialMinSize = (value >= BS_MIN_SIZE ? value : BS_MIN_SIZE); }

  /**
   * \brief Returns the threshold used for maximal gradient detection.
   */
  inline int getSensitivity () const {
    return (gMap->getGradientThreshold ()); }

  /**
   * \brief Increments the threshold used for maximal gradient detection.
   * @param inc Increment value.
   */
  inline void incSensitivity (int inc) {
    return (gMap->incGradientThreshold (inc)); }

  /**
   * \brief Returns the gradient threshold used for local max filtering.
   */
  inline int getGradientResolution () const {
    return (gMap->getGradientResolution ()); }

  /**
   * \brief Increments the gradient threshold used for local maximal filtering.
   * @param inc Increment value.
   */
  inline void incGradientResolution (int inc) {
    return (gMap->incGradientResolution (inc)); }

  /**
   * \brief Returns the stroke sweeping step value for automatic detections.
   */
  inline int getAutoSweepingStep () const { return autoSweepingStep; }

  /**
   * \brief Sets the stroke sweeping step value for automatic detections.
   */
  inline void setAutoSweepingStep (int number) {
    if (number > 0 && number < gMap->getWidth () / 8) autoSweepingStep = number; }

  /**
   * \brief Returns the pixel lack tolerence.
   */
  inline int getPixelLackTolerence () const {
    return bst2->pixelLackTolerence (); }

  /**
   * \brief Sets the pixel lack tolerence.
   */
  inline void setPixelLackTolerence (int number) {
    bst2->setPixelLackTolerence (number); }

  /**
   * \brief Returns the count of accepted points to release a failure.
   */
  inline int getRestartOnLack () { return bst2->getRestartOnLack (); }

  /**
   * \brief Switches on or off the automatic restart after failure.
   */
  inline void switchAutoRestart () { bst2->switchAutoRestart (); }

  /**
   * \brief Returns whether the crosswise segment detection is set.
   */
  inline bool trackCrosswiseOn () const { return bst2->trackCrosswiseOn (); }

  /**
   * \brief Switches on or off the crosswise segment detection.
   */
  inline void switchTrackCrosswise () { bst2->switchTrackCrosswise (); }

  /**
   * \brief Returns the preliminary detection modality status.
   */
  inline bool isPreliminary () { return (prelimDetectionOn); }

  /**
   * \brief Switches preliminary detection modality.
   */
  void switchPreliminary ();

  /**
   * \brief Returns whether opposite edge direction is set in double edge mode.
   */
  inline bool isOppositeGradientOn () const
  {
    return (oppositeGradientDir);
  }

  /**
   * \brief Inverts the edge direction for detection stage.
   * Effective only in single edge detection mode.
   * Returns whether the modification was actually made.
   */
  bool switchOppositeGradient ();

  /**
   * \brief Returns whether opposite edge direction is set in double edge mode.
   */
  inline bool isSingleEdgeModeOn () const
  {
    if (gMap != NULL) return (gMap->isOrientationConstraintOn ());
    return (true);
  }

  /**
   * \brief Switches between single and double edge detection.
   */
  inline void switchSingleOrDoubleEdge ()
  {
    if (gMap != NULL) gMap->switchOrientationConstraint ();
  }

  /**
   * \brief Returns whether a single edge mode is set for multidetections.
   */
  inline bool isSingleMultiOn () const
  {
    return (singleMultiOn);
  }

  /**
   * \brief Switches between single and double edge mode for multidetections.
   */
  inline void switchSingleOrDoubleMultiDetection ()
  {
    singleMultiOn = ! singleMultiOn;
  }

  /**
   * \brief Returns true if the fine scan is recentred on the detected segment.
   */
  inline bool isScanRecentering () { return recenteringOn; }

  /**
   * \brief Switches on or off the scan recentering modality.
   */
  inline void switchScanRecentering () { recenteringOn = ! recenteringOn; }

  /**
   * \brief Returns true if the multi-selection modality is set.
   */
  inline bool isMultiSelection () { return multiSelection; }

  /**
   * \brief Switches on or off the multi-selection modality.
   */
  inline void switchMultiSelection () { multiSelection = ! multiSelection; }

  /**
   * \brief Returns the scan lines at final step.
   */
  const vector <vector <Pt2i> > getFinalScans () const;

  /**
   * \brief Returns whether the final scan record modality is set.
   */
  bool finalScansRecordOn () const;

  /**
   * \brief Sets the scan record modality at final step.
   * @param status Sets on if true, off otherwise.
   */
  void setFinalScansRecord (bool status);

  /**
   * \brief Toggles the initial step bounding.
   */
  inline void switchInitialBounding () { bst1->switchScanExtent (); }

  /**
   * \brief Returns the maximal scan extent of the initial detection.
   */
  inline int initialDetectionMaxExtent () { return (bst1->maxScanExtent ()); }

  /**
   * \brief Returns the proximity threshold used for fast tracking.
   */
  inline int getFastTrackProximityThreshold () const {
    return (bst1->getProximityThreshold ()); }

  /**
   * \brief Increments the proximity threshold used for fast tracking.
   * @param inc Increment sign.
   */
  inline void incFastTrackProximityThreshold (bool inc) {
    bst1->incProximityThreshold (inc); }
    
  /**
   * \brief Returns the proximity test status.
   */
  inline bool fastTrackProximityConstraintOn () const {
    return (bst1->proximityConstraintOn ()); }

  /**
   * \brief Switches the proximity test used for fast tracking.
   */
  inline void switchFastTrackProximityConstraint () {
    bst1->switchProximityConstraint (); }

  /**
   * \brief Returns if the dynamic scans modality is set.
   */
  inline bool dynamicScansOn () { return bst2->dynamicScansOn (); }

  /**
   * \brief Switches on or off the dynamic scans modality.
   */
  inline void switchDynamicScans () { bst2->toggleDynamicScans (); }

  /**
   * \brief Sets the dynamic scans modality on or off.
   */
  inline void setDynamicScans (bool onOff) { bst2->setDynamicScans (onOff); }

  /**
   * \brief Returns if the orthographic scans modality set.
   */
  inline bool orthoScansOn () { return bst1->orthoScansOn (); }

  /**
   * \brief Switches on or off the orthogonal scans modality.
   */
  void switchOrthoScans ();

  /**
   * \brief Returns if the thickenning control is activated.
   */
  inline bool isAssignedThicknessControlOn () const {
    return bst2->isAssignedThicknessControlOn (); }

  /**
   * \brief Toggles the thickenning control.
   */
  inline void toggleAssignedThicknessControl () {
    bst2->toggleAssignedThicknessControl (); }

  /**
   * \brief Returns the assigned thickness control delay.
   */
  inline int getAssignedThicknessControlDelay () const {
    return bst2->getAssignedThicknessControlDelay (); }

  /**
   * \brief Increments the assigned thickness control delay.
   */
  inline void incAssignedThicknessControlDelay (int val) {
    bst2->incAssignedThicknessControlDelay (val); }

  /**
   * \brief Returns if one of the filter is activated.
   */
  inline bool isOneFilterSet () { return (filteringOn || prefilteringOn); }

  /**
   * \brief Returns a blurred segment filter.
   * @param step Initial step addressed if set to 0, final step otherwise.
   */
  inline BSFilter *getFilter (int step) const {
    return (step == STEP_FINAL ? lsf2 : lsf1); }

  /**
   * \brief Returns the accepted points by the blurred segment filter.
   * @param step Initial step addressed if set to 0, final step otherwise.
   */
  inline vector<Pt2i> getAccepted (int step) const {
    return (step == STEP_FINAL ?
      (filteringOn ? lsf2->getAccepted () : bsf->getAllPoints ()) :
      (prefilteringOn ? lsf1->getAccepted () : bsini->getAllPoints ())); }

  /**
   * \brief Returns the rejected points by the line space based filter.
   * @param step Initial step addressed if set to 0, final step otherwise.
   */
  vector<Pt2i> getRejected (int step) const;

  /**
   * \brief Returns if the given line space based filter is activated.
   * @param step Initial step addressed if set to 0, final step otherwise.
   */
  inline bool isFiltering (int step) const {
    if (step == STEP_FINAL) return (filteringOn);
    else if (step == STEP_INITIAL) return (prefilteringOn);
    else return false; }

  /**
   * \brief Toggles the use of the given line space based filter.
   * @param step Initial step addressed if set to 0, final step otherwise.
   */
  void switchFiltering (int step);

  /**
   * \brief Returns the blurred segment size before pre-filtering.
   */
  inline int prefilteringInputSize () {
    return (prefilteringOn ? lsf1->blurredSegmentInitialSize () : 0); }

  /**
   * \brief Returns the blurred segment size after pre-filtering.
   */
  inline int prefilteringOutputSize () {
    return (prefilteringOn ? lsf1->blurredSegmentFinalSize () : 0); }

  /**
   * \brief Returns whether the density test at initial step is set.
   */
  inline bool isInitialSparsityTestOn () const {
    return (initialSparsityTestOn); }

  /**
   * \brief Switches on or off the initial density test modality.
   */
  inline void switchInitialSparsityTest () {
    initialSparsityTestOn = ! initialSparsityTestOn; }

  /**
   * \brief Returns whether the density test at final step is set.
   */
  inline bool isFinalSparsityTestOn () const { return (finalSparsityTestOn); }

  /**
   * \brief Switches on or off the final density test modality.
   */
  inline void switchFinalSparsityTest () {
    finalSparsityTestOn = ! finalSparsityTestOn; }

  /**
   * \brief Returns whether the size test at final step is set.
   */
  inline bool isFinalSizeTestOn () const { return (finalSizeTestOn); }

  /**
   * \brief Switches on or off the final size test modality.
   */
  inline void switchFinalSizeTest () {
    finalSizeTestOn = ! finalSizeTestOn; }

  /**
   * \brief Returns the minimal size of final blurred segments.
   */
  inline int finalSizeMinValue () const {
    return (finalSizeTestOn ? finalMinSize : BS_MIN_SIZE); }

  /**
   * \brief Returns the minimal spread of final blurred segments.
   */
  inline void setFinalSizeMinValue (int val) {
    finalMinSize = (val < BS_MIN_SIZE ? BS_MIN_SIZE : val); }

  /**
   * \brief Returns the connectivity constraint status.
   */
  inline bool isFinalFragmentationTestOn () const {
    return (finalFragmentationTestOn); }

  /**
   * \brief Switches on or off the connectivity constraint.
   */
  inline void switchFinalFragmentationTest () {
    finalFragmentationTestOn = ! finalFragmentationTestOn; }

  /**
   * \brief Returns the minimal size of the segment connected components.
   */
  inline int fragmentSizeMinValue () const { return fragmentMinSize; }

  /**
   * \brief Increments the minimal size of the connected components.
   * @param increase Positive increment if true, negative otherwise.
   */
  bool incFragmentSizeMinValue (bool increase);

  /*
   * \brief Returns the count of trials in a multi-detection.
   */
  inline int countOfTrials () const { return (nbtrials); }

  /**
   * \brief Returns the maximum number of detections set for a multi-detection.
   */
  inline int getMaxDetections () const { return maxtrials; }

  /**
   * \brief Sets the maximum number of trials in a multi-detection.
   * Just kept for IPOL demo compatibility.
   * @param nb Number of trials (0 if illimited).
   */
  inline void setMaxTrials (int nb) { maxtrials = nb; resetMaxDetections (); }

  /**
   * \brief Increments the maximum number of detections in a multi-detection.
   * @param dir Increments if true, otherwise decrements.
   */
  void incMaxDetections (bool dir);

  /**
   * \brief Sets the maximum number of detections in a multi-detection.
   * @param nb Number of detections (0 if illimited).
   */
  inline void setMaxDetections (int nb) { maxtrials = nb; }

  /**
   * \brief Resets the maximum number of detections set for a multi-detection.
   */
  inline void resetMaxDetections () { maxtrials = 0; }

  /**
   * \brief Gets the last detection inputs.
   * @param step Detection step.
   * @param p1 Input stroke first point to fill in.
   * @param p2 Input stroke end point to fill in.
   * @param swidth Scan width if not set by P1 and p2, otherwise 0.
   * @param pc Input central point to fill in.
   */
  void getScanInput (int step,
                     Pt2i &p1, Pt2i &p2, int &swidth, Pt2i &pc) const;

  /**
   * \brief Retuns whether static detections (without ADS and ATC) are used.
   */
  inline bool staticDetectorOn () const { return staticDetOn; }

  /**
   * \brief Sets the static detection (without ADS and ATC) activation.
   * @param status Activation status.
   */
  void setStaticDetector (bool status);

  /**
   * \brief Returns the version number.
   */
  std::string version ();


private :

  /** Default value for the scan width for fast tracks. */
  static const int DEFAULT_FAST_TRACK_SCAN_WIDTH;
  /** Default value for the assigned thickess to detection method. */
  static const int DEFAULT_ASSIGNED_THICKNESS;
  /** Additional assigned thickness margin for fast tracks. */
  static const int FAST_TRACK_MARGIN;
  /** Default minimal size of the initial blurred segment. */
  static const int DEFAULT_INITIAL_MIN_SIZE;
  /** Minimal size of a blurred segment. */
  static const int BS_MIN_SIZE;
  /** Default minimal size of the final blurred segment. */
  static const int DEFAULT_FINAL_MIN_SIZE;
  /** Default value for the minimal size of segment fragments. */
  static const int DEFAULT_FRAGMENT_MIN_SIZE;
  /** Default value of the stroke sweeping step for automatic detections. */
  static const int DEFAULT_AUTO_SWEEPING_STEP;
  /** Default value for the preliminary stroke half length. */
  static const int PRELIM_MIN_HALF_WIDTH;


  /** Gradient map. */
  VMap *gMap;

  /** Selects points with opposite gradient direction.
   *  Opposite to gradient direction at start point.
   *  Used to detect two close edges with opposite gradients.
   */
  bool oppositeGradientDir;
  /** Minimal size of the initial segment. */
  int initialMinSize;
  /** Final fragmentation test status. */
  bool finalFragmentationTestOn;
  /** Minimal size of the segment fragments. */
  int fragmentMinSize;
  /** Automatic scan recentering (based on previous detection). */
  bool recenteringOn;
  /** Sparsity test modality after initial detection. */
  bool initialSparsityTestOn;
  /** Sparsity test modality after final detection. */
  bool finalSparsityTestOn;
  /** Size test modality after final detection. */
  bool finalSizeTestOn;
  /** Minimal size of the final segment. */
  int finalMinSize;
  /** Count of small BS eliminated by the spread test. */
  int nbSmallBS;
  /** Segment multi-selection modality status. */
  bool multiSelection;
  /** Single or double mode for multi-selections. */
  bool singleMultiOn;
  /** Count of trials in a multi-detection. */
  int nbtrials;
  /** Maximum number of trials in a multi-detection. */
  int maxtrials;
  /** Automatic detection modality. */
  bool autodet;
  /** Stroke sweeping step for the automatic extraction. */
  int autoSweepingStep;
  /** Result of the blurred segment extraction. */
  int resultValue;
  /** Activation status of static detector (IWCIA'09). */
  bool staticDetOn;

  /** Assigned maximal thickness to the detector. */
  int inThick;

  /** Last input start point. */
  Pt2i prep1;
  /** Last input end point. */
  Pt2i prep2;
  /** Last input central point. */
  Pt2i prepc;
  /** Preliminary fast scan width if not set by an input selection. */
  int prewidth;
  /** Preliminary stage modality. */
  bool prelimDetectionOn;
  /** Preliminary rough tracker. */
  BSTracker *bst0;
  /** Preliminary detected blurred segment. */
  BlurredSegment *bspre;

  /** Initial rough tracker. */
  BSTracker *bst1;
  /** Last input start point for initial step. */
  Pt2i inip1;
  /** Last input end point for initial step. */
  Pt2i inip2;
  /** Last input central point for initial step. */
  Pt2i inipc;
  /** Initial fast scan width if not set by an input selection. */
  int iniwidth;
  /** Initially detected blurred segment (initial step result). */
  BlurredSegment *bsini;

  /** Fine tracker. */
  BSTracker *bst2;
  /** Fine tracker for static detections (without ADS and ATC). */
  BSTracker *bstStatic;
  /** Detected blurred segment (final result). */
  BlurredSegment *bsf;
  /** Detected blurred segments in case of multi-detection (final results). */
  vector<BlurredSegment *> mbsf;

  /** Initial segment filtering modality. */
  bool prefilteringOn;
  /** Blurred segment pre-filter. */
  BSFilter *lsf1;

  /** Detected segment filtering modality. */
  bool filteringOn;
  /** Blurred segment post-filter. */
  BSFilter *lsf2;


  /**
   * \brief Detects all blurred segments between two input points.
   *   Returns the continuation modality.
   * @param p1 First input point.
   * @param p2 Second input point.
   */
  bool runMultiDetection (const Pt2i &p1, const Pt2i &p2);

  /**
   * \brief Resets the multi-selection list.
   */
  void freeMultiSelection ();
};
#endif
