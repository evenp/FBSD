#include <QImage>
#include <QString>
#include <QColor>
#include "bsdetector.h"
#include "vmap.h"

#include <iostream>
#include <fstream>



void usage(std::string str)
{
  std::cout << str << " : something is wrong with the prog parameter (not enough parameters or something wrong)... "<< std::endl;  

}



int main (int argc, char *argv[])
{
  for (int i = 1; i < argc; i++)
  {
    if (string(argv[i]) == "--version")
    {
       BSDetector detector;
       std::cout << detector.version()<< std::endl;
       return 0;
    }
  }

  if (argc < 4)
  {
    usage(argv[0]);
    exit(EXIT_FAILURE);
  }

      
    
  // getting parameters:
  // - input image file
  string input_filename = argv[1];
  // - input points file
  string inpoints_filename = "";
  ifstream fin;  
  bool with_points = false;
  double widthBS = atof(argv[3]);
  if (argc == 5)
  {
    inpoints_filename = argv[4];
    fin.open (inpoints_filename.c_str(), std::fstream::in);
    with_points = true;
  }
  // - output file
  string output_filename = argv[2];
  ofstream fout;  
  fout.open (output_filename.c_str(), std::fstream::out);
  ofstream foutAllPts;
  foutAllPts.open("segmentsPoints.dat",std::fstream::out);
  
  // Gradient map extraction (uses qt)
  QImage image;
  
  image.load (QString (input_filename.c_str()));
  int width = image.width ();
  int height = image.height ();
  int **tabImage = new int*[height];
  for (int i = 0; i < height; i++)
  {
    tabImage[i] = new int[width];
    for(int j = 0; j < width; j++)
    {
      QColor c = QColor (image.pixel (j, height - i - 1));
      tabImage[i][j] = c.value ();
    }
  }
  VMap *gMap = new VMap (width, height, tabImage, VMap::TYPE_SOBEL_5X5);

  // Input points reading (uses qt)
  vector<Pt2i> pts;
  if (with_points)
  {
    int val[4], i = 0;
    bool reading = true;
    if (fin)
    {
      while (reading)
      {
        if (fin.eof())
        {
          reading = false;
        }
        else
        {
          fin >> skipws >> val[i++];
        }
        //if (val[i-1] == -1) reading = false;
        //if (fin.eof()) continue;
        
        if (reading && i == 4)
        {
          pts.push_back (Pt2i (val[0], val[1]));
          pts.push_back (Pt2i (val[2], val[3]));
          i = 0;
        }
      }
    }
    fin.close();
  }
  fout << "# Line detection generated from " << argv[0] << "with format : X1 Y1 X2 Y2 on each line" << std::endl;

  // Blurred segment detection
  vector<BlurredSegment *> bss;
  BSDetector detector;
  detector.setGradientMap (gMap);
  detector.setAssignedThickness (widthBS);

  if (with_points)
  {
    vector<Pt2i>::iterator pit = pts.begin ();
    int nbseg = 0;
    while (pit != pts.end ())
    {
      Pt2i pt1 = *pit++;
      Pt2i pt2 = *pit++;
      std::cout << nbseg << " detect (" << pt1.x () << ", " << pt1.y ()
                << ") (" << pt2.x () << ", " << pt2.y () << ")" << std::endl;
      detector.detect (pt1, pt2);
      BlurredSegment *mybs = detector.getBlurredSegment ();
      if (mybs != NULL)
      {
        detector.preserveFormerBlurredSegment ();
        bss.push_back (mybs);
        nbseg ++;
      }
    }
  vector<BlurredSegment *>::const_iterator it = bss.begin ();
    while (it != bss.end ())
    {
      // Affichage du premier point
      vector<Pt2i> points = (*it)->getAllPoints ();
      
      fout <<  points.front().x() << " " << points.front().y() << " "
                <<  points.back().x() << " " << points.back().y() << std::endl;
      // Export pour l'affichage de tous les points d'un segments
      for(auto &p : points)
      {
        foutAllPts<< p.x() << " " << p.y() << " "; 
      }
      foutAllPts<< std::endl;
      
      // Affichage du DSS englobant
      // vector<Pt2i> bnd;
      // DigitalStraightSegment *dss = (*it)->getSegment ();
      // if (dss != NULL)
      // {
         
      //   dss->getBounds (bnd, 0, 0, width, height, false);
      //   // cout << "DSS starts from (" << bnd.front().x()
      //   //    << "," << bnd.front().y() << ")" << endl;
      //   auto b = bnd.begin();
      //   for (auto &x: bnd){
      //   cout << "DSS starts from (" << x.x()
      //        << "," << x.y() << ")" << endl;
          
      //   }
      // }

      it++;
    }
 
    std::cout << nbseg << " detections" << std::endl;
  }
  else
  {
    detector.detectAll ();
    bss = detector.getBlurredSegments ();
  }



  // Display
  if (! bss.empty ())
  {
    vector<BlurredSegment *>::const_iterator it = bss.begin ();
    while (it != bss.end ())
    {
      // Affichage du premier point
      vector<Pt2i> points = (*it)->getAllPoints ();
      
      fout <<  points.front().x() << " " << points.front().y() << " "
                <<  points.back().x() << " " << points.back().y() << std::endl;

      // Export pour l'affichage de tous les points d'un segments
      for(auto &p : points)
      {
        foutAllPts<< p.x() << " " << p.y() << " "; 
      }
      foutAllPts<< std::endl;

      // Affichage du DSS englobant
      // vector<Pt2i> bnd;
      // DigitalStraightSegment *dss = (*it)->getSegment ();
      // if (dss != NULL)
      // {
         
      //   dss->getBounds (bnd, 0, 0, width, height, false);
      //   // cout << "DSS starts from (" << bnd.front().x()
      //   //    << "," << bnd.front().y() << ")" << endl;
      //   auto b = bnd.begin();
      //   for (auto &x: bnd){
      //   cout << "DSS starts from (" << x.x()
      //        << "," << x.y() << ")" << endl;
          
      //   }
      // }

      it++;
    }
  }
  fout.close();
  foutAllPts.close();
  return (0);
}
