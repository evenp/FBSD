#include "bsdetector.h"
//#include "linespacefilter.h"


const std::string BSDetector::VERSION = "0.2.2";
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
const int BSDetector::RESULT_FINAL_TOO_FRAGMENTED = 25;
const int BSDetector::RESULT_FINAL_TOO_MANY_OUTLIERS = 26;

const int BSDetector::DEFAULT_FAST_TRACK_SCAN_WIDTH = 16;
const int BSDetector::DEFAULT_ASSIGNED_THICKNESS = 3;
const int BSDetector::FAST_TRACK_MARGIN = 2;

const int BSDetector::BS_MIN_SIZE = 3;
const int BSDetector::DEFAULT_INITIAL_MIN_SIZE = 5;
const int BSDetector::DEFAULT_FINAL_MIN_SIZE = 10;
const int BSDetector::DEFAULT_FRAGMENT_MIN_SIZE = 5;
const int BSDetector::DEFAULT_AUTO_SWEEPING_STEP = 15;
const int BSDetector::PRELIM_MIN_HALF_WIDTH = 10;


BSDetector::BSDetector ()
{
  gMap = NULL;
  inThick = DEFAULT_ASSIGNED_THICKNESS;
  prelimDetectionOn = false;
  staticDetOn = false;
  singleMultiOn = true;

  bst0 = (prelimDetectionOn ? new BSTracker () : NULL);
  bst1 = new BSTracker ();
  // bst1->setPixelLackTolerence (bst1->getVicinityThreshold ());
  bst2 = new BSTracker ();
  bstStatic = NULL;
  if (staticDetOn)
  {
    bstStatic = new BSTracker ();
    if (bstStatic->dynamicScansOn ()) bstStatic->toggleDynamicScans ();
    if (bstStatic->isAssignedThicknessControlOn ())
      bstStatic->toggleAssignedThicknessControl ();
  }

  prefilteringOn = false;
  //lsf1 = (prefilteringOn ? new LineSpaceFilter () : NULL);
  lsf1 = (prefilteringOn ? new BSFilter () : NULL);
  filteringOn = false;
  //lsf2 = (filteringOn ? new LineSpaceFilter () : NULL);
  lsf2 = (filteringOn ? new BSFilter () : NULL);

  oppositeGradientDir = false;   // main edge detection
  initialMinSize = DEFAULT_INITIAL_MIN_SIZE;
  finalFragmentationTestOn = false;
  fragmentMinSize = DEFAULT_FRAGMENT_MIN_SIZE;
  recenteringOn = true;
  initialSparsityTestOn = true;
  finalSizeTestOn = true;
  finalMinSize = DEFAULT_FINAL_MIN_SIZE;
  finalSparsityTestOn = false;
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
  if (staticDetOn) delete bstStatic;
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
  if (staticDetOn) bstStatic->setGradientMap (data);
  if (bst1) bst1->setGradientMap (data);
  if (bst2) bst2->setGradientMap (data);
  if (bstStatic) bstStatic->setGradientMap (data);
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
  for (int x = width / 2 + autoSweepingStep;
       isnext && x < width - 1; x += autoSweepingStep)
    isnext = runMultiDetection (Pt2i (x, 0), Pt2i (x, height - 1));
  for (int y = height / 2; isnext && y > 0; y -= autoSweepingStep)
    isnext = runMultiDetection (Pt2i (0, y), Pt2i (width - 1, y));
  for (int y = height / 2 + autoSweepingStep;
       isnext && y < height - 1; y += autoSweepingStep)
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
  int *locmax = new int[pts.size ()];
  int nlm = gMap->localMax (locmax, pts);
  bool isnext = true;
  for (int i = 0; isnext && i < nlm; i++)
  {
    Pt2i ptstart = pts.at (locmax[i]);
    if (gMap->isFree (ptstart))
    {
      bool savedOppDir = oppositeGradientDir;
      oppositeGradientDir = false;
      int nbDets = (gMap->isOrientationConstraintOn () ? 2 : 1);
      if (singleMultiOn) nbDets = 1;
      while (isnext && nbDets != 0)
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
        oppositeGradientDir = ! oppositeGradientDir;
        nbDets --;
        nbtrials ++;
      }
      oppositeGradientDir = savedOppDir;
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
    bspre = bst0->fastTrack (inThick + FAST_TRACK_MARGIN,
                             prep1, prep2, prewidth, prepc);
    if (bspre == NULL || bspre->size () < initialMinSize)
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
  bsini = bst1->fastTrack (inThick + FAST_TRACK_MARGIN,
                           inip1, inip2, iniwidth, inipc);
  if (bsini == NULL || bsini->size () < initialMinSize)
    return (bsini == NULL ? RESULT_INITIAL_NO_DETECTION
                          : RESULT_INITIAL_TOO_FEW);

  // Sparsity test
  //-------------
  if (initialSparsityTestOn)
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
    if (bsini->size () < initialMinSize)
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
  if (oppositeGradientDir && gMap->isOrientationConstraintOn ())
    gRef.invert ();

  // Scan recentering and fitting
  //-----------------------------
  if (recenteringOn)
    pCenter = bsini->getSegment()->centerOfIntersection (inip1, inip2);

  // Finer detection based on gradient maxima with orientation constraint
  //---------------------------------------------------------------------
  bsf = bst2->fineTrack (inThick, pCenter, bsinidir, 2 * inThick, gRef);
  if (bsf == NULL || bsf->size () < initialMinSize)
    return (bsf == NULL ? RESULT_FINAL_NO_DETECTION : RESULT_FINAL_TOO_FEW);

  // Size test
  //------------
  if (finalSizeTestOn)
  {
    // DigitalStraightSegment *dss = bsf->getSegment ();
    if ((int) (bsf->getAllPoints().size ()) < finalMinSize)
    {
      // nbSmallBS ++;
      return RESULT_FINAL_TOO_SMALL;
    }
  }

  // Sparsity test
  //--------------
  if (finalSparsityTestOn)
  {
    Pt2i pOrtho (pCenter.x () + bsinidir.x (), pCenter.y () - bsinidir.y ());
    DigitalStraightLine mydsl (pCenter, pOrtho, DigitalStraightLine::DSL_NAIVE);
    int mydrlf = mydsl.manhattan (bsf->getLastRight ())
                 - mydsl.manhattan (bsf->getLastLeft ());
    if (mydrlf < 0) mydrlf = -mydrlf; // Case of horizontal P1P2
    int expansion = 1 + mydrlf;
    if (expansion < 20 && (int) (bsf->size ()) < (expansion * 4) / 5)
      return RESULT_FINAL_TOO_SPARSE;
  }

  // Fragmentation test */
  //--------------------*/
  if (finalFragmentationTestOn)
  {
    int bsccp = bsf->countOfConnectedPoints (fragmentMinSize);
    int bssize = (int) (bsf->getAllPoints().size ());
    if (bsccp < bssize / 2) return RESULT_FINAL_TOO_FRAGMENTED;
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
  bstStatic->clear ();
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
  if (bsini == NULL || bsini->size () < initialMinSize)
    return (bsini == NULL ? RESULT_INITIAL_NO_DETECTION
                          : RESULT_INITIAL_TOO_FEW);

  // Sparsity test
  //-------------
/*
  if (initialSparsityTestOn)
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
  if (oppositeGradientDir && gMap->isOrientationConstraintOn ())
    gRef.invert ();

  // Scan recentering and fitting
  //-----------------------------
  if (recenteringOn)
    pCenter = bsini->getSegment()->centerOfIntersection (inip1, inip2);

  // Finer detection based on gradient maxima with orientation constraint
  //---------------------------------------------------------------------
  bsf = bstStatic->fineTrack (inThick, pCenter, bsinidir,
                              4 * inThick, gRef);
  if (bsf == NULL || bsf->size () < initialMinSize)
    return (bsf == NULL ? RESULT_FINAL_NO_DETECTION : RESULT_FINAL_TOO_FEW);

  // Scan recentering and fitting
  //-----------------------------
  pCenter = bsini->getCenter ();
  if (recenteringOn)
    pCenter = bsf->getSegment()->centerOfIntersection (inip1, inip2);

  // Third detection based on gradient maxima with orientation constraint
  //---------------------------------------------------------------------
  BlurredSegment *bsf2 = bstStatic->fineTrack (inThick,
                                               pCenter, bsf->getSupportVector(),
                                               4 * inThick, gRef);
  if (bsf2 == NULL || bsf2->size () < initialMinSize)
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

  // Size test
  //------------
  if (finalSizeTestOn)
  {
    // DigitalStraightSegment *dss = bsf->getSegment ();
    if ((int) (bsf->getAllPoints().size ()) < finalMinSize)
    {
      // nbSmallBS ++;
      return RESULT_FINAL_TOO_SMALL;
    }
  }

  // Final sparsity test
  //-----------------
  if (finalSparsityTestOn)
  {
    Pt2i pOrtho (pCenter.x () + bsinidir.y (), pCenter.y () - bsinidir.y ());
    DigitalStraightLine mydsl (pCenter, pOrtho, DigitalStraightLine::DSL_NAIVE);
    int mydrlf = mydsl.manhattan (bsf->getLastRight ())
                 - mydsl.manhattan (bsf->getLastLeft ());
    if (mydrlf < 0) mydrlf = -mydrlf; // Case of horizontal P1P2
    int expansion = 1 + mydrlf;
    if ((int) (bsf->size ()) < expansion / 2)
      return RESULT_FINAL_TOO_SPARSE;
  }

  // Connected components analysis
  //------------------------------
  if (finalFragmentationTestOn)
  {
    int bsccp = bsf->countOfConnectedPoints (fragmentMinSize);
    int bssize = (int) (bsf->getAllPoints().size ());
    if (bsccp < bssize / 2) return RESULT_FINAL_TOO_FRAGMENTED;
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
  if (staticDetOn) bstStatic->switchOrthoScans ();
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


bool BSDetector::incFragmentSizeMinValue (bool increase)
{
  if ((! increase) && fragmentMinSize <= 1) return false;
  fragmentMinSize += (increase ? 1 : -1);
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


bool BSDetector::switchOppositeGradient ()
{
  if (gMap != NULL && gMap->isOrientationConstraintOn ())
  {
    oppositeGradientDir = ! oppositeGradientDir;
    return true;
  }
  return false;
}


void BSDetector::setStaticDetector (bool status)
{
  if (staticDetOn && ! status)
  {
    delete bstStatic;
    staticDetOn = false;
  }
  else if (status && ! staticDetOn)
  {
    bstStatic = new BSTracker ();
    if (gMap) bstStatic->setGradientMap (gMap);
    if (bstStatic->dynamicScansOn ()) bstStatic->toggleDynamicScans ();
    if (bstStatic->isAssignedThicknessControlOn ())
      bstStatic->toggleAssignedThicknessControl ();
    staticDetOn = true;
  }
}


std::string BSDetector::version ()
{
  return BSDetector::VERSION;
}
