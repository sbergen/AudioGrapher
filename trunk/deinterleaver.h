#ifndef AUDIOGRAPHER_DEINTERLEAVER_H
#define AUDIOGRAPHER_DEINTERLEAVER_H

#include "types.h"
#include "vertex.h"
#include "exception.h"

#include <vector>

namespace AudioGrapher
{

template<typename T>
class DeInterleaver : public Sink<T>
{
  private:
	typedef boost::shared_ptr<IdentityVertex<T> > OutputPtr;
	
  public:
	DeInterleaver()
	  : channels (0)
	  , max_frames (0)
	  , buffer (0)
	  {}
	
	~DeInterleaver()
	{
		reset();
	}
	
	void init (unsigned int num_channels, nframes_t max_frames_per_channel)
	{
		reset();
		channels = num_channels;
		max_frames = max_frames_per_channel;
		buffer = new float[max_frames];
		
		for (unsigned int i = 0; i < channels; ++i) {
			outputs.push_back (OutputPtr (new IdentityVertex<T>));
		}
	}
	
	typedef boost::shared_ptr<Source<T> > SourcePtr;
	SourcePtr output (unsigned int channel)
	{
		if (channel >= channels) {
			throw Exception ("DeInterleaver channel out of range");
		}
		
		return boost::static_pointer_cast<SourcePtr> (outputs[channel]);
	}
	
	void process (T * data, nframes_t frames)
	{
		nframes_t const  frames_per_channel = frames / channels;
		
		if (frames_per_channel > max_frames) {
			throw Exception ("DeInterleaver: too many frames given to process()");
		}
		
		unsigned int channel = 0;
		for (typename Source<T>::SinkList::iterator it = outputs.begin(); it != Source<T>::outputs.end(); ++it, ++channel) {
			if (!*it) { continue; }
			
			for (unsigned int i = 0; i < frames_per_channel; ++i) {
				buffer[i] = data[channel + (channels * i)];
			}
			(*it)->process (buffer, frames_per_channel);
		}
	}
	
  private:

	void reset ()
	{
		outputs.clear();
		delete [] buffer;
		buffer = 0;
		channels = 0;
		max_frames = 0;
	}
	
	std::vector<OutputPtr> outputs;
	unsigned int channels;
	nframes_t max_frames;
	T * buffer;
};

} // namespace

#endif // AUDIOGRAPHER_DEINTERLEAVER_H
