#ifndef BS_RANDOM_TESTER_H
#define BS_RANDOM_TESTER_H

#include <QImage>
#include <QString>
#include "bsdetector.h"

using namespace std;


/** 
 * @class BSRandomTester bsrandomtester.h
 * \brief Segment detection random tester.
 * Tests different detectors on randomly generated images.
 * \author {P. Even}
 */
class BSRandomTester
{
public:

  /**
   * \brief Creates and initializes a segment detection random tester.
   */
  BSRandomTester ();

  /**
   * \brief Deletes the random tester.
   */
  ~BSRandomTester ();

  /**
   * \brief Runs a random test.
   */
  void randomTest ();


private:
 
  /** Effective test area width. */
  int swidth;
  /** Effective test area height. */
  int sheight;
  /** Test image margin size. */
  int margin;

  /** Number of random test images. */
  int nbruns;
  /** Number of randomly generated segments per image. */
  int nbsegs;
  /** Minimal thickness of randomly generated segments. */
  int sminwidth;
  /** Maximal thickness of randomly generated segments. */
  int smaxwidth;
  /** Assigned width tolerence to maximal segment width. */
  int swmargin;
  /** Minimal length of randomly generated segments. */
  int sminlength;
  /** Minimal squared length of detected segments considered as long. */
  int longEdgeThreshold;

  /** Generated image. */
  QImage image;
  /** Gradient map of the loaded picture. */
  VMap *gMap;
  /** Width of the present image. */
  int width;
  /** Height of the present image. */
  int height;
  /** Size of the present image. */
  int isize;

  /** Generated segments start point. */
  Pt2i *rp1;
  /** Generated segments end point. */
  Pt2i *rp2;
  /** Generated segments support vector. */
  Vr2i *rdir;
  /** Generated segments width. */
  int *rw;
  /** Occupancy map. */
  bool *tofind_map;
  /** Amount of detected (positive) points.
   * Negative sign for false positives, positive sign for true positives. */
  int *hit_map;
  /** Undetected points map (false negative points). */
  bool *stilltofind_map;
  /** Found points map (positive points). */
  bool *found_map;
  /** Correct found points map (true positive points). */
  bool *foundin_map;
  /** Incorrect found points map (false positive points). */
  bool *foundout_map;

  /** Per image results display modality. */
  bool dispEach;
  /** Last image results display modality. */
  bool dispLast;

  /** Number of tested detector configurations. */
  int nbdets;
  /** Blurred segment detectors. */
  BSDetector *detectors;
  /** Detectors names. */
  QString *names;
  /** Gradient extraction bias removal modality. */
  bool unbiasOn;
  /** Gradient extraction bias value. */
  AbsRat biasVal;

  /** Amount of points to find (points of synthetized segments). */
  int *nbIniPts;
  /** Amount of tests on local minima per each detection. */
  int *c_trials;
  /** Amount of detected segments per each detection. */
  int *c_det;
  /** Amount of long detected segments per each detection. */
  int *c_ldet;
  /** Amount of unmatched segments per each detection. */
  int *c_unmatch;
  /** Amount of undetected (false negative) points per each detection. */
  int *c_undet;
  /** Amount of trully detected (true positive) points per each detection. */
  int *c_true;
  /** Amount of wrongly detected (false positive) points per each detection. */
  int *c_false;
  /** Amount of redetected points per each detection. */
  int *c_redet;

  /** Measured precision per detector and per test. */
  double *m_precision;
  /** Measured recall per detector and per test. */
  double *m_recall;
  /** Measured F-measure per detector and per test. */
  double *m_fmeasure;
  /** Amount of matched segments per detector and per test. */
  double *m_matched;
  /** Measured biased width per detector and per test. */
  double *m_biased_width;
  /** Measured (debiased) width per detector and per test. */
  double *m_width;
  /** Measured width offset per detector and per test. */
  double *m_wdiff;
  /** Measured absolute width offset per detector and per test. */
  double *m_abswdiff;
  /** Measured angle offset per detector and per test. */
  double *m_adiff;
  /** Measured absolute angle offset per detector and per test. */
  double *m_absadiff;
  /** Measured absolute angle offset of long segments. */
  double *m_long_absadiff;


  /**
   * \brief Generates a new random image of segments.
   */
  void generateImage ();

  /**
   * \brief Builds and returns the tested maps.
   */
  void createMap (QString name);

  /**
   * \brief Builds and returns the image bitmap.
   */
  int **getBitmap (const QImage &image);
};
#endif
