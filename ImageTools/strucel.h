#ifndef STRUCEL_H
#define STRUCEL_H

#include "vr2i.h"

using namespace std;


/** 
 * @class Strucel strucel.h
 * \brief Structuring element for morphological operations.
 * \author {P. Even and P. Ngo}
 */
class Strucel
{
public:

  /** 3x3 cross structuring element type. */
  static const int TYPE_PLUS_3X3;
  /** 1x3 horizontal structuring element type. */
  static const int TYPE_HOR;
  /** 3x1 vertical structuring element type. */
  static const int TYPE_VER;


  /** 
   * \brief Creates a structural element.
   * @param type Pattern type.
   */
  Strucel (int type);

  /** 
   * \brief Deletes the structuring element.
   */
  ~Strucel ();

  /** 
   * \brief Returns the pattern width.
   */
  inline int getWidth () const
  {
    return width;
  }

  /** 
   * \brief Returns the pattern height.

   */
  inline int getHeight () const
  {
    return height;
  }

  /** 
   * \brief Calculates the top hat gradient of an image.
   * @param out Output gradient array (the memory should be allocated before).
   * @param in Image bi-dimensional array.
   * @param width Image width.
   * @param height Image height.
   */
  void tophatGradient (int *out, int **in, int width, int height);

  /** 
   * \brief Calculates the top hat gradient of an image.
   * @param out Output gradient array (the memory should be allocated before).
   * @param in Image array.
   * @param width Image width.
   * @param height Image height.
   */
  void tophatGradient (int *out, int *in, int width, int height);

  /** 
   * \brief Calculates the black hat gradient of an image.
   * @param out Output gradient array (the memory should be allocated before).
   * @param in Image bi-dimensional array.
   * @param width Image width.
   * @param height Image height.
   */
  void blackhatGradient (int *out, int **in, int width, int height);

  /** 
   * \brief Calculates the black hat gradient of an image.
   * @param out Output gradient array (the memory should be allocated before).
   * @param in Image array.
   * @param width Image width.
   * @param height Image height.
   */
  void blackhatGradient (int *out, int *in, int width, int height);

  /** 
   * \brief Calculates the morphological gradient of an image.
   * @param out Output gradient array (the memory should be allocated before).
   * @param in Image bi-dimensional array.
   * @param width Image width.
   * @param height Image height.
   */
  void morphoGradient (int *out, int **in, int width, int height);

  /** 
   * \brief Calculates the morphological gradient of an image.
   * @param out Output gradient array (the memory should be allocated before).
   * @param in Image array.
   * @param width Image width.
   * @param height Image height.
   */
  void morphoGradient (int *out, int *in, int width, int height);


private:

  /** Width of the structuring element. */
  int width;
  /** Height of the structuring element. */
  int height;
  /** Size of the pattern. */
  int size;
  /** Pattern of the structuring element as the list of occupied pixels. */
  Vr2i *pattern;
};

#endif
