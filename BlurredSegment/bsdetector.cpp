#include "bsdetector.h"
//#include "linespacefilter.h"


const std::string BSDetector::VERSION = "0.2.1";
const int BSDetector::STEP_FINAL = 0;
const int BSDetector::STEP_INITIAL = 1;
const int BSDetector::STEP_PRELIM = 2;

const int BSDetector::RESULT_VOID = -2;
const int BSDetector::RESULT_UNDETERMINED = -1;
const int BSDetector::RESULT_OK = 0;
const int BSDetector::RESULT_PRELIM_NO_DETECTION = 1;
const int BSDetector::RESULT_PRELIM_TOO_FEW = 2;
const int BSDetector::RESULT_INITIAL_NO_DETECTION = 11;
const int BSDetector::RESULT_INITIAL_TOO_FEW = 12;
const int BSDetector::RESULT_INITIAL_TOO_SPARSE = 13;
const int BSDetector::RESULT_INITIAL_TOO_MANY_OUTLIERS = 14;
const int BSDetector::RESULT_INITIAL_CLOSE_ORIENTATION = 15;
const int BSDetector::RESULT_FINAL_NO_DETECTION = 21;
const int BSDetector::RESULT_FINAL_TOO_FEW = 22;
const int BSDetector::RESULT_FINAL_TOO_SPARSE = 23;
const int BSDetector::RESULT_FINAL_TOO_SMALL = 24;
const int BSDetector::RESULT_FINAL_TOO_MANY_OUTLIERS = 25;

const int BSDetector::DEFAULT_FAST_TRACK_SCAN_WIDTH = 16;
const int BSDetector::DEFAULT_ASSIGNED_THICKNESS = 3;
const int BSDetector::DEFAULT_FAST_TRACK_MAX_MARGIN = 2;

const int BSDetector::DEFAULT_BS_MIN_SIZE = 5;
const int BSDetector::ABSOLUTE_BS_MIN_SIZE = 3;
const int BSDetector::DEFAULT_FINAL_MIN_SIZE = 10;
const int BSDetector::DEFAULT_CONNECT_MIN_SIZE = 5;
const int BSDetector::DEFAULT_AUTO_SWEEPING_STEP = 15;
const int BSDetector::PRELIM_MIN_HALF_WIDTH = 10;


BSDetector::BSDetector ()
{
  gMap = NULL;

  inThick = DEFAULT_ASSIGNED_THICKNESS;
  imaxMargin = DEFAULT_FAST_TRACK_MAX_MARGIN;

  prelimDetectionOn = false;
  bst0 = (prelimDetectionOn ? new BSTracker () : NULL);
  bst1 = new BSTracker ();
  // bst1->setPixelLackTolerence (bst1->getVicinityThreshold ());
  bst2 = new BSTracker ();
  bstold = new BSTracker ();
  if (bstold->dynamicScansOn ()) bstold->toggleDynamicScans ();
  if (bstold->isAssignedThicknessControlOn ())
    bstold->toggleAssignedThicknessControl ();

  staticDetOn = false;
  prefilteringOn = false;
  //lsf1 = (prefilteringOn ? new LineSpaceFilter () : NULL);
  lsf1 = (prefilteringOn ? new BSFilter () : NULL);
  filteringOn = false;
  //lsf2 = (filteringOn ? new LineSpaceFilter () : NULL);
  lsf2 = (filteringOn ? new BSFilter () : NULL);

  edgeDirection = 0;   // detects strokes (not only edges)
  bsMinSize = DEFAULT_BS_MIN_SIZE;
  ccOn = false;
  ccMinSize = DEFAULT_CONNECT_MIN_SIZE;
  recenteringOn = true;
  fittingOn = false;
  densityTestOn = true;
  finalDensityTestOn = false;
  finalLengthTestOn = false;
  finalSizeTestOn = true;
  finalSizeMinVal = DEFAULT_FINAL_MIN_SIZE;
  // nbSmallBS = 0;
  multiSelection = false;
  autodet = false;
  autoSweepingStep = DEFAULT_AUTO_SWEEPING_STEP;
  maxtrials = 0;
  nbtrials = 0;

  bspre = NULL;
  bsini = NULL;
  bsf = NULL;
  resultValue = RESULT_UNDETERMINED;
}


BSDetector::~BSDetector ()
{
  if (prelimDetectionOn) delete bst0;
  delete bst1;
  delete bst2;
  if (lsf1 != NULL) delete lsf1;
  if (lsf2 != NULL) delete lsf2;
  if (bsini != NULL) delete bsini;
  if (bsf != NULL) delete bsf;
  vector <BlurredSegment *>::iterator it = mbsf.begin ();
  while (it != mbsf.end ()) delete (*it++);
}


void BSDetector::setGradientMap (VMap *data)
{
  gMap = data;
  if (prelimDetectionOn) bst0->setGradientMap (data);
  bst1->setGradientMap (data);
  bst2->setGradientMap (data);
  bstold->setGradientMap (data);
}


void BSDetector::detectAll ()
{
  autodet = true;
  freeMultiSelection ();
  gMap->setMasking (true);
  gMap->clearMask ();

  bool isnext = true;
  nbtrials = 0;
  // nbSmallBS = 0;
  int width = gMap->getWidth ();
  int height = gMap->getHeight ();
  for (int x = width / 2; isnext && x > 0; x -= autoSweepingStep)
    isnext = runMultiDetection (Pt2i (x, 0), Pt2i (x, height - 1));
  for (int x = width / 2 + autoSweepingStep; isnext && x < width - 1; x += autoSweepingStep)
    isnext = runMultiDetection (Pt2i (x, 0), Pt2i (x, height - 1));
  for (int y = height / 2; isnext && y > 0; y -= autoSweepingStep)
    isnext = runMultiDetection (Pt2i (0, y), Pt2i (width - 1, y));
  for (int y = height / 2 + autoSweepingStep; isnext && y < height - 1; y += autoSweepingStep)
    isnext = runMultiDetection (Pt2i (0, y), Pt2i (width - 1, y));
  if (maxtrials > (int) (mbsf.size ())) maxtrials = 0;
  // cout << nbSmallBS << " petits BS elimines" << endl;

  gMap->setMasking (false);
}


void BSDetector::detectAllWithBalancedXY ()
{
  autodet = true;
  freeMultiSelection ();
  gMap->setMasking (true);
  gMap->clearMask ();

  bool isnext = true;
  nbtrials = 0;
  // nbSmallBS = 0;
  int width = gMap->getWidth ();
  int height = gMap->getHeight ();
  int xg = width / 2, yb = height / 2;
  int xd = xg + autoSweepingStep, yh = yb + autoSweepingStep;
  bool agauche = true, enbas = true, adroite = true, enhaut = true;
  while (isnext && (agauche || enbas || adroite || enhaut))
  {
    if (agauche)
    {
      isnext = runMultiDetection (Pt2i (xg, 0), Pt2i (xg, height - 1));
      xg -= autoSweepingStep;
      if (xg <= 0) agauche = false;
    }
    if (isnext && enbas)
    {
      isnext = runMultiDetection (Pt2i (0, yb), Pt2i (width - 1, yb));
      yb -= autoSweepingStep;
      if (yb <= 0) enbas = false;
    }
    if (isnext && adroite)
    {
      isnext = runMultiDetection (Pt2i (xd, 0), Pt2i (xd, height - 1));
      xd += autoSweepingStep;
      if (xd >= width - 1) adroite = false;
    }
    if (isnext && enhaut)
    {
      isnext = runMultiDetection (Pt2i (0, yh), Pt2i (width - 1, yh));
      yh += autoSweepingStep;
      if (yh >= height - 1) enhaut = false;
    }
  }
  if (maxtrials > (int) (mbsf.size ())) maxtrials = 0;
  // cout << nbSmallBS << " petits BS elimines" << endl;
  gMap->setMasking (false);
}


void BSDetector::detectSelection (const Pt2i &p1, const Pt2i &p2)
{
  autodet = false;
  freeMultiSelection ();
  if (multiSelection)
  {
    gMap->setMasking (true);
    gMap->clearMask ();
    nbtrials = 0;
    // nbSmallBS = 0;
    runMultiDetection (p1, p2);
    // cout << nbSmallBS << " petits BS elimines" << endl;
    if (maxtrials > (int) (mbsf.size ())) maxtrials = 0;
    gMap->setMasking (false);
  }
  else
    if (staticDetOn) resultValue = staticDetect (p1, p2);
    else resultValue = detect (p1, p2);
}


void BSDetector::redetect ()
{
  if (autodet) detectAll ();
  else detectSelection (inip1, inip2);
}


void BSDetector::freeMultiSelection ()
{
  vector<BlurredSegment *>::iterator it = mbsf.begin ();
  while (it != mbsf.end ()) delete (*it++);
  mbsf.clear ();
}


bool BSDetector::runMultiDetection (const Pt2i &p1, const Pt2i &p2)
{
  vector<Pt2i> pts;
  p1.draw (pts, p2);
  int locmax[pts.size ()];
  int nlm = gMap->localMax (locmax, pts);
  bool isnext = true;
  for (int i = 0; isnext && i < nlm; i++)
  {
    Pt2i ptstart = pts.at (locmax[i]);
    if (gMap->isFree (ptstart))
    {
      int oldEdgeDir = edgeDirection;
      if (edgeDirection != 0) edgeDirection = 1;
      while (isnext && edgeDirection >= -1)
      {
        int res = RESULT_VOID;
        if (staticDetOn) res = staticDetect (p1, p2, true, ptstart);
        else res = detect (p1, p2, true, ptstart);
        if (res == RESULT_OK)
        {
          gMap->setMask (bsf->getAllPoints ());
          mbsf.push_back (bsf);
          bsf = NULL; // to avoid BS deletion
          if ((int) (mbsf.size ()) == maxtrials) isnext = false;
        }
        nbtrials ++;
        edgeDirection -= 2;
      }
      edgeDirection = oldEdgeDir;
    }
  }
  return (isnext);
}


int BSDetector::detect (const Pt2i &p1, const Pt2i &p2,
                        bool centralp, const Pt2i &pc)
{
  // Entry check
  //------------
  if (p1.equals (p2)
      || ((! centralp) && p1.chessboard (p2) < BSTracker::MIN_SCAN))
    return RESULT_VOID;


  // Clearance
  //----------
  if (prelimDetectionOn) bst0->clear ();
  bst1->clear ();
  bst2->clear ();
  if (prefilteringOn) lsf1->clear ();
  if (filteringOn) lsf2->clear ();
  if (bspre != NULL) delete bspre;
  bspre = NULL;
  if (bsini != NULL) delete bsini;
  bsini = NULL;
  if (bsf != NULL) delete bsf;
  bsf = NULL;

  prep1.set (p1);
  prep2.set (p2);
  prewidth = (centralp ? DEFAULT_FAST_TRACK_SCAN_WIDTH : 0);
  prepc.set (pc);

  // Preliminary based on highest gradient without orientation constraint
  //---------------------------------------------------------------------
  if (prelimDetectionOn)
  {
    bspre = bst0->fastTrack (inThick + imaxMargin,
                             prep1, prep2, prewidth, prepc);
    if (bspre == NULL || bspre->size () < bsMinSize)
      return (bspre == NULL ? RESULT_PRELIM_NO_DETECTION
                            : RESULT_PRELIM_TOO_FEW);

    Vr2i v0 = bspre->getSupportVector ();
    int l = v0.chessboard ();
    if (l != 0)
    {
      Pt2i pcentral = bspre->getSegment()->centerOfIntersection (prep1, prep2);
      int detw = 2 * (1 + bspre->minimalWidth().floor ());
      if (detw < PRELIM_MIN_HALF_WIDTH) detw = PRELIM_MIN_HALF_WIDTH;
      int dx = (int) ((v0.y () * detw) / l);
      int dy = (int) (- (v0.x () * detw) / l);
      inip1 = Pt2i (pcentral.x () + dx, pcentral.y () + dy);
      inip2 = Pt2i (pcentral.x () - dx, pcentral.y () - dy);
      iniwidth = 0;
    }
  }
  else
  {
    inip1.set (p1);
    inip2.set (p2);
    iniwidth = (centralp ? DEFAULT_FAST_TRACK_SCAN_WIDTH : 0);
    inipc.set (pc);
  }

  // Initial detection based on highest gradient without orientation constraint
  //---------------------------------------------------------------------------
  bsini = bst1->fastTrack (inThick + imaxMargin,
                           inip1, inip2, iniwidth, inipc);
  if (bsini == NULL || bsini->size () < bsMinSize)
    return (bsini == NULL ? RESULT_INITIAL_NO_DETECTION
                          : RESULT_INITIAL_TOO_FEW);

  // Density test
  //-------------
  if (densityTestOn)
  {
    DigitalStraightLine mydsl (inip1, inip2, DigitalStraightLine::DSL_NAIVE);
    int mydrlf = mydsl.manhattan (bsini->getLastRight ())
                 - mydsl.manhattan (bsini->getLastLeft ());
    if (mydrlf < 0) mydrlf = -mydrlf; // Case of horizontal P1P2
    int expansion = 1 + mydrlf;
    if (bsini->size () < expansion / 2)
      return RESULT_INITIAL_TOO_SPARSE;
  }

  // Filtering the initial segment
  //------------------------------
  if (prefilteringOn)
  {
    BlurredSegment *fbs = lsf1->filter (bsini);
    if (fbs != NULL)
    {
      delete bsini;
      bsini = fbs;
    }
    if (bsini->size () < bsMinSize)
      return RESULT_INITIAL_TOO_MANY_OUTLIERS;
  }

  // Orientation test for automatic extractions
  //-------------------------------------------
  Vr2i bsinidir = bsini->getSupportVector();
  if (bsinidir.orientedAs (inip1.vectorTo (inip2)))
    return RESULT_INITIAL_CLOSE_ORIENTATION;
  
  // Gradient reference selection
  //-----------------------------
  Pt2i pCenter = bsini->getCenter ();
  Vr2i gRef = gMap->getValue (pCenter);
  if (edgeDirection == -1) gRef.invert ();

  // Scan recentering and fitting
  //-----------------------------
  if (recenteringOn)
    pCenter = bsini->getSegment()->centerOfIntersection (inip1, inip2);
  int bswidth = inThick;
  if (fittingOn)
  {
    DigitalStraightSegment *dss = bsini->getSegment ();
    if (dss != NULL)
      bswidth = 1 + dss->width () / dss->period ();
  }

  // Finer detection based on gradient maxima with orientation constraint
  //---------------------------------------------------------------------
  bsf = bst2->fineTrack (bswidth, pCenter, bsinidir, 2 * bswidth, gRef);
  if (bsf == NULL || bsf->size () < bsMinSize)
    return (bsf == NULL ? RESULT_FINAL_NO_DETECTION : RESULT_FINAL_TOO_FEW);

  // Size test
  //------------
  if (finalSizeTestOn)
  {
    // DigitalStraightSegment *dss = bsf->getSegment ();
    if ((int) (bsf->getAllPoints().size ()) < finalSizeMinVal)
    {
      // nbSmallBS ++;
      return RESULT_FINAL_TOO_SMALL;
    }
  }

  // Length test
  //------------
  if (finalLengthTestOn)
  {
    DigitalStraightSegment *dss = bsf->getSegment ();
    if ((int) (bsf->getAllPoints().size ())
        < (3 * dss->width ()) / dss->period ())
    // if ((int) (bsf->getAllPoints().size ()) < 10)
      return RESULT_FINAL_TOO_SPARSE;
  }

  // New density test
  //-----------------
  if (finalDensityTestOn)
  {
    DigitalStraightLine mydsl (inip1, inip2, DigitalStraightLine::DSL_NAIVE);
    int mydrlf = mydsl.manhattan (bsf->getLastRight ())
                 - mydsl.manhattan (bsf->getLastLeft ());
    if (mydrlf < 0) mydrlf = -mydrlf; // Case of horizontal P1P2
    int expansion = 1 + mydrlf;
    if (expansion < 20 && bsf->size () < (expansion * 4) / 5)
      return RESULT_FINAL_TOO_SPARSE;
  }

  // Connected components analysis */
  //------------------------------*/
  if (ccOn)
  {
    int bsccp = bsf->countOfConnectedPoints (ccMinSize);
    int bssize = bsf->getAllPoints().size ();
    if (bsccp < bssize / 2)
      return RESULT_FINAL_TOO_SPARSE;

/*
    if (bssize < 20 || bsf->countOfConnectedComponents (bssize / 2) == 0)
      return RESULT_FINAL_TOO_SPARSE;
*/
  }


  // Final filtering
  //----------------
  if (filteringOn)
  {
    BlurredSegment *fbsf = lsf2->filter (bsf);
    if (fbsf != NULL)
    {
      delete bsf;
      bsf = fbsf;
    }
    else return RESULT_FINAL_TOO_MANY_OUTLIERS;
  }

  return RESULT_OK;
}


int BSDetector::staticDetect (const Pt2i &p1, const Pt2i &p2,
                           bool centralp, const Pt2i &pc)
{
  // Entry check
  //------------
  if (p1.equals (p2)
      || ((! centralp) && p1.chessboard (p2) < BSTracker::MIN_SCAN))
    return RESULT_VOID;

  // Clearance
  //----------
  bst1->clear ();
  bst2->clear ();
  if (bsini != NULL) delete bsini;
  bsini = NULL;
  if (bsf != NULL) delete bsf;
  bsf = NULL;

  inip1.set (p1);
  inip2.set (p2);
  iniwidth = (centralp ? DEFAULT_FAST_TRACK_SCAN_WIDTH : 0);
  inipc.set (pc);

  // Initial detection based on highest gradient without orientation constraint
  //---------------------------------------------------------------------------
  bsini = bst1->fastTrack (DEFAULT_FAST_TRACK_SCAN_WIDTH / 4,
                           inip1, inip2, iniwidth, inipc);
  if (bsini == NULL || bsini->size () < bsMinSize)
    return (bsini == NULL ? RESULT_INITIAL_NO_DETECTION
                          : RESULT_INITIAL_TOO_FEW);

  // Density test
  //-------------
/*
  if (densityTestOn)
  {
    DigitalStraightLine mydsl (inip1, inip2, DigitalStraightLine::DSL_NAIVE);
    int mydrlf = mydsl.manhattan (bsini->getLastRight ())
                 - mydsl.manhattan (bsini->getLastLeft ());
    if (mydrlf < 0) mydrlf = -mydrlf; // Case of horizontal P1P2
    int expansion = 1 + mydrlf;
    if (bsini->size () < expansion / 2)
      return RESULT_INITIAL_TOO_SPARSE;
  }
*/

  // Orientation test for automatic extractions
  //-------------------------------------------
  Vr2i bsinidir = bsini->getSupportVector();
  if (bsinidir.orientedAs (inip1.vectorTo (inip2)))
    return RESULT_INITIAL_CLOSE_ORIENTATION;

  // Gradient reference selection
  //-----------------------------
  Pt2i pCenter = bsini->getCenter ();
  Vr2i gRef = gMap->getValue (pCenter);
  if (edgeDirection == -1) gRef.invert ();

  // Scan recentering and fitting
  //-----------------------------
  if (recenteringOn)
    pCenter = bsini->getSegment()->centerOfIntersection (inip1, inip2);

  // Finer detection based on gradient maxima with orientation constraint
  //---------------------------------------------------------------------
  bsf = bstold->fineTrack (inThick, pCenter, bsinidir,
                           4 * inThick, gRef);
  if (bsf == NULL || bsf->size () < bsMinSize)
    return (bsf == NULL ? RESULT_FINAL_NO_DETECTION : RESULT_FINAL_TOO_FEW);

  // Scan recentering and fitting
  //-----------------------------
  pCenter = bsini->getCenter ();
  if (recenteringOn)
    pCenter = bsf->getSegment()->centerOfIntersection (inip1, inip2);

  // Third detection based on gradient maxima with orientation constraint
  //---------------------------------------------------------------------
  BlurredSegment *bsf2 = bstold->fineTrack (inThick,
                                            pCenter, bsf->getSupportVector(),
                                            4 * inThick, gRef);
  if (bsf2 == NULL || bsf2->size () < bsMinSize)
  {
    if (bsf2 != NULL)
    {
      delete bsf2;
      return RESULT_FINAL_TOO_FEW;
    }
    else return RESULT_FINAL_NO_DETECTION;
  }
  else
  {
    delete bsf;
    bsf = bsf2;
  }

  // Length test
  //------------
  if (finalLengthTestOn)
  {
    DigitalStraightSegment *dss = bsf->getSegment ();
    if (dss == NULL || (int) (bsf->getAllPoints().size ())
                       < (10 * dss->period ()) / dss->width ())
      return RESULT_FINAL_TOO_SPARSE;
  }

  // New density test
  //-----------------
  if (finalDensityTestOn)
  {
    DigitalStraightLine mydsl (inip1, inip2, DigitalStraightLine::DSL_NAIVE);
    int mydrlf = mydsl.manhattan (bsf->getLastRight ())
                 - mydsl.manhattan (bsf->getLastLeft ());
    if (mydrlf < 0) mydrlf = -mydrlf; // Case of horizontal P1P2
    int expansion = 1 + mydrlf;
    if (bsf->size () < expansion / 2)
      return RESULT_FINAL_TOO_SPARSE;
  }

  // Connected components analysis
  //------------------------------
  if (ccOn)
  {
    int bsccp = bsf->countOfConnectedPoints (ccMinSize);
    int bssize = bsf->getAllPoints().size ();
    if (bsccp < bssize / 2)
      return RESULT_FINAL_TOO_SPARSE;

/*
    if (bssize < 20 || bsf->countOfConnectedComponents (bssize / 4) == 0)
      return RESULT_FINAL_TOO_SPARSE;
*/
  }

  return RESULT_OK;
}


BlurredSegment *BSDetector::getBlurredSegment (int step) const
{
  if (step == STEP_PRELIM) return (bspre);
  else if (step == STEP_INITIAL) return (bsini);
  else if (mbsf.empty ()) return (bsf);
  else return (mbsf.back ());
  return NULL;
}


void BSDetector::incMaxDetections (bool dir)
{
  maxtrials = maxtrials + (dir ? -1 : 1);
  if (maxtrials < 0) maxtrials = (int) (mbsf.size ());
}


void BSDetector::getScanInput (int step,
                               Pt2i &p1, Pt2i &p2, int &swidth, Pt2i &pc) const
{
  if (step == STEP_PRELIM)
  {
    if (prelimDetectionOn)
    {
      p1.set (prep1);
      p2.set (prep2);
      swidth = prewidth;
      pc.set (prepc);
    }
  }
  else if (step == STEP_INITIAL)
  {
    p1.set (inip1);
    p2.set (inip2);
    swidth = iniwidth;
    pc.set (inipc);
  }
}


const vector <vector <Pt2i> > BSDetector::getFinalScans () const
{
  return (bst2->getScans ());
}


bool BSDetector::finalScansRecordOn () const
{
  return (bst2->scanRecordOn ());
}


void BSDetector::setFinalScansRecord (bool status)
{
  bst2->setScanRecord (status);
  if (status) redetect ();
}


void BSDetector::switchOrthoScans ()
{
  if (prelimDetectionOn) bst0->switchOrthoScans ();
  bst1->switchOrthoScans ();
  bst2->switchOrthoScans ();
}


vector<Pt2i> BSDetector::getRejected (int step) const
{
  vector<Pt2i> res;
  if (step == STEP_FINAL)
  {
    if (filteringOn) res = lsf2->getRejected ();
  }
  else if (prefilteringOn) res = lsf1->getRejected ();
  return res;
}


void BSDetector::switchFiltering (int step)
{
  if (step == STEP_FINAL)
  {
    filteringOn = ! filteringOn;
    //if (filteringOn && lsf2 == NULL) lsf2 = new LineSpaceFilter ();
    if (filteringOn && lsf2 == NULL) lsf2 = new BSFilter ();
  }
  else if (step == STEP_INITIAL)
  {
    prefilteringOn = ! prefilteringOn;
    //if (prefilteringOn && lsf1 == NULL) lsf1 = new LineSpaceFilter ();
    if (prefilteringOn && lsf1 == NULL) lsf1 = new BSFilter ();
  }
}


bool BSDetector::incConnectedComponentMinSize (bool increase)
{
  if ((! increase) && ccMinSize <= 1) return false;
  ccMinSize += (increase ? 1 : -1);
  return true;
}


void BSDetector::switchPreliminary ()
{
  if (prelimDetectionOn)
  {
    delete bst0;
    prelimDetectionOn = false;
  }
  else
  {
    prelimDetectionOn = true;
    bst0 = new BSTracker ();
    bst0->setGradientMap (gMap);
    if (bst1->orthoScansOn ()) bst0->switchOrthoScans ();
  }
}


std::string BSDetector::version ()
{
  return BSDetector::VERSION;
}
