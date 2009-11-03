#ifndef AUDIOGRAPHER_SINK_H
#define AUDIOGRAPHER_SINK_H

#include <boost/shared_ptr.hpp>

#include "types.h"

namespace AudioGrapher
{

template <typename T>
class Sink  {
  public:
	virtual ~Sink () {}
	
	/** Process given data
	 *  \param data pointer to an array of data to process
	 *  \param frames number of frames in data. process() is allowed to be a NOP when frames == 0
	 */
	virtual void process (T * data, nframes_t frames) = 0;
};

} // namespace

#endif // AUDIOGRAPHER_SINK_H

