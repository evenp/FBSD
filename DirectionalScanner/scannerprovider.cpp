// #include <cstdlib>
// #include <iostream>
#include "scannerprovider.h"
#include "directionalscannero2.h"
#include "directionalscannero7.h"
#include "directionalscannero1.h"
#include "directionalscannero8.h"
#include "vhscannero2.h"
#include "vhscannero7.h"
#include "vhscannero1.h"
#include "vhscannero8.h"



DirectionalScanner *ScannerProvider::getScanner (Pt2i p1, Pt2i p2)
{
  // Enforces P1 to be lower than P2
  // or to left of P2 in cas of equality
  if ((p1.y () > p2.y ())
      || ((p1.y () == p2.y ()) && (p1.x () > p2.x ())))
  {
    Pt2i tmp = p1;
    p1 = p2;
    p2 = tmp;
  }

  // Computes the steps position array
  int nbs = 0;
  bool *steps = p1.stepsTo (p2, &nbs);

  // Equation of the strip support lines : ax + by = c
  int a = p2.x () - p1.x ();
  int b = p2.y () - p1.y ();
  if (a < 0 || (a == 0 && b < 0)) // Enforces a >= 0, then b > 0
  {
    a = -a;
    b = -b;
  }
  int c2 = a * p2.x () + b * p2.y ();

  // Builds and returns the appropriate scanner
  if (b < 0)
    if (-b > a)
    {
      if (isOrtho)
      {
        int repx = (p1.x () + p2.x ()) / 2;    // central scan start
        int repy = p1.y () - (int) ((p1.x () - repx) * (p1.x () - p2.x ())
                                    / (p2.y () - p1.y ()));
        return (new VHScannerO1 (xmin, ymin, xmax, ymax,
                                 a, b, c2, nbs, steps, repx, repy));
      }
      else return (new DirectionalScannerO1 (xmin, ymin, xmax, ymax,
                          a, b, c2, nbs, steps, p1.x (), p1.y ()));
    }
    else
    {
      if (isOrtho)
      {
        int repy = (p1.y () + p2.y ()) / 2;    // central scan start
        int repx = p1.x () + (int) ((repy - p1.y ()) * (p2.y () - p1.y ())
                                    / (p1.x () - p2.x ()));
        return (new VHScannerO2 (xmin, ymin, xmax, ymax,
                                 a, b, c2, nbs, steps, repx, repy));
      }
      else return (new DirectionalScannerO2 (xmin, ymin, xmax, ymax,
                          a, b, c2, nbs, steps, p1.x (), p1.y ()));
    }
  else
    if (b > a)
    {
      if (isOrtho)
      {
        int repx = (p1.x () + p2.x ()) / 2;    // central scan start
        int repy = p1.y () - (int) ((repx - p1.x ()) * (p2.x () - p1.x ())
                                    / (p2.y () - p1.y ()));
        return (new VHScannerO8 (xmin, ymin, xmax, ymax,
                                 a, b, c2, nbs, steps, repx, repy));
      }
      else return (new DirectionalScannerO8 (xmin, ymin, xmax, ymax,
                          a, b, c2, nbs, steps, p1.x (), p1.y ()));
    }
    else
    {
      if (isOrtho)
      {
        int repy = (p1.y () + p2.y ()) / 2;    // central scan start
        int repx = p1.x () - (int) ((repy - p1.y ()) * (p2.y () - p1.y ())
                                    / (p2.x () - p1.x ()));
        return (new VHScannerO7 (xmin, ymin, xmax, ymax,
                                 a, b, c2, nbs, steps, repx, repy));
      }
      else return (new DirectionalScannerO7 (xmin, ymin, xmax, ymax,
                          a, b, c2, nbs, steps, p1.x (), p1.y ()));
    }
}



DirectionalScanner *ScannerProvider::getScanner (Pt2i p1, Pt2i p2,
                                                 Pt2i v1, Pt2i v2)
{
  // Get the scan strip center
  int cx = (p1.x () + p2.x ()) / 2;
  int cy = (p1.y () + p2.y ()) / 2;
    
  // Gets the steps position array
  int nbs = 0;
  bool *steps = v1.stepsTo (v2, &nbs);

  // Equation of the straight line passing through the center : ax - by = mu
  int a = v2.y () - v1.y ();
  int b = v1.x () - v2.x ();
  if (a < 0 || (a == 0 && b < 0))
  {
    a = -a;
    b = -b;
  }

  // Equation of the support lines of the scan strip
  int c1 = a * p1.x () + b * p1.y ();
  int c2 = a * p2.x () + b * p2.y ();

  // Builds and returns the appropriate scanner
  if (b < 0)
    if (-b > a)
      return (new DirectionalScannerO1 (xmin, ymin, xmax, ymax,
                                        a, b, c1, c2, nbs, steps, cx, cy));
    else
      return (new DirectionalScannerO2 (xmin, ymin, xmax, ymax,
                                        a, b, c1, c2, nbs, steps, cx, cy));
  else
    if (b > a)
      return (new DirectionalScannerO8 (xmin, ymin, xmax, ymax,
                                        a, b, c1, c2, nbs, steps, cx, cy));
    else
      return (new DirectionalScannerO7 (xmin, ymin, xmax, ymax,
                                        a, b, c1, c2, nbs, steps, cx, cy));
}


DirectionalScanner *ScannerProvider::getScanner (Pt2i centre, Vr2i normal,
                                                 int length)
{
  // Gets the steps position array
  int nbs = 0;
  bool *steps = normal.steps (&nbs);

  // Orients the direction rightwards
  int a = normal.x ();
  int b = normal.y ();  // as equation is (ax - by)
  if (a < 0 || (a == 0 && b < 0))
  {
    a = -a;
    b = -b;
  }

  // Builds and returns the appropriate scanner
  if (b < 0)
    if (-b > a)
      return (new DirectionalScannerO1 (xmin, ymin, xmax, ymax,
                                        a, b, nbs, steps,
                                        centre.x (), centre.y (), length));
    else
      return (new DirectionalScannerO2 (xmin, ymin, xmax, ymax,
                                        a, b, nbs, steps,
                                        centre.x (), centre.y (), length));
  else
    if (b > a)
      return (new DirectionalScannerO8 (xmin, ymin, xmax, ymax,
                                        a, b, nbs, steps,
                                        centre.x (), centre.y (), length));
    else
      return (new DirectionalScannerO7 (xmin, ymin, xmax, ymax,
                                        a, b, nbs, steps,
                                        centre.x (), centre.y (), length));
}


DirectionalScanner *ScannerProvider::getScanner (Pt2i centre, Vr2i normal,
                                                 int length, bool controlable)
{
  // Gets the steps position array
  int nbs = 0;
  bool *steps = normal.steps (&nbs);

  // Orients the direction rightwards
  int a = normal.x ();
  int b = normal.y ();  // as equation is (ax - by)
  if (a < 0 || (a == 0 && b < 0))
  {
    a = -a;
    b = -b;
  }

  
  if (b < 0)
    if (-b > a)
      return (controlable ?
              (isOrtho ?
               (DirectionalScanner *)
               new VHScannerO1 (xmin, ymin, xmax, ymax,
                                a, b, nbs, steps,
                                centre.x (), centre.y (), length) :
               (DirectionalScanner *)
               new AdaptiveScannerO1 (xmin, ymin, xmax, ymax,
                                      a, b, nbs, steps,
                                      centre.x (), centre.y (), length)) :
              (DirectionalScanner *)
              new DirectionalScannerO1 (xmin, ymin, xmax, ymax,
                                        a, b, nbs, steps,
                                        centre.x (), centre.y (), length));
    else
      return (controlable ?
              (isOrtho ?
               (DirectionalScanner *)
               new VHScannerO2 (xmin, ymin, xmax, ymax,
                                a, b, nbs, steps,
                                centre.x (), centre.y (), length) :
               (DirectionalScanner *)
               new AdaptiveScannerO2 (xmin, ymin, xmax, ymax,
                                      a, b, nbs, steps,
                                      centre.x (), centre.y (), length)) :
              (DirectionalScanner *)
              new DirectionalScannerO2 (xmin, ymin, xmax, ymax,
                                        a, b, nbs, steps,
                                        centre.x (), centre.y (), length));
  else
    if (b > a)
      return (controlable ?
              (isOrtho ?
               (DirectionalScanner *)
               new VHScannerO8 (xmin, ymin, xmax, ymax,
                                a, b, nbs, steps,
                                centre.x (), centre.y (), length) :
               (DirectionalScanner *)
               new AdaptiveScannerO8 (xmin, ymin, xmax, ymax,
                                      a, b, nbs, steps,
                                      centre.x (), centre.y (), length)) :
              (DirectionalScanner *)
              new DirectionalScannerO8 (xmin, ymin, xmax, ymax,
                                        a, b, nbs, steps,
                                        centre.x (), centre.y (), length));
    else
      return (controlable ?
              (isOrtho ?
               (DirectionalScanner *)
               new VHScannerO7 (xmin, ymin, xmax, ymax,
                                a, b, nbs, steps,
                                centre.x (), centre.y (), length) :
               (DirectionalScanner *)
               new AdaptiveScannerO7 (xmin, ymin, xmax, ymax,
                                      a, b, nbs, steps,
                                      centre.x (), centre.y (), length)) :
              (DirectionalScanner *)
              new DirectionalScannerO7 (xmin, ymin, xmax, ymax,
                                        a, b, nbs, steps,
                                        centre.x (), centre.y (), length));
}
