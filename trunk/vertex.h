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
	
	void process (TIn * data, nframes_t frames)
	{
		TOut * out = 0;
		nframes_t const out_frames = process (data, out, frames);
		assert(out);
		
		for (typename Source<TOut>::SinkList::iterator i = Source<TOut>::outputs.begin(); i != Source<TOut>::outputs.end(); ++i) {
			(*i)->process (out, out_frames);
		}
	}
	
  protected:

	/// Process frames from in and make out point to the result
	virtual nframes_t process (TIn * in, TOut * & out, nframes_t frames) = 0;
};

} // namespace

#endif // AUDIOGRAPHER_VERTEX_H

