// #include <iostream>
#include "strucel.h"

using namespace std;


const int Strucel::TYPE_PLUS_3X3 = 0;
const int Strucel::TYPE_HOR = 1;
const int Strucel::TYPE_VER = 2;


Strucel::Strucel (int type)
{
  if (type == TYPE_PLUS_3X3)
  {
    width = 3;
    height = 3;
    size = 5;
    pattern = new Vr2i[5];
    pattern[0] = Vr2i (0, 0);
    pattern[1] = Vr2i (0, 1);
    pattern[2] = Vr2i (-1, 0);
    pattern[3] = Vr2i (1, 0);
    pattern[4] = Vr2i (0, -1);
  }
  else if (type == TYPE_HOR)
  {
    width = 3;
    height = 1;
    size = 3;
    pattern = new Vr2i[3];
    pattern[0] = Vr2i (0, 0);
    pattern[1] = Vr2i (0, 1);
    pattern[2] = Vr2i (0, -1);
  }
  else if (type == TYPE_VER)
  {
    width = 1;
    height = 3;
    size = 3;
    pattern = new Vr2i[3];
    pattern[0] = Vr2i (0, 0);
    pattern[1] = Vr2i (1, 0);
    pattern[2] = Vr2i (-1, 0);
  }
  else
  {
    width = 1;
    height = 1;
    size = 1;
    pattern = new Vr2i[1];
    pattern[0] = Vr2i (1, 1);
  }
}


Strucel::~Strucel ()
{
  delete pattern;
}


void Strucel::tophatGradient (int *out, int **in, int width, int height)
{
// cout << "TH IN" << endl;
  for (int j = 0; j < height; j++)
    for (int i = 0; i < width; i++)
    {
      int min = -1;
      for (int k = 0; k < size; k++)
      {
        int x = i - pattern[k].x ();
        int y = j - pattern[k].y ();
        if (x >= 0 && x < width && y >= 0 && y < height)
          if (min == -1 || in[y][x] < min) min = in[y][x];
      }
      out[j * width + i] = in[j][i] - min;
    }
// cout << "TH OUT" << endl;
}


void Strucel::tophatGradient (int *out, int *in, int width, int height)
{
  for (int j = 0; j < height; j++)
    for (int i = 0; i < width; i++)
    {
      int min = -1;
      for (int k = 0; k < size; k++)
      {
        int x = i - pattern[k].x ();
        int y = j - pattern[k].y ();
        if (x >= 0 && x < width && y >= 0 && y < height)
          if (min == -1 || in[y * width + x] < min) min = in[y * width + x];
      }
      out[j * width + i] = in[j * width + i] - min;
    }
}


void Strucel::blackhatGradient (int *out, int **in, int width, int height)
{
// cout << "BH IN" << endl;
  for (int j = 0; j < height; j++)
    for (int i = 0; i < width; i++)
    {
      int max = 0;
      for (int k = 0; k < size; k++)
      {
        int x = i - pattern[k].x ();
        int y = j - pattern[k].y ();
        if (x >= 0 && x < width && y >= 0 && y < height)
          if (in[y][x] > max) max = in[y][x];
      }
      out[j * width + i] = max - in[j][i];
    }
// cout << "BH OUT" << endl;
}


void Strucel::blackhatGradient (int *out, int *in, int width, int height)
{
  for (int j = 0; j < height; j++)
    for (int i = 0; i < width; i++)
    {
      int max = 0;
      for (int k = 0; k < size; k++)
      {
        int x = i - pattern[k].x ();
        int y = j - pattern[k].y ();
        if (x >= 0 && x < width && y >= 0 && y < height)
          if (in[y * width + x] > max) max = in[y * width + x];
      }
      out[j * width + i] = max - in[j * width + i];
    }
}


void Strucel::morphoGradient (int *out, int **in, int width, int height)
{
  for (int j = 0; j < height; j++)
    for (int i = 0; i < width; i++)
    {
      int max = 0;
      int min = -1;
      for (int k = 0; k < size; k++)
      {
        int x = i - pattern[k].x ();
        int y = j - pattern[k].y ();
        if (x >= 0 && x < width && y >= 0 && y < height)
        {
          if (min == -1 || in[y][x] < min) min = in[y][x];
          if (in[y][x] > max) max = in[y][x];
        }
      }
      out[j * width + i] = max - min;
    }
}


void Strucel::morphoGradient (int *out, int *in, int width, int height)
{
  for (int j = 0; j < height; j++)
    for (int i = 0; i < width; i++)
    {
      int max = 0;
      int min = -1;
      for (int k = 0; k < size; k++)
      {
        int x = i - pattern[k].x ();
        int y = j - pattern[k].y ();
        if (x >= 0 && x < width && y >= 0 && y < height)
        {
          if (min == -1 || in[y * width + x] < min) min = in[y * width + x];
          if (in[y * width + x] > max) max = in[y * width + x];
        }
      }
      out[j * width + i] = max - min;
    }
}
