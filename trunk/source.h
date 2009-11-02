#ifndef AUDIOGRAPHER_SOURCE_H
#define AUDIOGRAPHER_SOURCE_H

#include "types.h"
#include "sink.h"

#include <boost/shared_ptr.hpp>


namespace AudioGrapher
{

template <typename T>
class Source
{
  public:

	typedef boost::shared_ptr<Sink<T> > SinkPtr;
	
	Source () {}
	virtual ~Source () {}
	
	virtual void add_output (SinkPtr output) = 0;
	virtual void clear_outputs () = 0;
	virtual void remove_output (SinkPtr output) = 0;
};

} // namespace

#endif //AUDIOGRAPHER_SOURCE_H

