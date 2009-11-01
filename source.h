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

template<typename T>
class StandaloneSource : public Source<T>
{
  public:
	StandaloneSource() {}
	virtual ~StandaloneSource() {}
	
	void process (nframes_t frames)
	{
		T * data = 0;
		nframes_t const frames_read = read_data (data, frames);
		assert(data);
		
		for (typename Source<T>::SinkList::iterator i = Source<T>::outputs.begin(); i != Source<T>::outputs.end(); ++i) {
			(*i)->process (data, frames_read);
		}
	}
	
  protected:

	/// Try to read nframes and set data to point to it, return the actual amount of frames read
	virtual nframes_t read_data (T* & data, nframes_t nframes) = 0;

};


} // namespace

#endif //AUDIOGRAPHER_SOURCE_H

