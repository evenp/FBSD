#ifndef BLURRED_SEGMENT_FILTER_H
#define BLURRED_SEGMENT_FILTER_H

#include <vector>
#include "pt2i.h"
#include "blurredsegment.h"

using namespace std;


/** 
 * @class Blurred segment filter.
 * \author {P. Even}
 */
class BSFilter
{
public:

  /**
   * \brief Creates a empty blurred segment filter.
   */
  BSFilter ();

  /**
   * \brief Deletes the filter.
   */
  virtual ~BSFilter ();

  /**
   * \brief Clears the stored information.
   */
  virtual void clear ();

  /** \brief Filters and returns a blurred segment.
    * @param bs The blurred segment to be processed.
    */
  virtual BlurredSegment *filter (BlurredSegment *bs);

  /**
   * \brief Returns the accepted points by the line space filter.
   */
  vector<Pt2i> getAccepted ();

  /**
   * \brief Returns the rejected points by the line space filter.
   */
  inline vector<Pt2i> getRejected () const { return out; }

  /**
   * \brief Returns the blurred segment size before filtering.
   */
  inline int blurredSegmentInitialSize () const { return bsInitialSize; }

  /**
   * \brief Returns the blurred segment size after filtering.
   */
  inline int blurredSegmentFinalSize () const { return bsFinalSize; }


protected :

  /** Blurred segment size before filtering. */
  int bsInitialSize;
  /** Blurred segment size after filtering. */
  int bsFinalSize;
  /** Accepted left points after line space filtering. */
  vector<Pt2i> leftIn;
  /** Accepted right points after line space filtering. */
  vector<Pt2i> rightIn;
  /** Rejected points after line space filtering. */
  vector<Pt2i> out;

};
#endif
