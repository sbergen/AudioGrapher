#ifndef AUDIOGRAPHER_SOURCE_H
#define AUDIOGRAPHER_SOURCE_H

#include "types.h"
#include "sink.h"

#include <list>
#include <cassert>
#include <boost/shared_ptr.hpp>


namespace AudioGrapher
{

template <typename T>
class Source
{
  public:

	typedef boost::shared_ptr<Sink<T> > SinkPtr;
	typedef std::list<SinkPtr>          SinkList;
	
	Source () {}
	virtual ~Source () {}
	
	void add_output (SinkPtr output)
	{
		outputs.push_back(output);
	}
	
	void clear_outputs ()
	{
		outputs.clear();
	}
	
	void remove_output (SinkPtr output)
	{
		outputs.remove(output);
	}
	
  protected:

	SinkList outputs;

};

} // namespace

#endif //AUDIOGRAPHER_SOURCE_H

