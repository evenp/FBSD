#ifndef ABSOLUTE_RATIONAL_H
#define ABSOLUTE_RATIONAL_H


using namespace std;



/** 
 * @class AbsRat absrat.h
 * \brief Absolute rational number.
 * 
 * This absolute number may have a null denominator.
 * It should not be evaluated.
 * It is mostly intended to comparison operations.
 * \author {P. Even}
 */
class AbsRat
{
public:

  /**
   * @fn AbsRat ()
   * \brief Creates a null absolute rational number.
   */
  AbsRat ();

  /**
   * @fn AbsRat (int numerator, int denominator)
   * \brief Creates a absolute rational number from numerator and denominator.
   * @param numerator Numerator.
   * @param denominator Denominator.
   */
  AbsRat (int numerator, int denominator);

  /**
   * @fn AbsRat (const AbsRat &rat)
   * \brief Creates a absolute rational number from another one.
   * @param rat The rational number to copy.
   */
  AbsRat (const AbsRat &rat);

  /**
   * @fn ~AbsRatRational ()
   * \brief Deletes the absolute rational number.
   */
  ~AbsRat () { }

  /**
   * @fn int numerator ()
   * \brief Returns the numerator of the absolute rational number.
   */
  inline int numerator () const { return num; }

  /**
   * @fn int denominator ()
   * \brief Returns the denominator of the absolute rational number.
   */
  inline int denominator () const { return den; }

  /**
   * @fn int floor ()
   * \brief Returns the nearest smaller integer value.
   */
  inline int floor () const { return (num / den); }

  /**
   * @fn void set (const AbsRat &val)
   * \brief Sets the value of the rational number.
   * @value val New value of the rational number.
   */
  inline void set (const AbsRat &val) { num = val.num; den = val.den; }

  /**
   * @fn void set (int val)
   * \brief Sets the value of the rational number.
   * @value val New value of the rational number.
   */
  inline void set (int val) { num = val; den = 1; }

  /**
   * @fn void set (int numerator, int denominator)
   * \brief Sets the value of the rational number.
   * @value numerator New numerator of the rational number.
   * @value denominator New denominator of the rational number.
   */
  inline void set (int numerator, int denominator) {
    num = numerator; den = denominator; }

  /**
   * @fn bool equals (const AbsRat &r)
   * \brief Checks equivalence to the given rational number.
   * @param r the given rational number.
   */
  inline bool equals (const AbsRat &r) const {
    return (num * r.den == den * r.num); }

  /**
   * @fn bool lessThan (const AbsRat &r)
   * \brief Checks if the rational number is strictly less than given one.
   * @param r the given rational number.
   */
  inline bool lessThan (const AbsRat &r) const {
    return (num * r.den < den * r.num); }

  /**
   * @fn bool lessEqThan (const AbsRat &r)
   * \brief Checks if the rational number is less or equal to given one.
   * @param r the given rational number.
   */
  inline bool lessEqThan (const AbsRat &r) const {
    return (num * r.den <= den * r.num); }

  /**
   * @fn bool greaterThan (const AbsRat &r)
   * \brief Checks if the rational number is strictly greater than given one.
   * @param r the given rational number.
   */
  inline bool greaterThan (const AbsRat &r) const {
    return (num * r.den > den * r.num); }

  /**
   * @fn bool greaterEqThan (const AbsRat &r)
   * \brief Checks if the rational number is greater or equal to given one.
   * @param r the given rational number.
   */
  inline bool greaterEqThan (const AbsRat &r) const {
    return (num * r.den >= den * r.num); }

  /**
   * @fn void attractsTo (const AbsRat &val, const AbsRat &ratio)
   * \brief Attracts the rational number to the given one with given ratio.
   * Withdraws to the rational number
   *   the difference between the rational number and an attractor number
   *   multiplied by the ratio.
   * The denominator is left unchanged.
   * @param val Goal rational value.
   * @param ratio Attraction ratio.
   */
  void attractsTo (const AbsRat &val, const AbsRat &ratio);

  /**
   * @fn void sticksTo (const AbsRat &val)
   * \brief Sets the rational number to the given one value.
   * The denominator is left unchanged.
   * @param val Given rational number.
   */
  void sticksTo (const AbsRat &val);

  /**
   * @fn void mult (const AbsRat &val)
   * \brief Multiplies the rational number by the given one.
   * @param val Given rational number.
   */
  void mult (const AbsRat &val);

  /**
   * @fn AbsRat sum (int val) const
   * \brief Returns the sum of the rational with an integer value.
   * @param val integer value.
   */
  inline AbsRat sum (int val) const { return (AbsRat (num + val * den, den)); }

  /**
   * @fn AbsRat sumHalf () const
   * \brief Returns the sum of the rational with 1/2.
   */
  inline AbsRat sumHalf () const {
    return (den % 2 == 1 ? AbsRat (2 * num + den, 2 * den)
                         : AbsRat (num + den / 2, den)); }


protected:

  /** Positive numerator of the rational number. */
  int num;
  /** Positive denominator of the rational number (might be null). */
  int den;
};

#endif
