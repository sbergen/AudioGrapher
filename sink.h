#ifndef AUDIOGRAPHER_SINK_H
#define AUDIOGRAPHER_SINK_H

#include <boost/shared_ptr.hpp>

#include "types.h"

namespace AudioGrapher
{

template <typename T>
class Sink  {
  public:
	Sink () {}
	virtual ~Sink () {}
	
	// processes data and return number of frames written
	virtual void process (T * data, nframes_t frames) = 0;
};

} // namespace

#endif // AUDIOGRAPHER_SINK_H

