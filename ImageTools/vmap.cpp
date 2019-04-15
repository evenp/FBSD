// #include <iostream>
#include "vmap.h"
#include "math.h"

using namespace std;


const int VMap::TYPE_SOBEL_3X3 = 0;
const int VMap::TYPE_SOBEL_5X5 = 1;
const int VMap::TYPE_TOP_HAT = 2;
const int VMap::TYPE_BLACK_HAT = 3;
const int VMap::TYPE_MORPHO = 4;
const int VMap::TYPE_FULL_TOP_HAT = 5;
const int VMap::TYPE_FULL_BLACK_HAT = 6;
const int VMap::TYPE_FULL_MORPHO = 7;

const int VMap::NEAR_SQ_ANGLE = 80;  // 80% (roughly 25 degrees)
const int VMap::DEFAULT_GRADIENT_THRESHOLD = 20;
const int VMap::DEFAULT_GRADIENT_RESOLUTION = 100;

const int VMap::MAX_BOWL = 20;
const int VMap::NB_DILATIONS = 5;
const int VMap::DEFAULT_DILATION = 4;


VMap::VMap (int width, int height, int *data, int type)
{
  this->width = width;
  this->height = height;
  this->gtype = type;
  init ();
  imap = new int[width * height];
  if (type == TYPE_TOP_HAT)
  {
    Strucel se (Strucel::TYPE_PLUS_3X3);
    se.tophatGradient (imap, data, width, height);
    buildSobel5x5Map (data);
  }
  else if (type == TYPE_FULL_TOP_HAT)
  {
    Strucel se (Strucel::TYPE_PLUS_3X3);
    se.tophatGradient (imap, data, width, height);
    int *jmap = new int[width * height];
    Strucel seh (Strucel::TYPE_HOR);
    seh.tophatGradient (jmap, data, width, height);
    int *kmap = new int[width * height];
    Strucel sev (Strucel::TYPE_VER);
    sev.tophatGradient (kmap, data, width, height);
    map = new Vr2i[width * height];
    Vr2i *tmpmap = map;
    for (int i = 0; i < width * height; i ++)
    {
      tmpmap->set (jmap[i], kmap[i]);
      tmpmap ++;
    }
  }
  else if (type == TYPE_BLACK_HAT)
  {
    Strucel se (Strucel::TYPE_PLUS_3X3);
    se.blackhatGradient (imap, data, width, height);
    buildSobel5x5Map (data);
  }
  else if (type == TYPE_FULL_BLACK_HAT)
  {
    Strucel se (Strucel::TYPE_PLUS_3X3);
    se.blackhatGradient (imap, data, width, height);
    int *jmap = new int[width * height];
    Strucel seh (Strucel::TYPE_HOR);
    seh.blackhatGradient (jmap, data, width, height);
    int *kmap = new int[width * height];
    Strucel sev (Strucel::TYPE_VER);
    sev.blackhatGradient (kmap, data, width, height);
    map = new Vr2i[width * height];
    Vr2i *tmpmap = map;
    for (int i = 0; i < width * height; i ++)
    {
      tmpmap->set (jmap[i], kmap[i]);
      tmpmap ++;
    }
  }
  else if (type == TYPE_MORPHO)
  {
    Strucel se (Strucel::TYPE_PLUS_3X3);
    se.morphoGradient (imap, data, width, height);
    buildSobel5x5Map (data);
  }
  else if (type == TYPE_FULL_MORPHO)
  {
    Strucel se (Strucel::TYPE_PLUS_3X3);
    se.morphoGradient (imap, data, width, height);
    int *jmap = new int[width * height];
    Strucel seh (Strucel::TYPE_HOR);
    seh.morphoGradient (jmap, data, width, height);
    int *kmap = new int[width * height];
    Strucel sev (Strucel::TYPE_VER);
    sev.morphoGradient (kmap, data, width, height);
    map = new Vr2i[width * height];
    Vr2i *tmpmap = map;
    for (int i = 0; i < width * height; i ++)
    {
      tmpmap->set (jmap[i], kmap[i]);
      tmpmap ++;
    }
  }
  else if (type == TYPE_SOBEL_5X5)
  {
    buildSobel5x5Map (data);
    for (int i = 0; i < width * height; i++)
      imap[i] = (int) sqrt (map[i].norm2 ());
    gmagThreshold *= gradientThreshold;
  }
  else if (type == TYPE_SOBEL_3X3)
  {
    buildGradientMap (data);
    for (int i = 0; i < width * height; i++)
      imap[i] = (int) sqrt (map[i].norm2 ());
    gmagThreshold *= gradientThreshold;
  }
}


VMap::VMap (int width, int height, int **data, int type)
{
  this->width = width;
  this->height = height;
  this->gtype = type;
  init ();
  imap = new int[width * height];
  if (type == TYPE_TOP_HAT)
  {
    Strucel se (Strucel::TYPE_PLUS_3X3);
    se.tophatGradient (imap, data, width, height);
    buildSobel5x5Map (data);
  }
  else if (type == TYPE_FULL_TOP_HAT)
  {
    Strucel se (Strucel::TYPE_PLUS_3X3);
    se.tophatGradient (imap, data, width, height);
    int *jmap = new int[width * height];
    Strucel seh (Strucel::TYPE_HOR);
    seh.tophatGradient (jmap, data, width, height);
    int *kmap = new int[width * height];
    Strucel sev (Strucel::TYPE_VER);
    sev.tophatGradient (kmap, data, width, height);
    map = new Vr2i[width * height];
    Vr2i *tmpmap = map;
    for (int i = 0; i < width * height; i ++)
    {
      tmpmap->set (jmap[i], kmap[i]);
      tmpmap ++;
    }
  }
  else if (type == TYPE_BLACK_HAT)
  {
    Strucel se (Strucel::TYPE_PLUS_3X3);
    se.blackhatGradient (imap, data, width, height);
    buildSobel5x5Map (data);
  }
  else if (type == TYPE_FULL_BLACK_HAT)
  {
    Strucel se (Strucel::TYPE_PLUS_3X3);
    se.blackhatGradient (imap, data, width, height);
    int *jmap = new int[width * height];
    Strucel seh (Strucel::TYPE_HOR);
    seh.blackhatGradient (jmap, data, width, height);
    int *kmap = new int[width * height];
    Strucel sev (Strucel::TYPE_VER);
    sev.blackhatGradient (kmap, data, width, height);
    map = new Vr2i[width * height];
    Vr2i *tmpmap = map;
    for (int i = 0; i < width * height; i ++)
    {
      tmpmap->set (jmap[i], kmap[i]);
      tmpmap ++;
    }
  }
  else if (type == TYPE_MORPHO)
  {
    Strucel se (Strucel::TYPE_PLUS_3X3);
    se.morphoGradient (imap, data, width, height);
    buildSobel5x5Map (data);
  }
  else if (type == TYPE_FULL_MORPHO)
  {
    Strucel se (Strucel::TYPE_PLUS_3X3);
    se.morphoGradient (imap, data, width, height);
    int *jmap = new int[width * height];
    Strucel seh (Strucel::TYPE_HOR);
    seh.morphoGradient (jmap, data, width, height);
    int *kmap = new int[width * height];
    Strucel sev (Strucel::TYPE_VER);
    sev.morphoGradient (kmap, data, width, height);
    map = new Vr2i[width * height];
    Vr2i *tmpmap = map;
    for (int i = 0; i < width * height; i ++)
    {
      tmpmap->set (jmap[i], kmap[i]);
      tmpmap ++;
    }
  }
  else if (type == TYPE_SOBEL_5X5)
  {
    buildSobel5x5Map (data);
    for (int i = 0; i < width * height; i++)
      imap[i] = (int) sqrt (map[i].norm2 ());
    gmagThreshold *= gradientThreshold;
  }
  else if (type == TYPE_SOBEL_3X3)
  {
    buildGradientMap (data);
    for (int i = 0; i < width * height; i++)
      imap[i] = (int) sqrt (map[i].norm2 ());
    gmagThreshold *= gradientThreshold;
  }
}


VMap::~VMap ()
{
  delete [] map;
  delete [] imap;
  delete [] mask;
  delete [] dilations;
  delete [] bowl;
}


void VMap::init ()
{
  gradientThreshold = DEFAULT_GRADIENT_THRESHOLD;
  gmagThreshold = gradientThreshold;
  gradres = DEFAULT_GRADIENT_RESOLUTION;
  mask = new bool[width * height];
  for (int i = 0; i < width * height; i++) mask[i] = false;
  masking = false;
  angleThreshold = NEAR_SQ_ANGLE;
  orientedGradient = true;
  bowl = new Vr2i[MAX_BOWL];
  bowl[0] = Vr2i (1, 0);
  bowl[1] = Vr2i (0, 1);
  bowl[2] = Vr2i (-1, 0);
  bowl[3] = Vr2i (0, -1);
  bowl[4] = Vr2i (1, 1);
  bowl[5] = Vr2i (1, -1);
  bowl[6] = Vr2i (-1, -1);
  bowl[7] = Vr2i (-1, 1);
  bowl[8] = Vr2i (2, 0);
  bowl[9] = Vr2i (0, 2);
  bowl[10] = Vr2i (-2, 0);
  bowl[11] = Vr2i (0, -2);
  bowl[12] = Vr2i (2, 1);
  bowl[13] = Vr2i (1, 2);
  bowl[14] = Vr2i (-1, 2);
  bowl[15] = Vr2i (-2, 1);
  bowl[16] = Vr2i (-2, -1);
  bowl[17] = Vr2i (-1, -2);
  bowl[18] = Vr2i (1, -2);
  bowl[19] = Vr2i (2, -1);
  dilations = new int[NB_DILATIONS];
  dilations[0] = 0;
  dilations[1] = 4;
  dilations[2] = 8;
  dilations[3] = 12;
  dilations[4] = 20;
  maskDilation = DEFAULT_DILATION;
}


void VMap::buildGradientMap (int *data)
{
  map = new Vr2i[width * height];
  Vr2i *gm = map;

  for (int j = 0; j < width; j++)
  {
    gm->set (0, 0);
    gm++;
  }
  for (int i = 1; i < height - 1; i++)
  {
    gm->set (0, 0);
    gm++;
    for (int j = 1; j < width - 1; j++)
    {
      gm->set (data[(i - 1) * height + j + 1]
               + 2 * data[i * height + j + 1]
               + data[(i + 1) * height + j + 1]
               - data[(i - 1) * height + j - 1]
               - 2 * data[i * height + j - 1]
               - data[(i + 1) * height + j - 1],
               data[(i + 1) * height + j - 1]
               + 2 * data[(i + 1) * height + j]
               + data[(i + 1) * height + j + 1]
               - data[(i - 1) * height + j - 1]
               - 2 * data[(i - 1) * height + j]
               - data[(i - 1) * height + j + 1]);
      gm++;
    }
    gm->set (0, 0);
    gm++;
  }
  for (int j = 0; j < width; j++)
  {
    gm->set (0, 0);
    gm++;
  }
}


void VMap::buildGradientMap (int **data)
{
  map = new Vr2i[width * height];
  Vr2i *gm = map;

  for (int j = 0; j < width; j++)
  {
    gm->set (0, 0);
    gm++;
  }
  for (int i = 1; i < height - 1; i++)
  {
    gm->set (0, 0);
    gm++;
    for (int j = 1; j < width - 1; j++)
    {
      gm->set (data[i-1][j+1] + 2 * data[i][j+1] + data[i+1][j+1]
               - data[i-1][j-1] - 2 * data[i][j-1] - data[i+1][j-1],
               data[i+1][j-1] + 2 * data[i+1][j] + data[i+1][j+1]
               - data[i-1][j-1] - 2 * data[i-1][j] - data[i-1][j+1]);
      gm++;
    }
    gm->set (0, 0);
    gm++;
  }
  for (int j = 0; j < width; j++)
  {
    gm->set (0, 0);
    gm++;
  }
}


void VMap::buildSobel5x5Map (int *data)
{
  map = new Vr2i[width * height];
  Vr2i *gm = map;

  for (int j = 0; j < 2 * width; j++)
  {
    gm->set (0, 0);
    gm++;
  }
  for (int i = 2; i < height - 2; i++)
  {
    gm->set (0, 0);
    gm++;
    gm->set (0, 0);
    gm++;
    for (int j = 2; j < width - 2; j++)
    {
      gm->set (5 * data[(i - 2) * height + j + 2]
                 + 8 * data[(i - 1) * height + j + 2]
                 + 10 * data[i * height + j + 2]
                 + 8 * data[(i + 1) * height + j + 2]
                 + 5 * data[(i + 2) * height + j + 2]
               + 4 * data[(i - 2) * height + j + 1]
                 + 10 * data[(i - 1) * height + j + 1]
                 + 20 * data[i * height + j + 1]
                 + 10 * data[(i + 1) * height + j + 1]
                 + 4 * data[(i + 2) * height + j + 1]
               - 4 * data[(i - 2) * height + j - 1]
                 - 10 * data[(i - 1) * height + j - 1]
                 - 20 * data[i * height + j - 1]
                 - 10 * data[(i + 1) * height + j - 1]
                 - 4 * data[(i + 2) * height + j - 1] 
               - 5 * data[(i - 2) * height + j - 2]
                 - 8 * data[(i - 1) * height + j - 2]
                 - 10 * data[i * height + j - 2]
                 - 8 * data[(i + 1) * height + j - 2]
                 - 5 * data[(i + 2) * height + j - 2],
               5 * data[(i + 2) * height + j - 2]
                 + 8 * data[(i + 2) * height + j - 1]
                 + 10 * data[(i + 2) * height + j]
                 + 8 * data[(i + 2) * height + j + 1]
                 + 5 * data[(i + 2) * height + j + 2]
               + 4 * data[(i + 1) * height + j - 2]
                 + 10 * data[(i + 1) * height + j - 1]
                 + 20 * data[(i + 1) * height + j]
                 + 10 * data[(i + 1) * height + j + 1]
                 + 4 * data[(i + 1) * height + j + 2]
               - 4 * data[(i - 1) * height + j - 2]
                 - 10 * data[(i - 1) * height + j - 1]
                 - 20 * data[(i - 1) * height + j]
                 - 10 * data[(i - 1) * height + j + 1]
                 - 4 * data[(i - 1) * height + j + 2]
               - 5 * data[(i - 2) * height + j - 2]
                 - 8 * data[(i - 2) * height + j - 1]
                 - 10 * data[(i - 2) * height + j]
                 - 8 * data[(i - 2) * height + j + 1]
                 - 5 * data[(i - 2) * height + j + 2]);
      gm++;
    }
    gm->set (0, 0);
    gm++;
    gm->set (0, 0);
    gm++;
  }
  for (int j = 0; j < 2 * width; j++)
  {
    gm->set (0, 0);
    gm++;
  }
}


void VMap::buildSobel5x5Map (int **data)
{
  map = new Vr2i[width * height];
  Vr2i *gm = map;

  for (int j = 0; j < 2 * width; j++)
  {
    gm->set (0, 0);
    gm++;
  }
  for (int i = 2; i < height - 2; i++)
  {
    gm->set (0, 0);
    gm++;
    gm->set (0, 0);
    gm++;
    for (int j = 2; j < width - 2; j++)
    {
      gm->set (
        5 * data[i-2][j+2] + 8 * data[i-1][j+2] + 10 * data[i][j+2]
                           + 8 * data[i+1][j+2] + 5 * data[i+2][j+2]
        + 4 * data[i-2][j+1] + 10 * data[i-1][j+1] + 20 * data[i][j+1]
                           + 10 * data[i+1][j+1] + 4 * data[i+2][j+1]
        - 4 * data[i-2][j-1] - 10 * data[i-1][j-1] - 20 * data[i][j-1]
                           - 10 * data[i+1][j-1] - 4 * data[i+2][j-1]
        - 5 * data[i-2][j-2] - 8 * data[i-1][j-2] - 10 * data[i][j-2]
                           - 8 * data[i+1][j-2] - 5 * data[i+2][j-2],
        5 * data[i+2][j-2] + 8 * data[i+2][j-1] + 10 * data[i+2][j]
                           + 8 * data[i+2][j+1] + 5 * data[i+2][j+2]
        + 4 * data[i+1][j-2] + 10 * data[i+1][j-1] + 20 * data[i+1][j]
                           + 10 * data[i+1][j+1] + 4 * data[i+1][j+2]
        - 4 * data[i-1][j-2] - 10 * data[i-1][j-1] - 20 * data[i-1][j]
                           - 10 * data[i-1][j+1] - 4 * data[i-1][j+2]
        - 5 * data[i-2][j-2] - 8 * data[i-2][j-1] - 10 * data[i-2][j]
                           - 8 * data[i-2][j+1] - 5 * data[i-2][j+2]);
      gm++;
    }
    gm->set (0, 0);
    gm++;
    gm->set (0, 0);
    gm++;
  }
  for (int j = 0; j < 2 * width; j++)
  {
    gm->set (0, 0);
    gm++;
  }
}


int VMap::sqNorm (int i, int j) const
{
  return (map[j * width + i].norm2 ());
}


int VMap::sqNorm (Pt2i p) const
{
  return (map[p.y () * width + p.x ()].norm2 ());
}


int VMap::largestIn (const vector<Pt2i> &pix) const
{
  if (pix.empty ()) return (-1);

  int imax = -1;
  vector<Pt2i>::const_iterator pt = pix.begin ();
  int gmax = imap[pt->y() * width + pt->x()];
  if (gmax < gmagThreshold) gmax = gmagThreshold;

  int i = 0;
  while (pt != pix.end ())
  {
    int g = imap[pt->y() * width + pt->x()];
    if (g > gmax)
    {
      gmax = g;
      imax = i;
    }
    pt ++;
    i ++;
  }
  return (imax == i - 1 ? -1 : imax);
}


int VMap::keepFreeElementsIn (const vector<Pt2i> &pix, int n, int *ind) const
{
  int i = 0;
  while (i < n)
  {
    if (mask[pix[ind[i]].y () * width + pix[ind[i]].x ()]) ind[i] = ind[--n];
    else i++;
  }
  return (n);
}


int VMap::keepContrastedMax (int *lmax, int n, int *in) const
{
  int min[n-1];
  bool fired[n];
  int nbfired = 0;
  int sleft = 0;

  // Clears the list of fired max
  for (int i = 0; i < n; i++) fired[i] = false;

  // Computes the ponds depth
  for (int i = 0; i < n - 1; i++)
  {
    min[i] = in[lmax[i]];
    for (int j = lmax[i] + 1; j < lmax[i+1]; j++)
      if (in[j] < min[i]) min[i] = in[j];
  }
  // For each ponds
  for (int i = 0; i < n - 1; i++)
  {
    // if the right summit is lower
    if (in[lmax[i+1]] < in[lmax[sleft]])
    {
      if (in[lmax[i+1]] - min[i] < gradres) // gradient resolution
      {
        fired[i+1] = true;
        nbfired ++;
        if (i < n - 2) if (min[i+1] < min[i]) min[i+1] = min[i];
      }
    }
    // if the left summit is lower
    else
    {
      if (in[lmax[sleft]] - min[i] < gradres) // gradient resolution
      {
        fired[sleft] = true;
        nbfired ++;
        sleft = i + 1;
      }
    }
  }
  // Pruning
  int i = 0, j = 0;
  while (i < n && ! fired[i++]) j++;
  while (i < n)
  {
    while (i < n && fired[i]) i++;
    if (i < n) lmax[j++] = lmax[i++];
  }
  return (n - nbfired);
}


int VMap::keepDirectedElementsAs (const vector<Pt2i> &pix,
                                  int n, int *ind, const Vr2i &ref) const
{
  int vx = ref.x ();
  int vy = ref.y ();
  int i = 0;
  while (i < n)
  {
    Vr2i gr = map[pix[ind[i]].y () * width + pix[ind[i]].x ()];
    if (vx * gr.x () + vy * gr.y () <= 0) ind[i] = ind[--n];
    else i++;
  }
  return (n);
}


int VMap::keepOrientedElementsAs (const vector<Pt2i> &pix,
                                  int n, int *ind, const Vr2i &ref) const
{
  int vx = ref.x ();
  int vy = ref.y ();
  long vn2 = vx * vx + vy * vy;

  int i = 0;
  while (i < n)
  {
    Pt2i p = pix.at (ind[i]);
    Vr2i gr = map[p.y () * width + p.x ()];
    long gx = gr.x ();
    long gy = gr.y ();
    if ((vx * vx * gx * gx + vy * vy * gy * gy + 2 * vx * vy * gx * gy) * 100
        < vn2 * (gx * gx + gy * gy) * angleThreshold)
      ind[i] = ind[--n];
    else i++;
  }
  return (n);
}


int VMap::localMax (int *lmax, const vector<Pt2i> &pix) const
{
  // Builds the gradient norm signal
  int n = (int) pix.size ();
  int *gn = new int[n];
  int i = 0;
  vector<Pt2i>::const_iterator it = pix.begin ();
  while (it != pix.end ()) gn[i++] = magn (*it++);

  // Gets the local maxima
  int count = searchLocalMax (lmax, n, gn);

  // Prunes the low contrasted local maxima
  count = keepContrastedMax (lmax, count, gn);

  // Prunes the already selected candidates
  count = keepFreeElementsIn (pix, count, lmax);

  // Sorts candidates by gradient magnitude
  sortMax (lmax, count, gn);

  delete gn;
  return count;
}


int VMap::localMax (int *lmax, const vector<Pt2i> &pix, const Vr2i &gref) const
{
  // Builds the gradient norm signal
  int n = (int) pix.size ();
  int *gn = new int[n];
  int i = 0;
  vector<Pt2i>::const_iterator it = pix.begin ();
  while (it != pix.end ()) gn[i++] = magn (*it++);

  // Gets the local maxima
  int count = searchLocalMax (lmax, n, gn);

  // Prunes the already selected candidates
  if (masking)
    count = keepFreeElementsIn (pix, count, lmax);

  // Prunes the candidates with opposite gradient
  if (orientedGradient)
    count = keepDirectedElementsAs (pix, count, lmax, gref);

  // Prunes the candidates wrongly oriented
  count = keepOrientedElementsAs (pix, count, lmax, gref);

  // Sorts candidates by gradient magnitude
  sortMax (lmax, count, gn);

  delete gn;
  return count;
}


int VMap::searchLocalMax (int *lmax, int n, int *in) const
{
  int offset = 0;
  int count = 0;
  bool up = true;

  // Gets the first distinct value from start
  while (offset < n - 1 && in[offset] == in[0])
  {
    if (in[offset] - in[offset + 1] < 0)
    {
      up = true;
      break;
    }
    if (in[offset] - in[offset + 1] > 0)
    {
      up = false;
      break;
    }
    offset++;
  }

  for(int i = offset; i < n - 1; i++)
  {
    if (up)
    {
      if ((in[i + 1] - in[i]) < 0)
      {
        up = false;
        int k = i;
        while (in[k - 1] == in[i]) k--;
        if (in[k + (i - k) / 2] > gmagThreshold)
          lmax[count++] = k + (i - k) / 2;
      }
    }
    else
      if (in[i + 1] - in[i] > 0) up = true;
  }
  return count;
}


void VMap::sortMax (int *lmax, int n, int *val) const
{
  for (int i = 1; i < n; i ++)
  {
    int j = i, tmp = lmax[i];
    bool on = true;
    while (on && j > 0)
    {
      if (val[tmp] > val[lmax[j-1]])
      {
        lmax[j] = lmax[j-1];
        j --;
      }
      else on = false;
    }
    lmax[j] = tmp;
  }
}


void VMap::clearMask ()
{
  for (int i = 0; i < width * height; i++) mask[i] = false;
}


void VMap::setMask (const vector<Pt2i> &pts)
{
  vector<Pt2i>::const_iterator it = pts.begin ();
  while (it != pts.end ())
  {
    Pt2i pt = *it++;
    mask[pt.y () * width + pt.x ()] = true;
    for (int i = 0; i < dilations[maskDilation]; i++)
    {
      int x = pt.x () + bowl[i].x ();
      int y = pt.y () + bowl[i].y ();
      if (x >= 0 && x < width && y >= 0 && y < height)
        mask[y * width + x] = true;
    }
  }
}
