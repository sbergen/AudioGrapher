#ifndef AUDIOGRAPHER_INTERLEAVER_H
#define AUDIOGRAPHER_INTERLEAVER_H

#include "types.h"
#include "vertex.h"
#include "exception.h"

#include <vector>
#include <cmath>

namespace AudioGrapher
{

template<typename T>
class Interleaver : public Source<T>
{
  public: 
	
	void init (unsigned int num_channels, nframes_t max_frames_per_channel)
	{
		reset();
		channels = num_channels;
		max_frames = max_frames_per_channel;
		
		buffer = new float[channels * max_frames];
		memset (buffer, 0, channels * max_frames);
		
		for (unsigned int i = 0; i < channels; ++i) {
			inputs.push_back (InputPtr (new Input (this, i)));
		}
	}
	
	typename Source<T>::SinkPtr input (unsigned int channel)
	{
		if (channel >= channels) {
			throw Exception ("Interleaver channel out of range");
		}
		
		return boost::static_pointer_cast<Source<T>::SinkPtr> (inputs[channel]);
	}
	
  private: 
 
	class Input : public Sink<T>
	{
	  public:
		Input (Interleaver & parent, unsigned int channel)
		  : parent (parent), channel (channel) {}
		
		void process (T * data, nframes_t frames)
		{
			parent.write_channel (data, frames, channel);
			frames_written = frames;
		}
		
		nframes_t frames() { return frames_written; }
		void reset() { frames_written = 0; }
		
	  private:
		nframes_t frames_written;
		Interleaver & parent;
		unsigned int channel;
	};
	  
	void reset ()
	{
		inputs.clear();
		delete [] buffer;
		buffer = 0;
		channels = 0;
		max_frames = 0;
	}
	
	void write_channel (T * data, nframes_t frames, unsigned int channel)
	{
		if (frames > max_frames) {
			throw Exception ("Interleaver: too many frames given to an input");
		}
		
		for (unsigned int i = 0; i < frames; ++i) {
			buffer[channel + (channels * i)] = data[i];
		}
		
		nframes_t max_frames = 0;
		for (unsigned int i = 0; i < channels; ++i) {
			nframes_t const frames = inputs[i]->frames();
			if (!frames) { return; }
			max_frames = std::max (max_frames, frames);
		}
		
		for (typename Source<T>::SinkList::iterator i = Source<T>::outputs.begin(); i != Source<T>::outputs.end(); ++i) {
			(*i)->process (buffer, max_frames);
		}
		
		memset (buffer, 0, channels * max_frames);
	}
	
	typedef boost::shared_ptr<Input> InputPtr;
	std::vector<InputPtr> inputs;
	
	unsigned int channels;
	nframes_t max_frames;
	T * buffer;
};

} // namespace

#endif // AUDIOGRAPHER_INTERLEAVER_H
