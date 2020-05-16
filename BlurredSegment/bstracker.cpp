#include "bstracker.h"
#include "blurredsegmentproto.h"



const int BSTracker::DEFAULT_ACCEPTED_LACKS = 5;
const int BSTracker::DEFAULT_PROX_THRESHOLD = 10;
const int BSTracker::MIN_SCAN = 8;
const int BSTracker::DEFAULT_MAX_SCAN = 32;
const int BSTracker::DEFAULT_FITTING_DELAY = 20;

const int BSTracker::DEFAULT_ASSIGNED_THICKNESS_CONTROL_DELAY = 20;

const int BSTracker::FAILURE_NO_START = 1;
const int BSTracker::FAILURE_IMAGE_BOUND_ON_RIGHT = 2;
const int BSTracker::FAILURE_IMAGE_BOUND_ON_LEFT = 4;
const int BSTracker::FAILURE_LOST_ORIENTATION = 32;



BSTracker::BSTracker ()
{
  proxTestOff = true;
  proxThreshold = DEFAULT_PROX_THRESHOLD;
  acceptedLacks = DEFAULT_ACCEPTED_LACKS;
  minRestart = acceptedLacks;

  dynamicScans = true;
  fittingDelay = DEFAULT_FITTING_DELAY;
  recordScans = false;
  orthoScan = false;
  trackCrosswise = false;

  assignedThicknessControlOn = true;
  assignedThicknessControlDelay = DEFAULT_ASSIGNED_THICKNESS_CONTROL_DELAY;

  gMap = NULL;

  maxScan = DEFAULT_MAX_SCAN;

  fail = 0;

  cand = new int[1]; // to avoid systematic tests
  lscan = 0;
  rscan = 0;
}


BSTracker::~BSTracker ()
{
  delete cand;
}


void BSTracker::clear ()
{
  scanBound1.clear ();
  scanBound2.clear ();
  scanLine.clear ();
}


void BSTracker::setGradientMap (VMap *data)
{
  gMap = data;
  scanp.setSize (gMap->getWidth (), gMap->getHeight ());
  delete cand;
  cand = new int[data->getHeightWidthMax ()];
}


BlurredSegment *BSTracker::fastTrack (int bsMaxWidth,
                                      const Pt2i &p1, const Pt2i &p2,
                                      int swidth, const Pt2i &pc)
{
  // Creates the scanner
  DirectionalScanner *ds = NULL;
  if (swidth != 0)
  {
    if (swidth < MIN_SCAN) swidth = MIN_SCAN;
    ds = scanp.getScanner (pc, p1.vectorTo (p2), swidth, false);
  }
  else ds = scanp.getScanner (p1, p2);
  if (ds == NULL) return NULL;

  // Builds a BS builder around a central point
  vector<Pt2i> pix;
  if (ds->first (pix) < MIN_SCAN)
  {
    delete ds;
    return NULL;
  }

  if (recordScans)
  {
    scanBound1.push_back (pix.front ());
    scanBound2.push_back (pix.back ());
    scanLine.push_back (pix);
  }
  int candide;
  Pt2i pfirst;
  if (swidth != 0) pfirst.set (pc.x (), pc.y ());
  else
  {
    candide = gMap->largestIn (pix);
    if (candide == -1)
    {
      delete ds;
      return NULL;
    }
    pfirst.set (pix.at (candide));
  }

  BlurredSegmentProto bs (bsMaxWidth, pfirst);
  Pt2i lastLeft (pfirst);
  Pt2i lastRight (pfirst);
  
  // Extends the segment
  int lstop = 0;
  int rstop = 0;
  bool added = false;
  bool scanningRight = true;
  bool scanningLeft = true;
  int fsCount = maxScan;

  while ((scanningRight || scanningLeft) && (fsCount--))
  {
    // Extends on right
    if (scanningRight)
    {
      if (ds->nextOnRight (pix) < MIN_SCAN) scanningRight = false;
      else
      {
        if (recordScans)
        {
          scanBound1.push_back (pix.front ());
          scanBound2.push_back (pix.back ());
          scanLine.push_back (pix);
        }
        added = false;
        candide = gMap->largestIn (pix);
        if (candide != -1)
        {
          if (proxTestOff
              || lastRight.manhattan (pix.at (candide)) <= proxThreshold)
            added = bs.addRight (pix.at (candide));
        }
        if (added)
        {
          lastRight = pix.at (candide);
          if (rstop != 0) rstop = 0;
        }
        else if (++rstop > acceptedLacks)
          scanningRight = false;
      }
    }
    // Extends on left
    if (scanningLeft)
    {
      if (ds->nextOnLeft (pix) < MIN_SCAN) scanningLeft = false;
      else
      {
        if (recordScans)
        {
          scanBound1.push_back (pix.front ());
          scanBound2.push_back (pix.back ());
          scanLine.push_back (pix);
        }
        added = false;
        candide = gMap->largestIn (pix);
        if (candide != -1)
        {
          if (proxTestOff
              || lastLeft.manhattan (pix.at (candide)) <= proxThreshold)
            added = bs.addLeft (pix.at (candide));
        }
        if (added)
        {
          lastLeft = pix.at (candide);
          if (lstop != 0) lstop = 0;
        }
        else if (++lstop > acceptedLacks)
          scanningLeft = false;
      }
    }
  }
  delete ds;
  return (bs.endOfBirth ());
}



BlurredSegment *BSTracker::fineTrack (int bsMaxWidth,
                                      const Pt2i &center, const Vr2i &scandir,
                                      int scanwidth, const Vr2i &gref)
{
  // Checks scan width minimal size
  if (scanwidth < MIN_SCAN) scanwidth = MIN_SCAN;

  // Gets detected segment normal vector
  Vr2i normal = scandir.orthog ();
  if (! normal.directedAs (gref)) normal.invert ();

  fail = 0;

  // Creation of the directional scanner and the array of candidates
  DirectionalScanner *ds = scanp.getScanner (center, normal,
                                             scanwidth, dynamicScans);
  if (ds == NULL)
  {
    fail = FAILURE_NO_START;
    return NULL;
  }

  // Looking for a central point
  vector<Pt2i> pix;
  if (ds->first (pix) < MIN_SCAN)
  {
    delete ds;
    fail = FAILURE_NO_START;
    return NULL;
  }
  if (recordScans)
  {
    scanBound1.push_back (pix.front ());
    scanBound2.push_back (pix.back ());
    scanLine.push_back (pix);
  }
  int nbc = gMap->localMax (cand, pix, normal);
  if (nbc == 0)
  {
    delete ds;
    fail = FAILURE_NO_START;
    return NULL;
  }

  BlurredSegmentProto bs (bsMaxWidth, pix[cand[0]]);

  // Handles assigned thickness control
  bool atcOn = assignedThicknessControlOn;
  int stableWidthCount = 0;
  int count = 0;

  // Extends the segment
  lscan = 0;
  rscan = 0;
  int lstop = 0;
  int rstop = 0;
  int lstart = 0;
  int rstart = 0;
  bool added = false;
  bool scanningRight = true;
  bool scanningLeft = true;

  while (scanningRight || scanningLeft)
  {
    count ++;
    AbsRat sw = bs.strictThickness ();

    // Handles assigned thickness control
    if (atcOn && stableWidthCount >= assignedThicknessControlDelay)
    {
      AbsRat finalWidth (bs.digitalThickness().sumHalf ());
      if (finalWidth.lessThan (bs.getMaxWidth ())) bs.setMaxWidth (finalWidth);
      atcOn = false;
    }

    // Resets the scan stripe
    if (dynamicScans && count > fittingDelay && bs.isLineable ())
    {
      // Stops the detection if the segment gets crosswise
      if (count == fittingDelay + 1)
      {
        Vr2i dirn = bs.getSupportVector ();
        if (4 * dirn.squaredScalarProduct (scandir)
            < 3 * dirn.norm2 () * scandir.norm2 ())
        {
          scanningLeft = false;
          scanningRight = false;
          fail += FAILURE_LOST_ORIENTATION;
        }
      }
      int ppa, ppb, ppc;
      bs.getLine()->getMedialAxis (ppa, ppb, ppc);
      ds->bindTo (ppa, ppb, ppc);
    }
    else if (trackCrosswise && count > 3 && bs.isLineable ())
    {
      Vr2i dirn = bs.getSupportVector ();
      if (4 * dirn.squaredScalarProduct (scandir)
          < 3 * dirn.norm2 () * scandir.norm2 ())
      {
        scanningLeft = false;
        scanningRight = false;
        fail += FAILURE_LOST_ORIENTATION;
      }
    }

    // Extends on right
    if (scanningRight)
    {
      added = false;
      if (ds->nextOnRight (pix) < MIN_SCAN)
      {
        fail += FAILURE_IMAGE_BOUND_ON_RIGHT;
        scanningRight = false;
      }
      else
      {
        if (recordScans)
        {
          scanBound1.push_back (pix.front ());
          scanBound2.push_back (pix.back ());
          scanLine.push_back (pix);
        }
        added = false;
        nbc = gMap->localMax (cand, pix, normal);
        for (int i = 0; ! added && i < nbc; i++)
          added = bs.addRight (pix[cand[i]]);
        stableWidthCount ++;
        if (added)
        {
          if (atcOn
              && sw.lessThan (bs.strictThickness ())) stableWidthCount = 0;
          rscan = count;
          if (rstop == 0) rstart = 0;
          else
          {
            rstart ++;
            //if (rstart >= minRestart || rstart >= rstop)
            if (rstart >= rstop)
            {
              rstop = 0;
              rstart = 0;
            }
          }
        }
        else
        {
          if (++rstop - rstart > acceptedLacks)
          {
            if (bs.size () <= 3) fail = FAILURE_NO_START;
            scanningRight = false;
          }
        }
      }
    }

    // Extends on left
    if (scanningLeft)
    {
      if (ds->nextOnLeft (pix) < MIN_SCAN)
      {
        fail += FAILURE_IMAGE_BOUND_ON_LEFT;
        scanningLeft = false;
      }
      else
      {
        if (recordScans)
        {
          scanBound1.push_back (pix.front ());
          scanBound2.push_back (pix.back ());
          scanLine.push_back (pix);
        }
        added = false;
        nbc = gMap->localMax (cand, pix, normal);
        for (int i = 0; ! added && i < nbc; i++)
          added = bs.addLeft (pix[cand[i]]);
        stableWidthCount ++;
        if (added)
        {
          if (atcOn
              && sw.lessThan (bs.strictThickness ())) stableWidthCount = 0;
          lscan = count;
          if (lstop == 0) lstart = 0;
          else
          {
            lstart ++;
            //if (lstart >= minRestart || lstart >= lstop)
            if (lstart >= lstop)
            {
              lstop = 0;
              lstart = 0;
            }
          }
        }
        else
        {
          if (++lstop - lstart > acceptedLacks)
          {
            if (bs.size () <= 3) fail = FAILURE_NO_START;
            scanningLeft = false;
          }
        }
      }
    }
  }
  if (rstart) bs.removeRight (rstart);
  if (lstart) bs.removeLeft (lstart);
  delete ds;
  return (bs.endOfBirth ());
}


void BSTracker::switchOrthoScans ()
{
  orthoScan = ! orthoScan;
  scanp.setOrtho (orthoScan);
}


void BSTracker::switchScanExtent ()
{
  maxScan = (maxScan == gMap->getHeightWidthMax () ?
             DEFAULT_MAX_SCAN : gMap->getHeightWidthMax ());
}
