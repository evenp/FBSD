#include <QApplication>
#include <string>
#include <cmath>
#include "bswindow.h"
#include "bsrandomtester.h"


int main (int argc, char *argv[])
{
  int val = 0;
  int imageName = 0;
  bool random = false, testing = false;
  bool out = false;
  QApplication app (argc, argv);

  BSWindow window (&val);   // val : necessary argument !
  for (int i = 1; i < argc; i++)
  {
    if (string(argv[i]).at(0) == '-')
    {
      if (string(argv[i]) == string ("-random")) random = true;
      else if (string(argv[i]) == string ("-test")) testing = true;
      else if (string(argv[i]) == string ("-out")) out = true;
      else if (string(argv[i]) == string ("-sobel3x3"))
        window.useGradient (VMap::TYPE_SOBEL_3X3);
      else if (string(argv[i]) == string ("-sobel5x5"))
        window.useGradient (VMap::TYPE_SOBEL_5X5);
      else if (string(argv[i]) == string ("-tophat"))
        window.useGradient (VMap::TYPE_TOP_HAT);
      else if (string(argv[i]) == string ("-blackhat"))
        window.useGradient (VMap::TYPE_BLACK_HAT);
      else if (string(argv[i]) == string ("-morpho"))
        window.useGradient (VMap::TYPE_MORPHO);
      else if (string(argv[i]) == string ("-fulltophat"))
        window.useGradient (VMap::TYPE_FULL_TOP_HAT);
      else if (string(argv[i]) == string ("-fullblackhat"))
        window.useGradient (VMap::TYPE_FULL_BLACK_HAT);
      else if (string(argv[i]) == string ("-fullmorpho"))
        window.useGradient (VMap::TYPE_FULL_MORPHO);
    }
    else imageName = i;
  }
  if (random)
  {
    BSRandomTester *tester = new BSRandomTester ();
    tester->randomTest ();
    delete tester;
    return (EXIT_SUCCESS);
  }
  else if (out)
  {
    QImage im;
    if (imageName != 0) im.load (argv[imageName]);
    else im.load ("Images/couloir.gif");
    int width = im.width ();
    int height = im.height ();
    int **tabImage = new int*[height];
    for (int i = 0; i < height; i++)
    {
      tabImage[i] = new int[width];
      for(int j = 0; j < width; j++)
      {
        QColor c = QColor (im.pixel (j, height - i - 1));
        tabImage[i][j] = c.value ();
      }
    }
    BSDetector detector;
    AbsRat x1, y1, x2, y2;
    VMap *gMap = NULL;
    if (gMap != NULL) delete gMap;
    gMap = new VMap (width, height, tabImage, VMap::TYPE_SOBEL_5X5);
    detector.setGradientMap (gMap);
    // buildGradientImage (0);
    detector.detectAll ();
    ofstream outf ("naivelines.txt", ios::out);
    vector<BlurredSegment *> bss = detector.getBlurredSegments ();
    vector<BlurredSegment *>::iterator it = bss.begin ();
    while (it != bss.end ())
    {
      if (*it != NULL)
      {
        DigitalStraightSegment *dss = (*it)->getSegment ();
        if (dss != NULL)
        {
          dss->naiveLine (x1, y1, x2, y2);
          AbsRat th = dss->squaredEuclideanThickness ();
          outf << (x1.num () / (double) x1.den ()) << " "
               << (height - 1 - y1.num () / (double) y1.den ()) << " "
               << (x2.num () / (double) x2.den ()) << " "
               << (height - 1 - y2.num () / (double) y2.den ()) << " "
               << sqrt (th.num () / (double) th.den ()) << endl;
        }
      }
      it ++;
    }
    outf.close ();
    return (EXIT_SUCCESS);
  }
  else
  {
    if (imageName != 0) window.setFile (QString (argv[imageName]));
    else window.setFile (QString ("Images/couloir.gif"));
    if (testing)
    {
      window.runTest ();
      return (EXIT_SUCCESS);
    }
    window.runOptions (); 
    window.show ();
    return app.exec ();
  }
}
