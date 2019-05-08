#include <QtGui>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "bsdetectionwidget.h"

using namespace std;


const int BSDetectionWidget::BACK_BLACK = 0;
const int BSDetectionWidget::BACK_WHITE = 1;
const int BSDetectionWidget::BACK_IMAGE = 2;
const int BSDetectionWidget::BACK_GRAD = 3;
const int BSDetectionWidget::BACK_GRADX = 4;
const int BSDetectionWidget::BACK_GRADY = 5;

const int BSDetectionWidget::DEFAULT_PEN_WIDTH = 1;
const int BSDetectionWidget::SELECT_TOL = 5;



BSDetectionWidget::BSDetectionWidget (QWidget *parent)
{
  Q_UNUSED (parent);

  // Sets initial user inputs parameters
  setFocus ();
  grabKeyboard ();
  picking = false;
  udef = false;
  nodrag = true;

  // Initializes the gradient map and the auxiliary views
  gMap = NULL;

  // Sets initial user outputs parameters
  verbose = false;
  statsOn = false;
  background = BACK_IMAGE;
  blevel = 0;

  // Sets display parameters
  bsDisplay = 1;
  arlequin = 0;
  bsColor = 2;
  bsHighColor[0] = Qt::black;
  bsLowColor[0] = Qt::gray;
  bsHighColor[1] = Qt::blue;
  bsLowColor[1] = Qt::magenta;
  bsHighColor[2] = Qt::yellow;
  bsLowColor[2] = Qt::red;
  bsHighColor[3] = Qt::white;
  bsLowColor[3] = Qt::gray;
  selectionColor = Qt::red;

  maxWidth = 768;
  maxHeight = 512;
  xMaxShift = 0;
  yMaxShift = 0;
  xShift = 0;
  yShift = 0;
  zoom = 1;
}


BSDetectionWidget::~BSDetectionWidget ()
{
}


QSize BSDetectionWidget::openImage (const QString &fileName, int type)
{
  QSize newSize (0, 0);
  loadedImage.load (fileName);
  width = loadedImage.width ();
  height = loadedImage.height ();
  newSize = loadedImage.size ();
 
  augmentedImage = loadedImage;
  if (gMap != NULL) delete gMap;
  gMap = new VMap (width, height, getBitmap (augmentedImage), type);
  detector.setGradientMap (gMap);
  buildGradientImage (0);
 
  update ();

  xMaxShift = (width > maxWidth ? maxWidth - width : 0);
  yMaxShift = (height > maxHeight ? maxHeight - height : 0);
  xShift = xMaxShift / 2;
  if (xShift > 0) xShift = 0;
  yShift = yMaxShift / 2;
  if (yShift > 0) yShift = 0;
  return newSize.boundedTo (QSize (maxWidth, maxHeight));
}


void BSDetectionWidget::setDefaults ()
{
  ifstream input ("gradient.txt", ios::in);
  if (input)
  {
    int gtval = gMap->getGradientThreshold ();
    input >> gtval;
    if (gtval >= 0 && gtval < 256)
      gMap->incGradientThreshold (gtval - gMap->getGradientThreshold ());
  }
}


int **BSDetectionWidget::getBitmap (const QImage &image)
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


void BSDetectionWidget::toggleBackground ()
{
  if (background++ == BACK_GRADY) background = BACK_BLACK;
  if (background >= BACK_GRAD) buildGradientImage (background - BACK_GRAD);
}


bool BSDetectionWidget::saveAugmentedImage (const QString &fileName,
                                            const char *fileFormat)
{
  QImage aImage = augmentedImage;
  return (aImage.save (fileName, fileFormat));
}


void BSDetectionWidget::clearImage ()
{
  augmentedImage.fill (qRgb (255, 255, 255));
  update ();
}


void BSDetectionWidget::buildGradientImage (int dir)
{
  int w = gMap->getWidth ();
  int h = gMap->getHeight ();
  double *gn = new double[w * h];
  // double gn[w * h];
  for (int j = 0; j < h; j++)
    for (int i = 0; i < w; i++)
    {
      if (dir == 2)
        gn[j * w + i] = gMap->getValue(i,j).y ();
      else if (dir == 1)
        gn[j * w + i] = gMap->getValue(i,j).x ();
      else
        gn[j * w + i] = gMap->magn (i, j);
    }
  double min = gn[0];
  double max = gn[0];
  for (int i = 1; i < w * h; i++)
  {
    if (max < gn[i]) max = gn[i];
    if (min > gn[i]) min = gn[i];
  }
  gradImage = QImage (w, h, QImage::Format_RGB32);
  for (int j = 0; j < h; j++)
    for (int i = 0; i < w; i++)
    {
      int val = (int) ((gn[(h - 1 - j) * w + i] - min) * 255 / (max - min));
      gradImage.setPixel (i, j, val + val * 256 + val * 256 * 256);
    }
  // gradImage.save ("hgradient.png");
}


void BSDetectionWidget::paintEvent (QPaintEvent *)
{
  QPainter painter (this);
  QImage zoomImage = augmentedImage.scaled (width / zoom, height / zoom);
  painter.drawImage (QPoint (xShift, yShift), zoomImage);
}


void BSDetectionWidget::switchColorSet (bool back)
{
  if (back)
  {
    if (--bsColor < 0) bsColor = 3;
  }
  else if (++ bsColor > 3) bsColor = 0;
}


void BSDetectionWidget::mousePressEvent (QMouseEvent *event)
{
  int ex = zoom * (event->pos().x () - xShift);
  int ey = height - 1 - zoom * (event->pos().y () - yShift);

  if (event->button () == Qt::RightButton)
  {
    vector<BlurredSegment *> bsl = detector.getBlurredSegments ();
    if (! bsl.empty ())
    {
      bool searching = true;
      int nb = 0;
      vector<BlurredSegment *>::iterator it = bsl.begin ();
      while (searching && it != bsl.end ())
      {
        if (*it != NULL)
        {
          DigitalStraightSegment *dss = (*it)->getSegment ();
          if (dss != NULL)
          {
            if (dss->contains (Pt2i (ex, ey), SELECT_TOL))
            {
              searching = false;
            }
            else nb ++;
          }
          else nb ++;
        }
        else nb ++;
        it ++;
      }
      cout << "Selection of segment " << (searching ? 0 : nb + 1) << endl;
      detector.setMaxDetections (searching ? 0 : nb + 1);
      extract ();
    }
    picking = true;
  }
  else
  {
    oldp1.set (p1);
    oldp2.set (p2);
    oldudef = udef;
    p1 = Pt2i (ex, ey);
    if (p1.manhattan (p2) < 10) p1.set (oldp1);
    else if (p1.manhattan (oldp1) < 10) p1.set (p2);
    udef = true;
  }
}


void BSDetectionWidget::mouseReleaseEvent (QMouseEvent *event)
{
  if (! picking)
  {
    int ex = zoom * (event->pos().x () - xShift);
    int ey = zoom * (event->pos().y () - yShift);
    p2 = Pt2i (ex, height - 1 - ey);
    if (p1.equals (p2))
    {
      p1.set (oldp1);
      p2.set (oldp2);
      udef = oldudef;
    }
    else
    {
      cerr << "p1 defined: " << p1.x () << " " << p1.y () << endl;
      cerr << "p2 defined: " << p2.x () << " " << p2.y () << endl;
      detector.resetMaxDetections ();
      extract ();
    }
  }
}


void BSDetectionWidget::mouseMoveEvent (QMouseEvent *event)
{
  if (picking) picking = false;
  else
  {
    int ex = zoom * (event->pos().x () - xShift);
    int ey = zoom * (event->pos().y () - yShift);
    p2 = Pt2i (ex, height - 1 - ey);
    if (verbose) cerr << "(" << p1.x () << ", " << p1.y () << ") ("
                      << p2.x () << ", " << p2.y () << ")" << endl;
    if (p1.manhattan (p2) > 5
        && (width > p2.x() && height > p2.y()
            && p2.x() > 0 && p2.y() > 0))
    {
      nodrag = false;
      extract ();
      nodrag = true;
      detector.setMaxTrials (0);
    }
  }
}


void BSDetectionWidget::keyPressEvent (QKeyEvent *event)
{
  int count = 0;
  if (isActiveWindow ()) switch (event->key ())
  {
    case Qt::Key_A :
      // Registers the last extracted blurred segment
      count = saveExtractedSegment ();
      cout << count << " new segment(s) -> "
           << extractedSegments.size () << " segments registered" << endl;
      break;

    case Qt::Key_B :
      if (event->modifiers () & Qt::ControlModifier)
      {
        // Toggles background image
        toggleBackground ();
        if (p1.equals (p2)) displayBackground ();
        else displayDetectionResult ();
      }
      else
      {
        // Tunes the background image black level
        incBlackLevel ((event->modifiers () & Qt::ShiftModifier) ? -1 : 1);
        cout << "Background black level = " << getBlackLevel () << endl;
        displayDetectionResult ();
      }
      break;

    case Qt::Key_C :
      // Toggles blurred segment color set
      switchColorSet (event->modifiers () & Qt::ShiftModifier);
      displayDetectionResult ();
      break;

    case Qt::Key_D :
      // Toggles blurred segment display style
      toggleBlurredSegmentDisplay (event->modifiers () & Qt::ShiftModifier);
      if (bsDisplay == 0)
        cout << "Blurred segment display : unvisible" << endl;
      else if (bsDisplay == 1)
        cout << "Blurred segment display : points" << endl;
      else if (bsDisplay == 2)
        cout << "Blurred segment display : bounds" << endl;
      else if (bsDisplay == 3)
        cout << "Blurred segment display : bounds and points" << endl;
      displayDetectionResult ();
      break;

    case Qt::Key_E :
      {
        if (event->modifiers () & Qt::ControlModifier)
        {
          // Handles single or double edge detection
          detector.switchSingleOrDoubleEdge ();
          if (detector.isSingleEdgeModeOn ())
            cout << "Single edge detection set ("
                 << (detector.isOppositeGradientOn () ?
                     "opposite" : "main") << ")" << endl;
          else cout << "Double edge detection set" << endl;
          extract ();
        }
        else
        {
          // Handles gradient orientation direction for the detection
          if (detector.switchOppositeGradient ())
          {
            cout << "Single edge detection set ("
                 << (detector.isOppositeGradientOn () ?
                     "opposite" : "main") << ")" << endl;
            extract ();
          }
        }
      }
      break;

    case Qt::Key_F :
      if (event->modifiers () & Qt::ControlModifier)
      {
        // Switches initial detection filtering
        detector.switchFiltering (BSDetector::STEP_INITIAL);
        cout << "Pre-filtering "
             << (detector.isFiltering (BSDetector::STEP_INITIAL) ? "on" : "off")
             << endl;
        extract ();
      }
      break;

    case Qt::Key_G :
      // Tunes the gradient threshold for maximal value detection
      detector.incSensitivity (
        (event->modifiers () & Qt::ShiftModifier ? -1 : 1));
      cout << "Sensitivity = " << detector.getSensitivity () << endl;
      extract ();
      break;

    case Qt::Key_H :
      if (event->modifiers () & Qt::ControlModifier)
      {
        // Switches final detection filtering
        detector.switchFiltering (BSDetector::STEP_FINAL);
        cout << "Final filtering "
             << (detector.isFiltering (BSDetector::STEP_FINAL) ? "on" : "off")
             << endl;
        extract ();
      }
      break;

    case Qt::Key_K :
      if (event->modifiers () & Qt::ControlModifier)
      {
        // Switches the final fragmentation test
        detector.switchFinalFragmentationTest ();
        cout << "Final fragmentation test "
             << (detector.isFinalFragmentationTestOn () ? "on" : "off")
             << endl;
        extract ();
      }
      else
      {
        // Tunes the minimal size of segment fragments
        detector.incFragmentSizeMinValue (
                    (event->modifiers () & Qt::ShiftModifier) == 0);
        cout << "Fragments minimal size = "
             << detector.fragmentSizeMinValue () << endl;
        extract ();
      }
      break;

    case Qt::Key_M :
      if (event->modifiers () & Qt::ControlModifier)
      {
        // Switches the multi-detection
        detector.switchMultiSelection ();
        cout << "Multi-selection "
             << (detector.isMultiSelection () ? "on" : "off") << endl;
      }
      else
      {
        // Runs an automatic detection
        udef = false;
        detector.resetMaxDetections ();
        cout << "Detects all segments" << endl;
        extract ();
      }
      break;

    case Qt::Key_N :
      if (event->modifiers () & Qt::ControlModifier)
      {
        // Switches the initial detection extension limitation
        detector.switchInitialBounding ();
        cout << "Initial step max extension = "
             << detector.initialDetectionMaxExtent () << endl;
        extract ();
      }
      else
      {
        // Highlights the next segment in a multi-detection
        vector<BlurredSegment *> bsl = detector.getBlurredSegments ();
        if (! bsl.empty ())
        {
          detector.incMaxDetections (event->modifiers () & Qt::ShiftModifier);
          cout << "Selection of segment "
               << detector.getMaxDetections () << endl;
          extract ();
        }
      }
      break;

    case Qt::Key_O :
      if (event->modifiers () & Qt::ControlModifier)
      {
        // Switches measured values display
        switchStats ();
        cout << "Stats display " << (isStatsOn () ? "on" : "off") << endl;
        displayDetectionResult ();
      }
      else
      {
        // Outputs the detected segment(s)
        cout << "Outputs detection result" << endl;
        writeDetectionResult ();
      }
      break;

    case Qt::Key_P :
      // Captures main window
      cout << "Saves main window in capture.png" << endl;
      augmentedImage.save ("capture.png");
      break;

    case Qt::Key_Q :
      // Displays registered blurred segments
      cout << "Displays all registered segments" << endl;
      displaySavedSegments ();
      break;

    case Qt::Key_R :
      // Tunes the sweeping step value for automatic detections
      detector.setAutoSweepingStep (detector.getAutoSweepingStep () +
        (event->modifiers () & Qt::ShiftModifier ? -1 : 1));
      cout << "Stroke sweeping step for automatic detections = "
           << detector.getAutoSweepingStep () << " pixels" << endl;
      break;

    case Qt::Key_S :
      if (event->modifiers () & Qt::ControlModifier)
      {
        // Switches the final size test of detected blurred segments
        detector.switchFinalSizeTest ();
        cout << "Final size test "
             << (detector.isFinalSizeTestOn () ? "on" : "off") << endl;
        extract ();
      }
      else
      {
        // Tunes the final size threshold
        detector.setFinalSizeMinValue (detector.finalSizeMinValue () +
          (event->modifiers () & Qt::ShiftModifier ? -1 : 1));
        cout << "Minimal size of detected blurred segments = "
             << detector.finalSizeMinValue () << " points" << endl;
        extract ();
      }
      break;

    case Qt::Key_T :
      if (event->modifiers () & Qt::ControlModifier)
      {
        // Switches the interruption handling
        detector.switchAutoRestart ();
        cout << "Segment continuation after = "
             << detector.getRestartOnLack () << " pixels" << endl;
        extract ();
      }
      else
      {
        // Tunes the pixel lack tolerence value
        detector.setPixelLackTolerence (detector.getPixelLackTolerence () +
          (event->modifiers () & Qt::ShiftModifier ? -1 : 1));
        cout << "Tolerence to detection lacks = "
             << detector.getPixelLackTolerence () << " pixels" << endl;
        extract ();
      }
      break;

    case Qt::Key_U :
      // Replays last extraction
      cerr << "p1 update: " << p1.x () << " " << p1.y () << endl;
      cerr << "p2 update: " << p2.x () << " " << p2.y () << endl;
      extract ();
      break;

    case Qt::Key_V :
      if (event->modifiers () & Qt::ControlModifier)
        // Switches the detection result edition
        switchVerbose ();
      break;

    case Qt::Key_W :
      // Tunes the assigned thickness to detector
      detector.setAssignedThickness (detector.assignedThickness () +
        (event->modifiers () & Qt::ShiftModifier ? -1 : 1));
      cout << "Assigned thickness = "
           << detector.assignedThickness () << endl;
      extract ();
      break;

    case Qt::Key_X :
      if (event->modifiers () & Qt::ControlModifier)
      {
        // Switches the static detection modality
        detector.setStaticDetector (! (detector.staticDetectorOn ()));
        cout << (detector.staticDetectorOn () ?
                 "Static (without ADS and ATC) detector set" :
                 "Static (without ADS and ATC) detector released") << endl;
        extract ();
      }
      else
      {
        // Clears the registered blurred segments
        cout << "Withdraws registered segments" << endl;
        clearSavedSegments ();
        displaySavedSegments ();
      }
      break;

    case Qt::Key_Y :
      if (event->modifiers () & Qt::ControlModifier)
      {
        // Switches sparsity test at final step
        detector.switchFinalSparsityTest ();
        cout << "Final sparsity test : "
             << (detector.isFinalSparsityTestOn () ? "on" : "off") << endl;
        extract ();
      }
      break;

    case Qt::Key_Exclam :
      // Displays segments with dark random colors
      arlequin = 1;
      displayDetectionResult ();
      break;

    case Qt::Key_Colon :
      // Displays segments with light random colors
      arlequin = -1;
      displayDetectionResult ();
      break;

    case Qt::Key_Plus :
      if (zoom > 1)
      {
        zoom /= 2;
        xShift = xShift * 2 - maxWidth / 2;
        yShift = yShift * 2 - maxHeight / 2;
        cout << "Zoom : " << zoom << endl;
        displayDetectionResult ();
      }
      break;

    case Qt::Key_Minus :
      if (width / zoom > maxWidth || height / zoom > maxHeight)
      {
        zoom *= 2;
        xShift = xShift / 2 + maxWidth / 4;
        if (xShift > 0) xShift = 0;
        if ((maxWidth - xShift) * zoom > width)
          xShift = maxWidth - width / zoom;
        yShift = yShift / 2 + maxHeight / 4;
        if (yShift > 0) yShift = 0;
        if ((maxHeight - yShift) * zoom > height)
          yShift = maxHeight - height / zoom;
        cout << "Zoom : " << zoom << endl;
        displayDetectionResult ();
      }
      break;

    case Qt::Key_Left :
      xShift += 50;
      if (xShift > 0) xShift = 0;
      cout << "X-shift : " << xShift << endl;
      displayDetectionResult ();
      break;

    case Qt::Key_Right :
      xShift -= 50;
      if ((maxWidth - xShift) * zoom > width)
        xShift = maxWidth - width / zoom;
      cout << "X-shift : " << xShift << endl;
      displayDetectionResult ();
      break;

    case Qt::Key_Up :
      yShift += 50;
      if (yShift > 0) yShift = 0;
      cout << "Y-shift : " << yShift << endl;
      displayDetectionResult ();
      break;

    case Qt::Key_Down :
      yShift -= 50;
      if ((maxHeight - yShift) * zoom > height)
        yShift = maxHeight - height / zoom;
      cout << "Y-shift : " << yShift << endl;
      displayDetectionResult ();
      break;

    case Qt::Key_7 :
      storeUserInput ();
      break;

    case Qt::Key_0 :
      localTest ();
      break;
  }
}


void BSDetectionWidget::toggleBlurredSegmentDisplay (bool getBack)
{
  if (getBack)
  {
    if (--bsDisplay < 0) bsDisplay = 3;
  }
  else if (++bsDisplay > 3) bsDisplay = 0;
}



void BSDetectionWidget::drawPoints (QPainter &painter,
                                    vector<Pt2i> pts, QColor color)
{
  vector<Pt2i>::iterator iter = pts.begin ();
  while (iter != pts.end ())
  {
    Pt2i p = *iter++;
    painter.setPen (QPen (color, DEFAULT_PEN_WIDTH, Qt::SolidLine,
                          Qt::RoundCap, Qt::RoundJoin));
    if (p.x() < width && p.y() < height && p.x() >= 0 && p.y() >= 0)
      painter.drawPoint (QPoint (p.x(), height - 1 - p.y()));  // dec 1
  }
}


void BSDetectionWidget::drawPixels (QPainter &painter, vector<Pt2i> pix)
{
  vector<Pt2i>::iterator iter = pix.begin ();
  while (iter != pix.end ())
  {
    Pt2i p = *iter++;
    painter.setPen (QPen (QBrush (loadedImage.pixel (p.x (),
                                  loadedImage.height () - 1 - p.y ())),
                          DEFAULT_PEN_WIDTH, Qt::SolidLine,
                          Qt::RoundCap, Qt::RoundJoin));
    if (p.x() < width && p.y() < height && p.x() >= 0 && p.y() >= 0)
      painter.drawPoint (QPoint (p.x(), height - 1 - p.y()));  // dec 1
  }
}


void BSDetectionWidget::drawLine (QPainter &painter,
                                  const Pt2i from, const Pt2i to, QColor color)
{
  int n;
  Pt2i *pts = from.drawing (to, &n);
  painter.setPen (QPen (color, DEFAULT_PEN_WIDTH, Qt::SolidLine,
                        Qt::RoundCap, Qt::RoundJoin));
  for (int i = 0; i < n; i++)
    painter.drawPoint (QPoint (pts[i].x (),
                               height - 1 - pts[i].y ()));  // dec 1
  delete [] pts;
}


void BSDetectionWidget::drawSelection (QPainter &painter,
                                       const Pt2i from, const Pt2i to)
{
  drawLine (painter, from, to, selectionColor);
}


void BSDetectionWidget::drawBlurredSegment (QPainter &painter,
                                            BlurredSegment *bs, bool high)
{
  if (bs != NULL)
  {
    if (bsDisplay >= 2) // Bounds
    {
      vector<Pt2i> bnd;
      DigitalStraightSegment *dss = bs->getSegment ();
      if (dss != NULL)
      {
        dss->getBounds (bnd, 0, 0, width, height);
        drawPoints (painter, bnd,
                    (high && bsDisplay == 2 ? bsHighColor[bsColor]
                                            : bsLowColor[bsColor]));
      }
    }
    if (bsDisplay % 2 == 1) // Points
      drawPoints (painter, bs->getAllPoints (),
                  high ? bsHighColor[bsColor] : bsLowColor[bsColor]);
  }
}


void BSDetectionWidget::drawArlequinSegment (QPainter &painter,
                                             BlurredSegment *bs)
{
  bool nok = true;
  int red, green, blue;
  while (nok)
  {
    red = rand () % 256;
    green = rand () % 256;
    blue = rand () % 256;
    nok = (arlequin == 1 && (red + green + blue) > 300)
          || (arlequin == -1 && (red + green + blue) < 300);
  }
  if (bs != NULL)
  {
    if (bsDisplay >= 2) // Bounds
    {
      vector<Pt2i> bnd;
      DigitalStraightSegment *dss = bs->getSegment ();
      if (dss != NULL)
      {
        dss->getBounds (bnd, 0, 0, width, height);
        if (bsDisplay == 3)
          drawPoints (painter, bnd, (arlequin == 1 ? Qt::black : Qt::white));
        else
          drawPoints (painter, bnd, QColor (red, green, blue));
      }
    }
    if (bsDisplay % 2 == 1) // Points
      drawPoints (painter, bs->getAllPoints (), QColor (red, green, blue));
  }
}


void BSDetectionWidget::incBlackLevel (int val)
{
  blevel += val * 5;
  if (blevel < 0) blevel = 0;
  if (blevel > 200) blevel = 200;
}


void BSDetectionWidget::lighten (QImage &im)
{
  if (blevel != 0 && background != BACK_BLACK && background != BACK_WHITE)
  {
    for (int i = 0; i < im.height (); i++)
      for(int j = 0; j < im.width (); j++)
      {
        int col = blevel + (QColor (im.pixel(j,i)).value ()
                            * (255 - blevel)) / 255;
        im.setPixel (j, i, col + col * 256 + col * 256 * 256);
      }
  }
}


void BSDetectionWidget::displayBackground ()
{
  if (background == BACK_BLACK) augmentedImage.fill (qRgb (0, 0, 0));
  else if (background == BACK_WHITE) augmentedImage.fill (qRgb (255, 255, 255));
  else if (background == BACK_IMAGE) augmentedImage = loadedImage;
  else augmentedImage = gradImage;
  QPainter painter (&augmentedImage);
  update (QRect (QPoint (0, 0), QPoint (width, height)));
}


void BSDetectionWidget::writeDetectionResult ()
{
  vector<BlurredSegment *> bss = detector.getBlurredSegments ();
  if (bss.empty ())
  {
    BlurredSegment *bs = detector.getBlurredSegment ();
    if (bs != NULL)
    {
      ofstream outf ("seg.txt", ios::out);
      outf << "(" << bs->antipodalEdgeStart().x ()
           << ", " << bs->antipodalEdgeStart().y ()
           << ") (" << bs->antipodalEdgeEnd().x ()
           << ", " << bs->antipodalEdgeEnd().y ()
           << ") (" << bs->antipodalVertex().x ()
           << ", " << bs->antipodalVertex().y ()
           << ") (" << bs->getLastLeft().x ()
           << ", " << bs->getLastLeft().y ()
           << ") (" << bs->getLastRight().x ()
           << ", " << bs->getLastRight().y ()
           << ")" << endl;
      outf.close ();
    }
  }
  else
  {
    ofstream outf ("segs.txt", ios::out);
    vector<BlurredSegment *>::iterator it = bss.begin ();
    while (it != bss.end ())
    {
      outf << "(" << (*it)->antipodalEdgeStart().x ()
           << ", " << (*it)->antipodalEdgeStart().y ()
           << ") (" << (*it)->antipodalEdgeEnd().x ()
           << ", " << (*it)->antipodalEdgeEnd().y ()
           << ") (" << (*it)->antipodalVertex().x ()
           << ", " << (*it)->antipodalVertex().y ()
           << ") (" << (*it)->getLastLeft().x ()
           << ", " << (*it)->getLastLeft().y ()
           << ") (" << (*it)->getLastRight().x ()
           << ", " << (*it)->getLastRight().y ()
           << ")" << endl;
      it++;
    }
    outf.close ();
  }
}


void BSDetectionWidget::displayDetectionResult ()
{
  if (background == BACK_BLACK) augmentedImage.fill (qRgb (0, 0, 0));
  else if (background == BACK_WHITE) augmentedImage.fill (qRgb (255, 255, 255));
  else if (background == BACK_IMAGE) augmentedImage = loadedImage;
  else augmentedImage = gradImage;
  lighten (augmentedImage);
  QPainter painter (&augmentedImage);
  vector<BlurredSegment *> bss = detector.getBlurredSegments ();
  if (! bss.empty ())
  {
    if (arlequin != 0) srand (time (NULL));
    vector<BlurredSegment *>::const_iterator it = bss.begin ();
    while (it != bss.end ())
    {
      if (arlequin != 0) drawArlequinSegment (painter, *it);
      else
        drawBlurredSegment (painter, *it,
                 detector.getMaxDetections () == 0 || *it == bss.back ());
      it++;
    }
  }
  else drawBlurredSegment (painter, detector.getBlurredSegment ());
  if (udef) drawSelection (painter, p1, p2);
  arlequin = 0;
  update (QRect (QPoint (0, 0), QPoint (width, height)));

  // Update auxiliary view if not dragging
  if (nodrag)
  {
    if (verbose) writeDetectionStatus ();
    if (statsOn) writeStats ();
  }
}


void BSDetectionWidget::displaySavedSegments ()
{
  if (background == BACK_BLACK) augmentedImage.fill (qRgb (0, 0, 0));
  else if (background == BACK_WHITE) augmentedImage.fill (qRgb (255, 255, 255));
  else if (background == BACK_IMAGE) augmentedImage = loadedImage;
  else augmentedImage = gradImage;
  lighten (augmentedImage);
  QPainter painter (&augmentedImage);
  if (! extractedSegments.empty ())
  {
    vector<ExtractedSegment>::iterator it = extractedSegments.begin ();
    while (it != extractedSegments.end ())
    {
      drawBlurredSegment (painter, it->bs);
      drawSelection (painter, it->p1, it->p2);
      it ++;
    }
  }
  update (QRect (QPoint (0, 0), QPoint (width, height)));
}


int BSDetectionWidget::saveExtractedSegment ()
{
  int count = 0;
  vector<BlurredSegment *> bss = detector.getBlurredSegments ();
  if (bss.empty ())
  {
    BlurredSegment *bs = detector.getBlurredSegment ();
    if (bs != NULL)
    {
      ExtractedSegment es;
      es.bs = bs;
      es.p1 = p1;
      es.p2 = p2;
      extractedSegments.push_back (es);
      detector.preserveFormerBlurredSegment ();
      count = 1;
    }
  }
  else
  {
    vector<BlurredSegment *>::const_iterator it = bss.begin ();
    while (it != bss.end ())
    {
      ExtractedSegment es;
      es.bs = *it++;
      es.p1 = p1;
      es.p2 = p2;
      extractedSegments.push_back (es);
    }
    detector.preserveFormerBlurredSegments ();
    count = (int) (bss.size ());
  }
  return count;
}


void BSDetectionWidget::clearSavedSegments ()
{
  vector<ExtractedSegment>::iterator it = extractedSegments.begin ();
  while (it != extractedSegments.end ())
    delete ((it++)->bs->getSegment ());
  extractedSegments.clear ();
}


void BSDetectionWidget::writeStats ()
{
  int longEdgeThreshold = 1600;

  vector<BlurredSegment *> bss = detector.getBlurredSegments ();
  if (bss.empty ())
  {
    BlurredSegment *bs = detector.getBlurredSegment ();
    if (bs != NULL)
    {
      Pt2i ptb = bs->getLastRight ();
      Pt2i ptf = bs->getLastLeft ();
      double length = sqrt ((ptb.x () - ptf.x ()) * (ptb.x () - ptf.x ())
                              + (ptb.y () - ptf.y ()) * (ptb.y () - ptf.y ()));
      cout << "Length : " << length << endl;
      DigitalStraightSegment *dss = bs->getSegment ();
      if (dss != NULL)
      {
        double width = dss->width () / (double) (dss->period ());
        cout << "Width : " << width << endl;
      }
      else cout << "DSS null" << endl;
    }
  }
  else
  {
    int nbdssnull = 0;
    double ltotal = 0.;
    int lcount = 0;
    double wtotal = 0.;
    vector<BlurredSegment *>::iterator it = bss.begin ();
    while (it != bss.end ())
    {
      Pt2i ptb = (*it)->getLastRight ();
      Pt2i ptf = (*it)->getLastLeft ();
      int length2 = (ptb.x () - ptf.x ()) * (ptb.x () - ptf.x ())
                    + (ptb.y () - ptf.y ()) * (ptb.y () - ptf.y ());
      double length = sqrt (length2);
      ltotal += length;
      if (length2 > longEdgeThreshold) lcount ++;
      DigitalStraightSegment *dss = (*it)->getSegment ();
      if (dss != NULL)
      {
        double width = dss->width () / (double) (dss->period ());
        wtotal += width * length;
      }
      else nbdssnull ++;
      it ++;
    }

    cout << bss.size () << " blurred segments detected on "
         << detector.countOfTrials () << " trials " << endl;
    cout << lcount << " long (>40) blurred segments detected" << endl;
    cout << "Mean length : " << ltotal / bss.size () << endl;
    cout << "Mean width (per unit length) : " << wtotal / ltotal << endl;
    if (nbdssnull != 0) cout << nbdssnull << " DSS null" << endl;
  }
}


void BSDetectionWidget::writeDetectionStatus ()
{
  int res = detector.result ();
  if (res == BSDetector::RESULT_UNDETERMINED)
    cout << "Extraction : undetermined." << endl;
  else if (res == BSDetector::RESULT_OK)
    cout << "Extraction : OK." << endl;
  else if (res == BSDetector::RESULT_PRELIM_NO_DETECTION)
    cout << "Extraction : no preliminary detection (bs0 == NULL)." << endl;
  else if (res == BSDetector::RESULT_PRELIM_TOO_FEW)
    cout << "Extraction : two few points at preliminary detection." << endl;
  else if (res == BSDetector::RESULT_INITIAL_NO_DETECTION)
    cout << "Extraction : no initial detection (bsini == NULL)." << endl;
  else if (res == BSDetector::RESULT_INITIAL_TOO_FEW)
    cout << "Extraction : two few points at initial detection." << endl;
  else if (res == BSDetector::RESULT_INITIAL_TOO_SPARSE)
    cout << "Extraction : unsuccessful density test at initial detection."
         << endl;
  else if (res == BSDetector::RESULT_INITIAL_TOO_MANY_OUTLIERS)
    cout << "Extraction : unsuccessful filter test at initial detection."
         << endl;
  else if (res == BSDetector::RESULT_INITIAL_CLOSE_ORIENTATION)
    cout << "Extraction : initial detection of a too closely oriented segment."
         << endl;
  else if (res == BSDetector::RESULT_FINAL_NO_DETECTION)
    cout << "Extraction : no final detection (bsini == NULL)." << endl;
  else if (res == BSDetector::RESULT_FINAL_TOO_FEW)
    cout << "Extraction : unsuccessful size test at final detection." << endl;
  else if (res == BSDetector::RESULT_FINAL_TOO_SPARSE)
    cout << "Extraction : unsuccessful sparsity test at final detection."
         << endl;
  else if (res == BSDetector::RESULT_FINAL_TOO_SMALL)
    cout << "Extraction : unsuccessful size test at final detection."
         << endl;
  else if (res == BSDetector::RESULT_FINAL_TOO_FRAGMENTED)
    cout << "Extraction : unsuccessful fragmentation test at final detection."
         << endl;
  else if (res == BSDetector::RESULT_FINAL_TOO_MANY_OUTLIERS)
    cout << "Extraction : unsuccessful filter test at final detection."
         << endl;
}


void BSDetectionWidget::saveMask ()
{
      QImage mim (width, height, QImage::Format_RGB32);
      bool *mask = gMap->getMask ();
      int nb = 0;
      for (int j = 0; j < height; j++)
        for (int i = 0; i < width; i++)
        {
          if (*mask) nb++;
          mim.setPixel (i, height - 1 - j,
                        *mask++ ? 0 : 255 + 255 * 256 + 255 * 256 * 256);
        }
      mim.save ("mask.png");
      cout << "mask.png created with " << nb << " points" << endl;
}


void BSDetectionWidget::extract ()
{
  if (udef)
  {
    if (p1.equals (p2))
    {
      displayBackground ();
      return;
    }
    detector.detectSelection (p1, p2);
  }
  else detector.detectAll ();
  displayDetectionResult ();
}


void BSDetectionWidget::alternateTest ()
{
  int longEdgeThreshold = 1600;
  int nbruns = 100;
  double diff1 = 0.;
  int nbdssnull = 0;
  double ltotal = 0.;
  double lltotal = 0.;
  int lcount = 0;
  double wtotal = 0.;
  double lwtotal = 0.;
  vector<BlurredSegment *> bss;
  vector<BlurredSegment *>::iterator it;

  ofstream outf ("perf.txt", ios::out);
  outf << nbruns << endl;
  outf << width << endl;
  outf << height << endl;

  detector.setStaticDetector (false);
  cout << "Performance test on new detector" << endl;
  clock_t start = clock ();
  for (int i = 0; i < nbruns; i++) detector.detectAll ();
  diff1 = (clock () - start) / (double) CLOCKS_PER_SEC;
  outf << diff1 << endl;

  detector.detectAll ();
  bss = detector.getBlurredSegments ();
  it = bss.begin ();
  while (it != bss.end ())
  {
    Pt2i ptb = (*it)->getLastRight ();
    Pt2i ptf = (*it)->getLastLeft ();
    int length2 = (ptb.x () - ptf.x ()) * (ptb.x () - ptf.x ())
                  + (ptb.y () - ptf.y ()) * (ptb.y () - ptf.y ());
    double length = sqrt (length2);
    ltotal += length;
    if (length2 > longEdgeThreshold)
    {
      lcount ++;
      lltotal += length;
    }
    DigitalStraightSegment *dss = (*it)->getSegment ();
    if (dss != NULL)
    {
      double width = dss->width () / (double) (dss->period ());
      wtotal += width * length;
      if (length2 > longEdgeThreshold) lwtotal += width * length;
    }
    else nbdssnull ++;
    it ++;
  }
  outf << detector.countOfTrials () << endl;
  outf << bss.size () << endl;
  outf << lcount << endl;;
  outf << ltotal / bss.size () << endl;
  outf << wtotal / ltotal << endl;
  outf << lwtotal / lltotal << endl;
  if (nbdssnull != 0) cout << nbdssnull << " DSS null" << endl;

  diff1 = 0.;
  nbdssnull = 0;
  ltotal = 0.;
  lltotal = 0.;
  lcount = 0;
  wtotal = 0.;
  lwtotal = 0.;
  detector.setStaticDetector (true);
  cout << "Performance test on old detector" << endl;
  start = clock ();
  for (int i = 0; i < nbruns; i++) detector.detectAll ();
  diff1 = (clock () - start) / (double) CLOCKS_PER_SEC;
  outf << diff1 << endl;

  detector.detectAll ();
  bss = detector.getBlurredSegments ();
  it = bss.begin ();
  while (it != bss.end ())
  {
    Pt2i ptb = (*it)->getLastRight ();
    Pt2i ptf = (*it)->getLastLeft ();
    int length2 = (ptb.x () - ptf.x ()) * (ptb.x () - ptf.x ())
                  + (ptb.y () - ptf.y ()) * (ptb.y () - ptf.y ());
    double length = sqrt (length2);
    ltotal += length;
    if (length2 > longEdgeThreshold)
    {
      lcount ++;
      lltotal += length;
    }
    DigitalStraightSegment *dss = (*it)->getSegment ();
    if (dss != NULL)
    {
      double width = dss->width () / (double) (dss->period ());
      wtotal += width * length;
      if (length2 > longEdgeThreshold) lwtotal += width * length;
    }
    else nbdssnull ++;
    it ++;
  }
  outf << detector.countOfTrials () << endl;
  outf << bss.size () << endl;
  outf << lcount << endl;
  outf << ltotal / bss.size () << endl;
  outf << wtotal / ltotal << endl;
  outf << lwtotal / lltotal << endl;
  if (nbdssnull != 0) cout << nbdssnull << " DSS null" << endl;

  outf.close ();
  cout << "Selection saved in perf.txt" << endl;
} 



void BSDetectionWidget::performanceTest ()
{
  if (p1.equals (p2))
  {
    // No stroke -> automatic
    cout << "Automatic extraction test" << endl;
    clock_t start = clock ();
    for (int i = 0; i < 100; i++) detector.detectAll ();
    double diff = (clock () - start) / (double) CLOCKS_PER_SEC;
    cout << "Test run : " << diff << endl;
    extract ();
  }
  else
  {
    udef = true;
    cout << "Run test" << endl;
    clock_t start = clock ();
    for (int i = 0; i < 1000; i++) detector.detect (p1, p2);
    double diff = (clock () - start) / (double) CLOCKS_PER_SEC;
    cout << "Test run : " << diff << endl;
    extract ();
  }
}


void BSDetectionWidget::storeUserInput ()
{
  if (udef && ! p1.equals (p2))
  {
    ofstream output ("test.txt", ios::out);
    output << p1.x () << " " << p1.y () << endl;
    output << p2.x () << " " << p2.y () << endl;
    cout << "New test.txt created" << endl;
  }
  else cout << "Unable to create test.txt : no user input" << endl;
}


void BSDetectionWidget::localTest ()
{
  int val[4], i = 0;
  ifstream input ("test.txt", ios::in);
  bool reading = true;
  if (input)
  {
    while (reading)
    {
      input >> val[i];
      if (input.eof ()) reading = false;
      else if (++i == 4) reading = false;
    }
    if (i == 4)
    {
      udef = true;
      p1 = Pt2i (val[0], val[1]);
      p2 = Pt2i (val[2], val[3]);
      cout << "Run test on (" << val[0] << ", " << val[1]
           << ") (" << val[2] << ", " << val[3] << ")" << endl;
    }
    else cout << "Test file damaged" << endl;
  }
  else cout << "Run autotest" << endl;

  detector.resetMaxDetections ();
  extract ();
  cout << "Test run" << endl;
}
