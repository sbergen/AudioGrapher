#ifndef AUDIOGRAPHER_VERTEX_H
#define AUDIOGRAPHER_VERTEX_H

#include <boost/shared_ptr.hpp>

#include "types.h"
#include "source.h"
#include "sink.h"

namespace AudioGrapher
{

template <typename TIn, typename TOut>
class Vertex : public Sink<TIn>, public Source<TOut> {
  public:
	Vertex () {}
	virtual ~Vertex () {}
	
	virtual void process (TIn * data, nframes_t frames) = 0;
	
  protected:

	/// Helper for derived classes
	void output (TOut * data, nframes_t frames)
	{
		for (typename Source<TOut>::SinkList::iterator i = Source<TOut>::outputs.begin(); i != Source<TOut>::outputs.end(); ++i) {
			(*i)->process (data, frames);
		}
	}
};

template<typename T>
class IdentityVertex : public Vertex<T, T>
{
  public:
	void process (T * data, nframes_t frames)
	{
		output(data, frames);
	}
};

} // namespace

#endif // AUDIOGRAPHER_VERTEX_H

