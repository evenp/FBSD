#include <QApplication>
#include <string>
#include "bswindow.h"
#include "bsrandomtester.h"


int main (int argc, char *argv[])
{
  int val = 0;
  int imageName = 0;
  bool random = false, testing = false;
  QApplication app (argc, argv);

  BSWindow window (&val);   // val : necessary argument !
  for (int i = 1; i < argc; i++)
  {
    if (string(argv[i]).at(0) == '-')
    {
      if (string(argv[i]) == string ("-random")) random = true;
      else if (string(argv[i]) == string ("-test")) testing = true;
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
