#ifndef VMAP_H
#define VMAP_H

#include "pt2i.h"
#include "strucel.h"

using namespace std;


/** 
 * @class VMap vmap.h
 * \brief Vector map.
 * \author {P. Even and P. Ngo}
 */
class VMap
{
public:

  /** Gradient extraction method : Sobel with 3x3 kernel. */
  static const int TYPE_SOBEL_3X3;
  /** Gradient extraction method : Sobel with 5x5 kernel. */
  static const int TYPE_SOBEL_5X5;
  /** Gradient extraction method : Morphological top hat (I - E(I)). */
  static const int TYPE_TOP_HAT;
  /** Gradient extraction method : Morphological black hat (D(I) - I). */
  static const int TYPE_BLACK_HAT;
  /** Gradient extraction method : Morphological gradient (D(I) - E(I)). */
  static const int TYPE_MORPHO;
  /** Gradient extraction method : Morphological directional top hat. */
  static const int TYPE_FULL_TOP_HAT;
  /** Gradient extraction method : Morphological directional black hat. */
  static const int TYPE_FULL_BLACK_HAT;
  /** Gradient extraction method : Morphological directional gradient. */
  static const int TYPE_FULL_MORPHO;


  /** 
   * \brief Creates a gradient map from scalar data.
   * @param width Map width.
   * @param height Map height.
   * @param data Scalar data array.
   * @param type Gradient extraction method (default is Soble with 3x3 kernel).
   */
  VMap (int width, int height, int *data, int type = 0);

  /** 
   * \brief Creates a gradient map from scalar data.
   * @param width Map width.
   * @param height Map height.
   * @param data Scalar data bi-dimensional array.
   */
  VMap (int width, int height, int **data, int type = 0);

  /** 
   * \brief Deletes the vector map.
   */
  ~VMap ();

  /** 
   * \brief Returns the map width.
   */
  inline int getWidth () const
  {
    return width;
  }

  /** 
   * \brief Returns the map height.
   */
  inline int getHeight () const
  {
    return height;
  }

  /** 
   * \brief Returns the maximal value between map width or height.
   */
  inline int getHeightWidthMax () const
  {
    return (height > width ? height : width);
  }

  /**
   * \brief Checks whether the given point lies within the map bounds.
   * @param i Column number.
   * @param j Line number.
   */
  inline bool contains (int i, int j)
  {
    return (i >= 0 && i < width && j >= 0 && j < height);
  }

  /**
   * \brief Returns the vector at point (i,j).
   * @param i Column number.
   * @param j Line number.
   */
  Vr2i getValue (int i, int j) const
  {
    return (map[j * width + i]);
  }

  /**
   * \brief Returns the vector at given point.
   * @param p Point position.
   */
  Vr2i getValue (Pt2i p) const
  {
    return (map[p.y () * width + p.x ()]);
  }

  /**
   * \brief Returns the squared norm of the vector magnitude at point (i,j).
   * @param i Column number.
   * @param j Line number.
   */
  int sqNorm (int i, int j) const;

  /**
   * \brief Returns the squared norm of the vector magnitude at given point.
   * @param p Point position.
   */
  int sqNorm (Pt2i p) const;

  /**
   * \brief Returns the comparable norm of the vector magnitude at point (i,j).
   * @param i Column number.
   * @param j Line number.
   */
  inline int magn (int i, int j) const { return (imap[j * width + i]); }

  /**
   * \brief Returns the comparable norm of the vector magnitude at given point.
   * @param p Point position.
   */
  inline int magn (Pt2i p) const { return (imap[p.y () * width + p.x ()]); }

  /** 
   * \brief Returns the index of the largest vector at given positions.
   *   First and last points of the list are not accepted.
   *   A gradient minimal threshold is set for the test.
   * Returns -1 if no max is found.
   * @param pix List of points.
   */
  int largestIn (const vector<Pt2i> &pix) const;

  /**
   * Keeps elements that are not already selected (in the mask array).
   * Returns the number of remaining elements in the selection.
   * @param pix Input array of scanned points.
   * @param n Initial size of the selection of points.
   * @param ind Selection of points.
   */
  int keepFreeElementsIn (const vector<Pt2i> &pix, int n, int *ind) const;

  /**
   * \brief Searches local gradient maxima values.
   * Returns the count of perceptible local maxima found.
   * @param lmax Local max index array.
   * @param n Count of input max values.
   * @param in Array of input values.
   */
  int keepContrastedMax (int *lmax, int n, int *in) const;

  /**
   * Keeps elements with the same direction as a reference vector
   *   in a selection of points (positive scalar product).
   * Returns the number of remaining elements in the selection.
   * @param pix Input array of scanned points.
   * @param n Initial size of the selection of points.
   * @param ind Selection of points.
   * @param ref The reference vector.
   */
  int keepDirectedElementsAs (const vector<Pt2i> &pix,
                              int n, int *ind, const Vr2i &ref) const;

  /**
   * Keeps elements with gradient value near a reference vector
   *   in a selection of points.
   * Relies on angleThreshold value for the test.
   * Returns the number of remaining elements in the selection.
   * @param pix Input array of scanned points.
   * @param n Initial size of the selection of points.
   * @param ind Selection of points.
   * @param ref The reference vector.
   */
  int keepOrientedElementsAs (const vector<Pt2i> &pix,
                              int n, int *ind, const Vr2i &ref) const;

  /**
   * \brief Gets filtered and sorted local gradient maxima.
   * Local max already used are pruned.
   * Returns the count of found gradient maxima.
   * @param lmax Local max index array.
   * @param pix Provided points.
   */
  int localMax (int *lmax, const vector<Pt2i> &pix) const;

  /**
   * \brief Gets filtered and sorted local oriented gradient maxima.
   * Local maxima are filtered according to the gradient direction and sorted.
   * Returns the count of found gradient maxima.
   * @param lmax Local max index array.
   * @param pix Provided points.
   * @param gref Gradient vector reference.
   */
  int localMax (int *lmax, const vector<Pt2i> &pix, const Vr2i &gref) const;

  /**
   * \brief Returns the gradient threshold value used for maxima detection.
   */
  inline int getGradientThreshold () const { return (gradientThreshold); }

  /**
   * \brief Increments the gradient threshold value used for maxima detection.
   */
  inline void incGradientThreshold (int inc) {
    gradientThreshold += inc;
    if (gradientThreshold < 0) gradientThreshold = 0;
    if (gradientThreshold > 255) gradientThreshold = 255;
    gmagThreshold = gradientThreshold;
    if (gtype <= TYPE_SOBEL_5X5) gmagThreshold *= gmagThreshold;
  }

  /**
   * \brief Returns the gradient resolution value used for maxima filtering.
   */
  inline int getGradientResolution () const { return (gradres); }

  /**
   * \brief Increments the gradient resolution value used for maxima filtering.
   */
  inline void incGradientResolution (int inc) {
    gradres += inc * 5;
    if (gradres < 0) gradres = 0; }

  /**
   * \brief Switches the direction constraint for local maxima selection.
   */
  inline void switchOrientationConstraint () {
    orientedGradient = ! orientedGradient; }

  /**
   * \brief Switches the direction constraint for local maxima selection.
   */
  inline bool isOrientationConstraintOn () const { return orientedGradient; }

  /**
   * \brief Returns the occupancy mask contents.
   */
  inline bool *getMask () const { return (mask); }

  /**
   * \brief Clears the occupancy mask.
   */
  void clearMask ();

  /**
   * \brief Adds positions to the occupancy mask.
   * @param pts Vector of points.
   */
  void setMask (const vector<Pt2i> &pts);

  /**
   * \brief Sets mask activation on or off.
   * @param status Required activation status.
   */
  inline void setMasking (bool status) { masking = status; }

  /**
   * \brief Retuns the mask dilation size.
   */
  inline int getMaskDilation () const { return (dilations[maskDilation]); }

  /**
   * \brief Toggles the mask dilation size.
   */
  inline void toggleMaskDilation () {
    if (++maskDilation == NB_DILATIONS) maskDilation = 0; }

  /**
   * \brief Tests the occupancy of a mask cell.
   * @param pt Position to test in the mask.
   */
  inline bool isFree (const Pt2i &pt) const {
    return (! mask[pt.y () * width + pt.x ()]); }


private:

  /** Default value for near angular deviation tests. */
  static const int NEAR_SQ_ANGLE;
  /** Effective value for the angular deviation test. */
  int angleThreshold;
  /** Default threshold value for the gradient selection. */
  static const int DEFAULT_GRADIENT_THRESHOLD;
  /** Default threshold value for the gradient resolution (filtering). */
  static const int DEFAULT_GRADIENT_RESOLUTION;
  /** Size of the maximal dilation bowl. */
  static const int MAX_BOWL;
  /** Number of dilation types. */
  static const int NB_DILATIONS;
  /** Default dilation for the points added to the mask. */
  static const int DEFAULT_DILATION;

  /** Image width. */
  int width;
  /** Image height. */
  int height;
  /** Gradient type. */
  int gtype;
  /** Vector map. */
  Vr2i *map;
  /** Magnitude map (squarred norm or morphologicalgradient). */
  int *imap;

  /** Occupancy mask. */
  bool *mask;
  /** Flag indicating whether the occupancy mask is in use. */
  bool masking;
  /** Type of dilation applied to the points added to the mask. */
  int maskDilation;
  /** Number of neighbours in the applied dilation. */
  int *dilations;
  /** Dilation bowl. */
  Vr2i *bowl;
  /** Standardized gradient threshold for highest value detection. */
  int gradientThreshold;
  /** Gradient magnitude threshold for highest value detection. */
  int gmagThreshold;
  /** Gradient resolution threshold for local max filtering. */
  int gradres;
  /** Direction constraint for local gradient maxima. */
  bool orientedGradient;


  /** 
   * \brief Initializes the internal data of the vector map.
   */
  void init ();

  /** 
   * \brief Builds the vector map as a gradient map from provided data.
   * Uses a Sobel 3x3 kernel.
   * @param data Initial scalar data.
   */
  void buildGradientMap (int *data);

  /** 
   * \brief Builds the vector map as a gradient map from provided data.
   * Uses a Sobel 3x3 kernel.
   * @param data Initial bi-dimensional scalar data.
   */
  void buildGradientMap (int **data);

  /** 
   * \brief Builds the vector map as a gradient map from provided data.
   * Uses a Sobel 5x5 kernel.
   * @param data Initial scalar data.
   */
  void buildSobel5x5Map (int *data);

  /** 
   * \brief Builds the vector map as a gradient map from provided data.
   * Uses a Sobel 5x5 kernel.
   * @param data Initial bi-dimensional scalar data.
   */
  void buildSobel5x5Map (int **data);

  /**
   * \brief Searches local gradient maxima values.
   * Returns the count of local maxima found.
   * @param lmax Local max index array.
   * @param n Count of input values.
   * @param in Array of input values.
   */
  int searchLocalMax (int *lmax, int n, int *in) const;

  /**
   * \brief Sorts the candidates array by highest magnitude.
   * @param lmax Local max index array.
   * @param n Size of index array.
   * @param val Input values.
   */
  void sortMax (int *lmax, int n, int *val) const;
};

#endif
