#ifndef AUDIOGRAPHER_SINK_H
#define AUDIOGRAPHER_SINK_H

#include <boost/shared_ptr.hpp>

#include "process_context.h"

namespace AudioGrapher
{

template <typename T>
class Sink  {
  public:
	virtual ~Sink () {}
	
	/** Process given data.
	  * The data can not be modified, so in-place processing is not allowed.
	  * At least this function must be implemented by deriving classes
	  */
	virtual void process (ProcessContext<T> const & /*context*/) {} // TODO make this pure vurtual
	
	/** Process given data
	  * Data may be modified, so in place processing is allowed.
	  * The default implementation calls the non-modifying version,
	  * so this function does not need to be overriden by deriving classes.
	  * However, if the sink can do in-place processing, overriding this is highly recommended!
	  */
	virtual void process (ProcessContext<T> & context) { this->process (const_cast<ProcessContext<T> const &> (context)); }
	
	// TODO: This is to be replaced by the versions above!
	virtual void process (T * /*data*/, nframes_t /*frames*/) {}
};

} // namespace

#endif // AUDIOGRAPHER_SINK_H

