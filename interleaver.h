#ifndef AUDIOGRAPHER_INTERLEAVER_H
#define AUDIOGRAPHER_INTERLEAVER_H

#include "types.h"
#include "listed_source.h"
#include "sink.h"
#include "exception.h"

#include <vector>
#include <cmath>

namespace AudioGrapher
{

template<typename T>
class Interleaver : public ListedSource<T>
{
  public: 
	
	void init (unsigned int num_channels, nframes_t max_frames_per_channel);
	typename Source<T>::SinkPtr input (unsigned int channel);
	
  private: 
 
	class Input : public Sink<T>
	{
	  public:
		Input (Interleaver & parent, unsigned int channel)
		  : frames_written (0), parent (parent), channel (channel) {}
		
		void process (T * data, nframes_t frames)
		{
			frames_written = frames;
			parent.write_channel (data, frames, channel);
		}
		
		nframes_t frames() { return frames_written; }
		void reset() { frames_written = 0; }
		
	  private:
		nframes_t frames_written;
		Interleaver & parent;
		unsigned int channel;
	};
	  
	void reset ();
	void write_channel (T * data, nframes_t frames, unsigned int channel);
	nframes_t ready_to_output();
	void output();	

	typedef boost::shared_ptr<Input> InputPtr;
	std::vector<InputPtr> inputs;
	
	unsigned int channels;
	nframes_t max_frames;
	T * buffer;
};

#include "interleaver-inl.h"

} // namespace

#endif // AUDIOGRAPHER_INTERLEAVER_H
