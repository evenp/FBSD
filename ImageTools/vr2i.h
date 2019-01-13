#ifndef VR2I_H
#define VR2I_H

using namespace std;


/** 
 * @class Vr2i vr2i.h
 * \brief Vector in the digital plane.
 * \author {P. Even}
 */
class Vr2i
{
public:

  /**
   * @fn Vr2i ()
   * \brief Creates a unit vector on X axis.
   */
  Vr2i ();

  /**
   * @fn Vr2i (int x, int y)
   * \brief Creates a vector using coordinates.
   * @param x abscissae.
   * @param y ordinate.
   */
  Vr2i (int x, int y);

  /**
   * @fn Vr2i (Vr2i p)
   * \brief Creates a copy of a given vector.
   * @param v original.
   */
  Vr2i (const Vr2i &v);

  /**
   * @fn int x ()
   * \brief Returns the vector abscissae.
   */
  inline int x () const {
    return xv; }

  /**
   * @fn int y ()
   * \brief Returns the vector ordinate.
   */
  inline int y () const {
    return yv; }

  /**
   * @fn void set (int x, int y)
   * \brief Sets the vector coordinates.
   * @param x new abscissae.
   * @param y new ordinate.
   */
  inline void set (int x, int y) {
    xv = x;
    yv = y; }

  /**
   * @fn int norm2 ()
   * \brief Returns the squared norm of the vector.
   * If intensity value holds on a byte, gradient holds on a short
   *   and gradient squared norm holds on a int.
   */
  inline int norm2 () const {
    return (xv * xv + yv * yv); }

  /**
   * @fn int scalarProduct (Vr2i vec)
   * \brief Returns the scalar product with the given vector.
   * If intensity value holds on a byte, scalar product (SP) holds on a short
   *   and squared SP holds on a int.
   * @param vec The given vector.
   */
  inline int scalarProduct (Vr2i vec) const {
    return (xv * vec.xv + yv * vec.yv); }

  /**
   * @fn int squaredScalarProduct (Vr2i vec)
   * \brief Returns the squared scalar product with the given vector.
   * @param vec The given vector.
   */
  inline int squaredScalarProduct (Vr2i vec) const {
    return ((xv * vec.xv + yv * vec.yv)
            * (xv * vec.xv + yv * vec.yv)); }

  /**
   * @fn bool leftside (Vr2i vec)
   * \brief Returns if the given vector is on the left side of the vector.
   * @param vec The given vector.
   */
  inline bool leftside (Vr2i vec) const {
    return (xv * vec.yv > yv * vec.xv); }

  /**
   * @fn int squaredVectorProduct (Vr2i vec)
   * \brief Returns the squared norm of the vector product with given vector.
   * @param vec The given vector.
   */
  inline int squaredVectorProduct (Vr2i vec) const {
    return ((xv * vec.yv - yv * vec.xv)
            * (xv * vec.yv - yv * vec.xv)); }

  /**
   * @fn bool equals (Vr2i p)
   * \brief Checks equivalence to the given vector.
   * @param v the given vector.
   */
  inline bool equals (Vr2i v) const {
    return (v.xv == xv && v.yv == yv); }

  /**
   * @fn int manhattan ()
   * \brief Returns the manhattan length of the vector.
   */
  inline int manhattan () const {
    return ((xv > 0 ? xv : - xv) + (yv > 0 ? yv : - yv)); }

  /**
   * @fn int chessboard ()
   * \brief Returns the chessboard length of the vector.
   */
  inline int chessboard () const {
    int x = (xv < 0 ? -xv : xv), y = (yv < 0 ? -yv : yv);
    return (x > y ? x : y); }

  /**
   * @fn Vr2i orthog ()
   * \brief Returns a CCW orthogonal vector.
   */
  Vr2i orthog () const;

  /**
   * @fn void setOrthog ()
   * \brief Returns a CCW orthogonal vector.
   */
  inline void setOrthog () { int tmp = xv; xv = - yv; yv = tmp; }

  /**
   * @fn bool directedAs (Vr2i ref)
   * \brief Returns true if v has the same direction as a reference.
   * @param ref The reference vector.
   */
  inline bool directedAs (const Vr2i &ref) const {
    return (xv * ref.xv + yv * ref.yv >= 0); }

  /**
   * @fn bool orientedAs (Vr2i ref)
   * \brief Returns true if v has the same orientation as a reference.
   * @param ref The reference vector.
   */
  bool orientedAs (const Vr2i &ref) const;

  /**
   * @fn void invert ()
   * \brief Inverts the vector.
   */
  inline void invert () {
    xv = -xv;
    yv = -yv; }

  /**
   * @fn bool *steps (int *n)
   * \brief Returns the location of the steps between the vector ends.
   * @param n size of the returned array.
   */
  bool *steps (int *n) const;


private:

  /** Vector abscissae. */
  int xv;
  /** Vector ordinate. */
  int yv;
};

#endif
