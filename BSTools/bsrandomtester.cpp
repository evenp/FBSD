#include <QtGui>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include "bsrandomtester.h"

using namespace std;



BSRandomTester::BSRandomTester ()
{
  swidth = 256;
  sheight = 256;
  margin = 10;
  width = swidth + 2 * margin;
  height = sheight + 2 * margin;
  isize = width * height;

  nbdets = 2;
  nbruns = 1000;
  nbsegs = 10;
  sminwidth = 2;
  smaxwidth = 5;
  swmargin = 2;
  sminlength = 20;
  longEdgeThreshold = 1600;

  unbiasOn = true;
  biasVal.set (7, 5);

  dispEach = false;
  dispLast = false;

  rp1 = new Pt2i[nbsegs];
  rp2 = new Pt2i[nbsegs];
  rdir = new Vr2i[nbsegs];
  rw = new int[nbsegs];
  tofind_map = new bool[isize];
  hit_map = new int[isize];
  stilltofind_map = new bool[isize];
  found_map = new bool[isize];
  foundin_map = new bool[isize];
  foundout_map = new bool[isize];

  nbIniPts = new int[nbruns];
  int nbt = nbdets * nbruns;

  c_trials = new int[nbt];
  c_det = new int[nbt];
  c_ldet = new int[nbt];
  c_unmatch = new int[nbt];
  c_undet = new int[nbt];
  c_true = new int[nbt];
  c_false = new int[nbt];
  c_redet = new int[nbt];

  m_precision = new double[nbt];
  m_recall = new double[nbt];
  m_fmeasure = new double[nbt];
  m_matched = new double[nbt];
  m_biased_width = new double[nbt];
  m_width = new double[nbt];
  m_wdiff = new double[nbt];
  m_abswdiff = new double[nbt];
  m_adiff = new double[nbt];
  m_absadiff = new double[nbt];
  m_long_absadiff = new double[nbt];

  gMap = NULL;
  detectors = new BSDetector[nbdets];
  for (int i = 0; i < nbdets; i++)
  {
    detectors[i].setAssignedThickness (smaxwidth + swmargin);
    if (! detectors[i].isFinalSizeTestOn ())
      detectors[i].switchFinalSizeTest ();
  }
  detectors[0].setStaticDetector (true);
  detectors[1].setStaticDetector (false);
  names = new QString[nbdets];
  names[0] = "old";
  names[1] = "new";

  image = QImage (width, height, QImage::Format_RGB32);
  srand (time (NULL));
}


BSRandomTester::~BSRandomTester ()
{
  if (gMap != NULL) delete gMap;
  delete [] names;
  delete [] detectors;
  delete [] m_long_absadiff;
  delete [] m_absadiff;
  delete [] m_adiff;
  delete [] m_abswdiff;
  delete [] m_wdiff;
  delete [] m_width;
  delete [] m_biased_width;
  delete [] m_matched;
  delete [] m_fmeasure;
  delete [] m_recall;
  delete [] m_precision;
  delete [] c_redet;
  delete [] c_false;
  delete [] c_true;
  delete [] c_undet;
  delete [] c_unmatch;
  delete [] c_ldet;
  delete [] c_det;
  delete [] c_trials;
  delete [] nbIniPts;
  delete [] foundout_map;
  delete [] foundin_map;
  delete [] found_map;
  delete [] stilltofind_map;
  delete [] hit_map;
  delete [] tofind_map;
  delete [] rw;
  delete [] rdir;
  delete [] rp2;
  delete [] rp1;
}


void BSRandomTester::randomTest ()
{
  // List of detected segments match to each input segment
  vector<BlurredSegment *> rbs[nbsegs];

  cout << "Testing ..." << endl;
  for (int run = 0; run < nbruns; run ++)
  {
    // Generates an image and provide it to the detectors
    if (dispEach) cout << "Generating new segments" << endl;
    generateImage ();
    if (gMap != NULL) delete gMap;
    gMap = new VMap (width, height, getBitmap (image), VMap::TYPE_SOBEL_5X5);
    gMap->incGradientThreshold (50 - gMap->getGradientThreshold ());
    for (int det = 0; det < nbdets; det ++)
    {
      detectors[det].setGradientMap (gMap);
      if (detectors[det].isSingleEdgeModeOn ())
        detectors[det].switchSingleOrDoubleEdge ();
    }

    for (int det = 0; det < nbdets; det ++)
    {
      int num = det * nbruns + run;

      // Detects the segments
      if (dispEach) cout << "Running detector " << (det + 1) << endl;
      detectors[det].detectAll ();

      nbIniPts[run] = 0;
      for (int i = 0; i < isize; i++)
      {
        stilltofind_map[i] = tofind_map[i];
        found_map[i] = false;
        foundin_map[i] = false;
        foundout_map[i] = false;
        if (tofind_map[i]) nbIniPts[run] ++;
        hit_map[i] = 0;
      }

      if (dispEach) cout << "Analyzing the blurred segments" << endl;
      for (int i = 0; i < nbsegs; i++) rbs[i].clear ();
      c_unmatch[num] = 0;
      int nbdssnul = 0;
      c_ldet[num] = 0;
      double nomatchlength = 0.;
      vector<BlurredSegment *> bss = detectors[det].getBlurredSegments ();
      vector<BlurredSegment *>::iterator bsit = bss.begin ();
      while (bsit != bss.end ())
      {
        DigitalStraightSegment *dss = (*bsit)->getSegment ();
        if (dss != NULL)
        {
          // Fills in occupancy maps
          if (unbiasOn) dss = dss->erosion (biasVal.numerator (),
                                            biasVal.denominator ());
          vector<Pt2i> dsspts;
          dss->getPoints (dsspts);
          vector<Pt2i>::iterator dssit = dsspts.begin ();
          while (dssit != dsspts.end ())
          {
            Pt2i dsspt = *dssit++;
            if (dsspt.x () >= 0 && dsspt.x () < width
                && dsspt.y () >= 0 && dsspt.y () < height)
            {
              stilltofind_map[dsspt.y () * width + dsspt.x ()] = false;
              found_map[dsspt.y () * width + dsspt.x ()] = true;
              if (tofind_map[dsspt.y () * width + dsspt.x ()])
              {
                hit_map[dsspt.y () * width + dsspt.x ()] ++;
                foundin_map[dsspt.y () * width + dsspt.x ()] = true;
              }
              else
              {
                hit_map[dsspt.y () * width + dsspt.x ()] --;
                foundout_map[dsspt.y () * width + dsspt.x ()] = true;
              }
            }
          }

          // Matches the detected segment with the nearest input segment
          Vr2i dssdir = dss->supportVector ();
          Pt2i bsc = (*bsit)->getMiddle ();
          double bsl2 = (*bsit)->getSquarredLength ();
          if (bsl2 > longEdgeThreshold) c_ldet[num] ++;
          double score[nbsegs];
          double minscore = 0.;
          int bestfit = -1;
          for (int si = 0; si < nbsegs; si++)
          {
            double denom = rdir[si].norm2 () * dssdir.norm2 ();
            score[si] = rdir[si].squaredScalarProduct (dssdir) / denom;
            Vr2i bsca = rp1[si].vectorTo (bsc);
            Vr2i bscb = bsc.vectorTo (rp2[si]);
            Vr2i *bsac = (rp1[si].chessboard (bsc) < sminlength / 2 ?
                          &bscb : &bsca);
            denom = rdir[si].norm2 () * bsac->norm2 ();
            score[si] *= rdir[si].squaredScalarProduct (*bsac) / denom;
            if (rdir[si].scalarProduct (bsca) < 0) score[si] = 0.;
            if (rdir[si].scalarProduct (bscb) < 0) score[si] = 0.;
            if (minscore < score[si])
            {
              minscore = score[si];
              bestfit = si;
            }
          }
          if (minscore > 0.7) rbs[bestfit].push_back (*bsit);
          else
          {
            c_unmatch[num] ++;
            nomatchlength += sqrt (bsl2);
          }
          if (unbiasOn) delete dss;
        }
        else nbdssnul ++;
        bsit ++;
      }

      if (dispEach) cout << "  DETECTOR "
                         << names[det].toStdString () << " :" << endl;
      c_trials[num] = detectors[det].countOfTrials ();
      c_det[num] = (int) (detectors[det].getBlurredSegments().size ());
      if (dispEach)
      {
        cout << c_det[num] << " blurred segments detected on "
             << c_trials[num] << " trials " << endl;
        cout << c_ldet[num] << " long blurred segments detected on "
             << c_trials[num] << " trials " << endl;
      }
      if (dispLast && run == nbruns - 1) createMap (names[det]);
      c_undet[num] = 0;
      c_true[num] = 0;
      c_false[num] = 0;
      for (int i = 0; i < width * height; i++)
      {
        if (stilltofind_map[i]) c_undet[num] ++;
        if (foundin_map[i]) c_true[num] ++;
        if (foundout_map[i]) c_false[num] ++;
      }
      if (dispEach)
        cout << (nbIniPts[run] - c_undet[num]) << " points detected on "
             << nbIniPts[run] << " ("
             << (nbIniPts[run] - c_undet[num]) * 100 / (double) nbIniPts[run]
             << " %)" << endl;
      c_redet[num] = 0;
      for (int i = 0; i < width * height; i++)
        if (hit_map[i] > 1) c_redet[num] += (hit_map[i] - 1);
      if (dispEach)
        cout << c_redet[num] << " points redetected on " << nbIniPts[run]
             << " (" << c_redet[num] * 100 / (double) nbIniPts[run]
             << " %)" << endl;
      m_precision[num] = nbIniPts[run] - c_undet[num];
      m_recall[num] = m_precision[num] / nbIniPts[run];
      if (m_precision[num] + c_false[num] != 0)
      {
        m_precision[num] = m_precision[num] / (m_precision[num] + c_false[num]);
        m_fmeasure[num] = 2 * m_precision[num] * m_recall[num]
                          / (m_precision[num] + m_recall[num]);
      }
      else
      {
        m_precision[num] = 0.;
        m_fmeasure[num] = 0.;
      }
      if (dispEach)
      {
        cout << c_false[num] << " false points detected on " << nbIniPts[run]
             << " (" << c_false[num] * 100 / (double) nbIniPts[run]
             << " %)" << endl;
        cout << "Precision : " << m_precision[num] << endl;
        cout << "Recall : " << m_recall[num] << endl;
        cout << "F-measure : " << m_fmeasure[num] << endl;
        cout << c_unmatch[num] << " unmatched blurred segment (mean length : "
             << (c_unmatch[num] != 0 ? nomatchlength / c_unmatch[num] : 0)
             << ")" << endl;
        cout << nbdssnul << " DSS nuls" << endl;
      }

      m_biased_width[num] = 0.;
      m_width[num] = 0.;
      m_wdiff[num] = 0.;
      m_abswdiff[num] = 0.;
      m_adiff[num] = 0.;
      m_absadiff[num] = 0.;
      m_long_absadiff[num] = 0.;
      m_matched[num] = 0;
      for (int si = 0; si < nbsegs; si ++)
      {
        // Compares input and detected segments
        if (! rbs[si].empty ()) m_matched[num] ++;
        vector<BlurredSegment *>::iterator sit = rbs[si].begin ();
        while (sit != rbs[si].end ())
        {
          double bsl2 = (*sit)->getSquarredLength ();
          DigitalStraightSegment *mydss = (*sit)->getSegment ();
          m_biased_width[num] += (mydss->width () / (double) (mydss->period ()))
                                 * sqrt (bsl2) / sqrt (rdir[si].norm2 ());
          if (unbiasOn) mydss = mydss->erosion (biasVal.numerator (),
                                                biasVal.denominator ());
          m_width[num] += (mydss->width () / (double) (mydss->period ()))
                           * sqrt (bsl2) / sqrt (rdir[si].norm2 ());
          double wd = (mydss->width () / (double) (mydss->period ()) - rw[si])
                      * sqrt (bsl2) / sqrt (rdir[si].norm2 ());
          m_wdiff[num] += wd;
          if (wd < 0) wd = -wd;
          m_abswdiff[num] += wd;
          Vr2i mydir = mydss->supportVector ();
          double ang = rdir[si].scalarProduct (mydir);
          bool onleft = rdir[si].leftside (mydir);
          if (ang < 0.)
          {
            ang = - ang;
            onleft = - onleft;
          }
          double den = sqrt (rdir[si].norm2 ()) * sqrt (mydir.norm2 ());
          if (den > ang)
          {
            ang = acos (ang / den) * 180 / M_PI;
            ang *= sqrt (bsl2) / sqrt (rdir[si].norm2 ());
            m_absadiff[num] += ang;
            m_adiff[num] += (onleft ? ang : -ang);
            if (bsl2 > longEdgeThreshold) m_long_absadiff[num] += ang;
          }
          if (unbiasOn) delete mydss;
          sit ++;
        }
      }
      if (dispEach)
      {
        cout << "Biased width = "
           << (m_matched[num] != 0 ?
               m_biased_width[num] / m_matched[num] : 0) << endl;
        cout << "Width = "
             << (m_matched[num] != 0 ?
                 m_width[num] / m_matched[num] : 0) << endl;
        cout << "Width difference = "
             << (m_matched[num] != 0 ?
                 m_wdiff[num] / m_matched[num] : 0) << endl;
        cout << "Absolute width difference = "
             << (m_matched[num] != 0 ?
                 m_abswdiff[num] / m_matched[num] : 0) << endl;
        cout << "Angle difference = "
             << (m_matched[num] != 0 ?
                 m_adiff[num] / m_matched[num] : 0) << endl;
        cout << "Absolute angle difference = "
             << (m_matched[num] != 0 ?
                 m_absadiff[num] / m_matched[num] : 0) << endl;
        cout << "Long edge angle difference = "
             << (m_matched[num] != 0 ?
                 m_long_absadiff[num] / m_matched[num] : 0) << endl;
      }
    }
  }

  double total = 0., mean = 0., sdev = 0.;
  double total_nbIniPts = 0;
  for (int i = 0; i < nbruns; i++) total_nbIniPts += nbIniPts[i];

  for (int det = 0; det < nbdets; det ++)
  {
    cout << "  RESULTS FOR THE DETECTOR " << names[det].toStdString () << endl;
    for (int i = 0; i < nbruns; i++) mean += c_trials[det * nbruns + i];
    mean /= nbruns;
    for (int i = 0; i < nbruns; i++)
      sdev += (c_trials[det * nbruns + i] - mean)
              * (c_trials[det * nbruns + i] - mean);
    sdev = sqrt (sdev / (nbruns - 1));
    cout << mean << " (pm " << sdev
         << ") segments searches (local min) / image" << endl;

    mean = 0.;
    sdev = 0.;
    for (int i = 0; i < nbruns; i++) mean += c_det[det * nbruns + i];
    mean /= nbruns;
    for (int i = 0; i < nbruns; i++)
      sdev += (c_det[det * nbruns + i] - mean)
              * (c_det[det * nbruns + i] - mean);
    sdev = sqrt (sdev / (nbruns - 1));
    cout << mean << " (pm " << sdev
         << ") provided segments / image" << endl;

    mean = 0.;
    sdev = 0.;
    for (int i = 0; i < nbruns; i++) mean += c_ldet[det * nbruns + i];
    mean /= nbruns;
    for (int i = 0; i < nbruns; i++)
      sdev += (c_ldet[det * nbruns + i] - mean)
              * (c_ldet[det * nbruns + i] - mean);
    sdev = sqrt (sdev / (nbruns - 1));
    cout << mean << " (pm " << sdev
         << ") provided long segments / image" << endl;

    mean = 0.;
    sdev = 0.;
    for (int i = 0; i < nbruns; i++) mean += c_unmatch[det * nbruns + i];
    mean /= nbruns;
    for (int i = 0; i < nbruns; i++)
      sdev += (c_unmatch[det * nbruns + i] - mean)
              * (c_unmatch[det * nbruns + i] - mean);
    sdev = sqrt (sdev / (nbruns - 1));
    cout << mean << " (pm " << sdev
         << ") undetected segments per image" << endl;

    /*
    mean = 0.;
    sdev = 0.;
    for (int i = 0; i < nbruns; i++)
      mean += nbIniPts[i] - c_undet[det * nbruns + i];
    mean /= total_nbIniPts;
    for (int i = 0; i < nbruns; i++)
      sdev += ((nbIniPts[i] - c_undet[det * nbruns + i])
               / (double) nbIniPts[i] - mean)
              * ((nbIniPts[i] - c_undet[det * nbruns + i])
                 / (double) nbIniPts[i] - mean);
    sdev = sqrt (sdev / (nbruns - 1));
    cout << "Weighted recall : " << 100 * mean << " (pm " << 100 * sdev
         << ") % of points found" << endl;
    */

    mean = 0.;
    sdev = 0.;
    for (int i = 0; i < nbruns; i++) mean += c_redet[det * nbruns + i];
    mean /= total_nbIniPts;
    for (int i = 0; i < nbruns; i++)
      sdev += (c_redet[det * nbruns + i] / (double) nbIniPts[i] - mean)
              * (c_redet[det * nbruns + i] / (double) nbIniPts[i] - mean);
    sdev = sqrt (sdev / (nbruns - 1));
    cout << 100 * mean << " (pm " << 100 * sdev
         << ") % of points found more than once (redetections)" << endl;

    mean = 0.;
    sdev = 0.;
    for (int i = 0; i < nbruns; i++) mean += c_false[det * nbruns + i];
    mean /= total_nbIniPts;
    for (int i = 0; i < nbruns; i++)
      sdev += (c_false[det * nbruns + i] / (double) nbIniPts[i] - mean)
              * (c_false[det * nbruns + i] / (double) nbIniPts[i] - mean);
    sdev = sqrt (sdev / (nbruns - 1));
    cout << 100 * mean << " (pm " << 100 * sdev
         << ") % false points produced" << endl;

    /*
    mean = 0.;
    sdev = 0.;
    int numer = 0;
    int denom = 0;
    for (int i = 0; i < nbruns; i++)
    {
      numer += nbIniPts[i] - c_undet[det * nbruns + i];
      denom += c_false[det * nbruns + i]
               + nbIniPts[i] - c_undet[det * nbruns + i];
    }
    mean = numer / (double) denom;
    for (int i = 0; i < nbruns; i++)
      sdev += (m_precision[det * nbruns + i] - mean)
              * (m_precision[det * nbruns + i] - mean);
    sdev = sqrt (sdev / (nbruns - 1));
    cout << "Weighted precision : " << mean << " (pm " << sdev << ")" << endl;
    */

    mean = 0.;
    sdev = 0.;
    for (int i = 0; i < nbruns; i++) mean += m_precision[det * nbruns + i];
    mean /= nbruns;
    for (int i = 0; i < nbruns; i++)
      sdev += (m_precision[det * nbruns + i] - mean)
              * (m_precision[det * nbruns + i] - mean);
    sdev = sqrt (sdev / (nbruns - 1));
    cout << "Precision : " << mean << " (pm " << sdev << ")" << endl;

    mean = 0.;
    sdev = 0.;
    for (int i = 0; i < nbruns; i++) mean += m_recall[det * nbruns + i];
    mean /= nbruns;
    for (int i = 0; i < nbruns; i++)
      sdev += (m_recall[det * nbruns + i] - mean)
              * (m_recall[det * nbruns + i] - mean);
    sdev = sqrt (sdev / (nbruns - 1));
    cout << "Recall : " << mean << " (pm " << sdev << ")" << endl;

    mean = 0.;
    sdev = 0.;
    for (int i = 0; i < nbruns; i++) mean += m_fmeasure[det * nbruns + i];
    mean /= nbruns;
    for (int i = 0; i < nbruns; i++)
      sdev += (m_fmeasure[det * nbruns + i] - mean)
              * (m_fmeasure[det * nbruns + i] - mean);
    sdev = sqrt (sdev / (nbruns - 1));
    cout << "F-measure : " << mean << " (pm " << sdev << ")" << endl;
    total = 0;
    for (int i = 0; i < nbruns; i++) total += m_matched[det * nbruns + i];

    mean = 0.;
    sdev = 0.;
    for (int i = 0; i < nbruns; i++) mean += m_biased_width[det * nbruns + i];
    mean /= total;
    for (int i = 0; i < nbruns; i++)
      sdev += (m_biased_width[det * nbruns + i]
               / (double) m_matched[det * nbruns + i] - mean)
              * (m_biased_width[det * nbruns + i]
                / (double) m_matched[det * nbruns + i] - mean);
    sdev = sqrt (sdev / (nbruns - 1));
    cout << "Biased width : " << mean << " (" << sdev
         << ") per matched segment" << endl;

    mean = 0.;
    sdev = 0.;
    for (int i = 0; i < nbruns; i++) mean += m_width[det * nbruns + i];
    mean /= total;
    for (int i = 0; i < nbruns; i++)
      sdev += (m_width[det * nbruns + i]
               / (double) m_matched[det * nbruns + i] - mean)
              * (m_width[det * nbruns + i]
                / (double) m_matched[det * nbruns + i] - mean);
    sdev = sqrt (sdev / (nbruns - 1));
    cout << "Width : " << mean << " (" << sdev
         << ") per matched segment" << endl;

    mean = 0.;
    sdev = 0.;
    for (int i = 0; i < nbruns; i++) mean += m_wdiff[det * nbruns + i];
    mean /= total;
    for (int i = 0; i < nbruns; i++)
      sdev += (m_wdiff[det * nbruns + i]
               / (double) m_matched[det * nbruns + i] - mean)
              * (m_wdiff[det * nbruns + i]
                / (double) m_matched[det * nbruns + i] - mean);
    sdev = sqrt (sdev / (nbruns - 1));
    cout << "Width difference : " << mean << " (" << sdev
         << ") per matched segment" << endl;

    mean = 0.;
    sdev = 0.;
    for (int i = 0; i < nbruns; i++) mean += m_abswdiff[det * nbruns + i];
    mean /= total;
    for (int i = 0; i < nbruns; i++)
      sdev += (m_abswdiff[det * nbruns + i]
               / (double) m_matched[det * nbruns + i] - mean)
              * (m_abswdiff[det * nbruns + i]
                / (double) m_matched[det * nbruns + i] - mean);
    sdev = sqrt (sdev / (nbruns - 1));
    cout << "Absolute width difference : " << mean << " (" << sdev
         << ") per matched segment" << endl;

    mean = 0.;
    sdev = 0.;
    for (int i = 0; i < nbruns; i++) mean += m_adiff[det * nbruns + i];
    mean /= total;
    for (int i = 0; i < nbruns; i++)
      sdev += (m_adiff[det * nbruns + i]
               / (double) m_matched[det * nbruns + i] - mean)
              * (m_adiff[det * nbruns + i]
                / (double) m_matched[det * nbruns + i] - mean);
    sdev = sqrt (sdev / (nbruns - 1));
    cout << "Angle difference : " << mean << " (" << sdev
         << ") degrees per matched segment" << endl;

    mean = 0.;
    sdev = 0.;
    for (int i = 0; i < nbruns; i++) mean += m_absadiff[det * nbruns + i];
    mean /= total;
    for (int i = 0; i < nbruns; i++)
      sdev += (m_absadiff[det * nbruns + i]
               / (double) m_matched[det * nbruns + i] - mean)
              * (m_absadiff[det * nbruns + i]
                / (double) m_matched[det * nbruns + i] - mean);
    sdev = sqrt (sdev / (nbruns - 1));
    cout << "Absolute angle difference : " << mean << " (" << sdev
         << ") per matched segment" << endl;

    mean = 0.;
    sdev = 0.;
    for (int i = 0; i < nbruns; i++) mean += m_long_absadiff[det * nbruns + i];
    mean /= total;
    for (int i = 0; i < nbruns; i++)
      sdev += (m_long_absadiff[det * nbruns + i]
               / (double) m_matched[det * nbruns + i] - mean)
              * (m_long_absadiff[det * nbruns + i]
                / (double) m_matched[det * nbruns + i] - mean);
    sdev = sqrt (sdev / (nbruns - 1));
    cout << "Absolute long edge angle difference : " << mean << " (" << sdev
         << ") per matched segment" << endl;
  }
}


void BSRandomTester::generateImage ()
{
  if (dispEach) cout << "Generating new segments" << endl;
  int val;
  for (int i = 0; i < width; i ++)
    for (int j = 0; j < height; j ++)
    {
      // val = rand () % 30;   // ZZZ
      val = 255 - (rand () % 30);
      image.setPixel (i, j, val + val * 256 + val * 256 * 256);
    }

  bool nok;
  double score1, score2, score3;
  Pt2i bsc1, bsc2;
  Vr2i ali;
  for (int i = 0; i < nbsegs; i++)
  {
    do
    {
      nok = false;
      rp1[i].set (margin + (rand () % swidth), margin + (rand () % sheight));
      rp2[i].set (margin + (rand () % swidth), margin + (rand () % sheight));
      if (rp1[i].chessboard (rp2[i]) < sminlength) nok = true;
      else
      {
        rdir[i] = rp1[i].vectorTo (rp2[i]);
        bsc1.set ((rp1[i].x () + rp2[i].x ()) / 2,
                  (rp1[i].y () + rp2[i].y ()) / 2);
        for (int si = 0; (! nok) && si < i; si ++)
        {
          score1 = rdir[si].squaredScalarProduct (rdir[i])
                          / (double) (rdir[si].norm2 () * rdir[i].norm2 ());
          if (rp1[si].chessboard (bsc1) < sminlength / 2)
            ali = bsc1.vectorTo (rp2[si]);
          else ali = rp1[si].vectorTo (bsc1);
          score2 = rdir[si].squaredScalarProduct (ali)
                   / (double) (rdir[si].norm2 () * ali.norm2 ());
          bsc2.set ((rp1[si].x () + rp2[si].x ()) / 2,
                    (rp1[si].y () + rp2[si].y ()) / 2);
          if (rp1[i].chessboard (bsc2) < sminlength / 2)
            ali = bsc2.vectorTo (rp2[i]);
          else ali = rp1[i].vectorTo (bsc2);
          score3 = rdir[i].squaredScalarProduct (ali)
                   / (double) (rdir[i].norm2 () * ali.norm2 ());
          if (score1 > 0.9 && (score2 > 0.9 || score3 > 0.9)) nok = true;
        }
      }
    }
    while (nok);
    rw[i] = sminwidth + (rand () % (smaxwidth - sminwidth));

    DigitalStraightSegment dss (rp1[i], rp2[i], rw[i]);
    vector<Pt2i> pix;
    dss.getPoints (pix);
    vector<Pt2i>::iterator it = pix.begin ();
    while (it != pix.end ())
    {
      Pt2i p = *it++;
      if (p.x () >= 0 && p.x () < width && p.y () >= 0 && p.y () < height)
        image.setPixel (p.x (), height - 1 - p.y (), 0);
        // image.setPixel (p.x (), height - 1 - p.y (),  // ZZZ
        //                 255 + 255 * 256 + 255 * 256 * 256);  // ZZZ
    }
  }

  for (int j = 0; j < height; j++)
    for (int i = 0; i < width; i++)
      tofind_map[j * width + i] =
        QColor (image.pixel (i, height - 1 - j)).value () < 10;
        // QColor (image.pixel (i, height - 1 - j)).value () > 200; // ZZZ
  if (dispEach) cout << "New segments generated" << endl;
}


void BSRandomTester::createMap (QString name)
{
  QImage mymap = QImage (width, height, QImage::Format_RGB32);
  for (int j = 0; j < height; j ++)
    for (int i = 0; i < width; i ++)
    {
      int col = 0; 
      if (tofind_map[j * width + i])
        if (found_map[j * width + i]) col = 255 * 256;
        else col = 255;
      else if (found_map[j * width + i]) col = 255 * 256 * 256;
      mymap.setPixel (i, j, col);
    }
  mymap.save (name + "_map.png");
}


int **BSRandomTester::getBitmap (const QImage &image)
{
  int w = image.width ();
  int h = image.height ();

  int **tabImage = new int*[h];
  for (int i = 0; i < h; i++)
  {
    tabImage[i] = new int[w];
    for(int j = 0; j < w; j++)
    {
      QColor c = QColor (image.pixel (j, h - i - 1));
      tabImage[i][j] = c.value ();
    }
  }
  return tabImage;
}
