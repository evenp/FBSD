#ifndef BS_DETECTION_WIDGET_H
#define BS_DETECTION_WIDGET_H

#include <QGraphicsView>
#include <QColor>
#include <QImage>
#include <QWidget>
#include <QVector>
#include <fstream>
#include "bsdetector.h"
// #include "bsaccumulatorview.h"
// #include "bsstructureview.h"
// #include "bsprofileview.h"
// #include "bsidetview.h"

using namespace std;


/** 
 * @class BSDetectionWidget bsdetectionwidget.h
 * \brief Segment extraction view and controller.
 * \author {P. Even and B. Kerautret}
 */
class BSDetectionWidget : public QWidget 
{
  Q_OBJECT


public:

  /**
   * \brief Creates a segment extraction widget.
   */
  BSDetectionWidget (QWidget *parent = 0);

  /**
   * \brief Deletes the segment extraction widget.
   */
  ~BSDetectionWidget ();

  /**
   * \brief Opens the image to be processed.
   * @param type Name of the image file to open.
   * @param type Gradient extraction method.
   */
  QSize openImage (const QString &fileName, int type = 0);

  /**
   * \brief Applies default options (read in defaults files).
   */
  void setDefaults ();

  /**
   * \brief Builds and returns the image bitmap.
   */
  int **getBitmap (const QImage &image);
  
  /**
   * \brief Updates the Qt widget display.
  void paint (QPainter *painter,
              const QStyleOptionGraphicsItem *option, QWidget *widget);
   */

  /**
   * \brief Toggles the background image.
   */
  void toggleBackground ();

  /**
   * \brief Saves the augmented image with extraction results.
   */
  bool saveAugmentedImage (const QString &fileName, const char *fileFormat);

  /**
   * \brief Builds ans stores the gradient image.
   * @param dir : Gradient type (magnitude, x or y)
   */
  void buildGradientImage (int dir);

  /**
   * \brief Requires the accumulation window closure.
   */
  // void closeAccuAnalyzer ();

  /**
   * \brief Requires the pixel analysis window closure.
   */
  // void closePixelAnalyzer ();

  /**
   * \brief Requires the profile analysis window closure.
   */
  // void closeProfileAnalyzer ();

  /**
   * \brief Requires the initial detection analysis window closure.
   */
  // void closeIdetAnalyzer ();

  /**
   * \brief Switches the pixel display window on or off.
   */
  // void switchPixelAnalyzer ();

  /**
   * \brief Switches the accumulator display window on or off.
   */
  // void switchAccuAnalyzer ();

  /**
   * \brief Switches the profile display window on or off.
   */
  // void switchProfileAnalyzer ();

  /**
   * \brief Switches the initial detection display window on or off.
   */
  // void switchIdetAnalyzer ();

  /**
   * \brief Switches the blurred segment highlight colors.
   */
  void switchHighlightColors ();

  /**
   * \brief Switches the random color display modality.
   */
  void switchArlequin ();

  /**
   * \brief Switches the extraction stats display on or off.
   */
  inline void switchStats () { stats = ! stats; }

  /**
   * \brief Switches the extraction result display on or off.
   */
  inline void switchVerbose () { verbose = ! verbose; }

  /**
   * \brief Runs a comparative test.
   */
  void alternateTest ();

  /**
   * \brief Runs a performance test.
   * Displays the time spent for 1000 detections under the present stroke.
   */
  void performanceTest ();

  /**
   * \brief Runs a local test (reads test.txt).
   */
  void localTest ();


public slots:
  /**
   * \brief Clears the widget drawing.
   */
  void clearImage ();


protected:
  /**
   * \brief Updates the widget drawing.
   */
  void paintEvent (QPaintEvent *event);

  /**
   * \brief Processes mouse press events.
   */
  void mousePressEvent (QMouseEvent *event);

  /**
   * \brief Processes mouse release events.
   */
  void mouseReleaseEvent (QMouseEvent *event);

  /**
   * \brief Processes move release events.
   */
  void mouseMoveEvent (QMouseEvent *event);

  /**
   * \brief Processes key press events.
   */
  void keyPressEvent (QKeyEvent *event);


private:
 
  /** Background status : uniform black. */
  static const int BACK_BLACK;
  /** Background status : uniform white. */
  static const int BACK_WHITE;
  /** Background status : intensity image displayed. */
  static const int BACK_IMAGE;
  /** Background status : gradient image displayed. */
  static const int BACK_GRAD;
  /** Background status : X-gradient image displayed. */
  static const int BACK_GRADX;
  /** Background status : Y-gradient image displayed. */
  static const int BACK_GRADY;

  /** Default value for pen width. */
  static const int DEFAULT_PEN_WIDTH;


  /** Initial scan start point. */
  Pt2i p1;
  /** Initial scan end point. */
  Pt2i p2;
  /** Saved scan start point. */
  Pt2i oldp1;
  /** Saved scan end point. */
  Pt2i oldp2;
  /** Flag indicating if the mouse is not dragging. */
  bool nodrag;
  /** Flag indicating if the detection is user defined. */
  bool udef;
  /** Saved user definition flag. */
  bool oldudef;

  /** Kind of highlight colors. */
  bool darkHighlightOn;
  /** Random color selection modality. */
  bool arlequinOn;
  /** Color of user selections. */
  QColor selectionColor;
  /** Color of blurred segments. */
  QColor bsColor;
  /** Color of highlighted blurred segments. */
  QColor bsHighColor;
  /** Flag indicating whether blurred segments points are visible. */
  bool bsPointsVisible;
  /** Color of blurred segments bounds. */
  QColor boundColor;
  /** Color of highlighted blurred segments bounds. */
  QColor boundHighColor;
  /** Flag indicating whether blurred segments bounds are visible. */
  bool bsBoundsVisible;
  /** Background type.
   * BACK_BLACK, BACK_WHITE, BACK_IMAGE, BACK_GRAD, BACK_GRADX, BACK_GRADY. */
  int background;
  /** Black level used to lighten background images. */
  int blevel;
  /** Flag indicating whether detection stats should be output. */
  bool stats;
  /** Flag indicating whether detection result should be output. */
  bool verbose;

  /** Presently loaded image. */
  QImage loadedImage;
  /** Computed gradient image. */
  QImage gradImage;
  /** Present image augmented with processed data. */
  QImage augmentedImage;
  /** Gradient map of the loaded picture. */
  VMap *gMap;
  /** Width of the present image. */
  int width;
  /** Height of the present image. */
  int height;

  /** Blurred segment detector. */
  BSDetector detector;
  /** Initial detection graphics view. */
  // BSIdetView *idetview;
  /** Scanned profile graphics view. */
  // BSProfileView *profileview;
  /** Filter accumulator view. */
  // BSAccumulatorView *accuview;
  /** Blurred segment contents view. */
  // BSStructureView *strucview;

  /** Aggregation of segment extraction results with initial conditions. */
  struct ExtractedSegment
  {
    /** Extracted blurred segment. */
    BlurredSegment *bs;
    /** Selection stroke start point. */
    Pt2i p1;
    /** Selection stroke end point. */
    Pt2i p2;
  };
  /** List of registred blurred segments. */
  vector<ExtractedSegment> extractedSegments;



  /**
   * \brief Draws a list of points with the given color.
   * @param painter Drawing device.
   * @param pts List of points to be drawn.
   * @param color Drawing color.
   */
  void drawPoints (QPainter &painter,
                   vector<Pt2i> pts, QColor color);

  /**
   * \brief Draws a list of image pixels.
   * @param painter Drawing device.
   * @param pix List of pixels to be drawn.
   */
  void drawPixels (QPainter &painter, vector<Pt2i> pix);

  /**
   * \brief Draws the line joining two points.
   * @param painter Drawing device.
   * @param from Line start position.
   * @param to Line reach position.
   * @param color Drawing color.
   */
  void drawLine (QPainter &painter,
                 const Pt2i from, const Pt2i to, QColor color);

  /**
   * \brief Draws a user selection.
   * @param painter Drawing device.
   * @param from Selection line start position.
   * @param to Selection line reach position.
   */
  void drawSelection (QPainter &painter, const Pt2i from, const Pt2i to);

  /**
   * \brief Draws a blurred segment.
   * @param painter Drawing device.
   * @param bs Reference to the blurred segment to be drawn.
   * @param high Flag indicated whether the blurred segment is highlighted.
   */
  void drawBlurredSegment (QPainter &painter,
                           BlurredSegment *bs, bool high = true);

  /**
   * \brief Draws a blurred segment with a random color.
   * @param painter Drawing device.
   * @param bs Reference to the blurred segment to be drawn.
   */
  void drawArlequinSegment (QPainter &painter, BlurredSegment *bs);

  /**
   * \brief Returns the background black level.
   */
  inline int getBlackLevel () const { return (blevel); }

  /**
   * \brief Increments the background black level.
   * @param val Increment value;
   */
  void incBlackLevel (int val);

  /**
   * \brief Lighten the image according to the black level set.
   * @param im Image to lighten.
   */
  void lighten (QImage &im);

  /**
   * \brief Displays the window background (no detection).
   */
  void displayBackground ();

  /**
   * \brief Writes the stats of the last detection in a file.
   */
  void writeStats ();

  /**
   * \brief Writes the selection stroke in test.txt.
   */
  void writeTest ();

  /**
   * \brief Writes the result of the last detection in a file.
   */
  void writeDetectionResult ();

  /**
   * \brief Displays the result of the last detection.
   */
  void displayDetectionResult ();

  /**
   * \brief Displays the saved blurred segments.
   */
  void displaySavedSegments ();

  /**
   * \brief Registers the last extracted blurred segment.
   * Returns whether something is saved or not.
   */
  bool saveExtractedSegment ();

  /**
   * \brief Clears off the saved blurred segments.
   */
  void clearSavedSegments ();

  /**
   * \brief Outputs the last detection result status.
   */
  void writeDetectionStatus ();

  /**
   * \brief Stores the occupancy mask in a mask.png.
   */
  void saveMask ();

  /**
   * \brief Detects and displays a blurred segment under the selected stroke.
   */
  void extract ();

  /**
   * \brief Stores the user input in test.txt.
   */
  void storeUserInput ();

};
#endif
