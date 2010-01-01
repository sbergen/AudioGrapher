#ifndef AUDIOGRAPHER_INTERLEAVER_H
#define AUDIOGRAPHER_INTERLEAVER_H

#include "audiographer/core/types.h"
#include "audiographer/core/sink.h"
#include "audiographer/core/exception.h"
#include "listed_source.h"

#include <vector>
#include <cmath>

namespace AudioGrapher
{

template<typename T>
class Interleaver : public ListedSource<T>
{
  public: 
	
	Interleaver()
	  : channels (0)
	  , max_frames (0)
	  , buffer (0)
	{}
	
	~Interleaver() { reset(); }
	
	void init (unsigned int num_channels, nframes_t max_frames_per_channel)
	{
		reset();
		channels = num_channels;
		max_frames = max_frames_per_channel;
		
		buffer = new T[channels * max_frames];
		
		for (unsigned int i = 0; i < channels; ++i) {
			inputs.push_back (InputPtr (new Input (*this, i)));
		}
	}
	
	typename Source<T>::SinkPtr input (unsigned int channel)
	{
		if (channel >= channels) {
			throw Exception (*this, "Channel out of range");
		}
		
		return boost::static_pointer_cast<Sink<T> > (inputs[channel]);
	}
	
  private: 
 
	class Input : public Sink<T>
	{
	  public:
		Input (Interleaver & parent, unsigned int channel)
		  : frames_written (0), parent (parent), channel (channel) {}
		
		void process (ProcessContext<T> const & c)
		{
			if (c.channels() > 1) { throw Exception (*this, "Data input has more than on channel"); }
			if (frames_written) { throw Exception (*this, "Input channels out of sync"); }
			frames_written = c.frames();
			parent.write_channel (c, channel);
		}
		
		using Sink<T>::process;
		
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
	
	void reset_channels ()
	{
		for (unsigned int i = 0; i < channels; ++i) {
			inputs[i]->reset();
		}

	}
	
	void write_channel (ProcessContext<T> const & c, unsigned int channel)
	{
		if (c.frames() > max_frames) {
			reset_channels();
			throw Exception (*this, "Too many frames given to an input");
		}
		
		for (unsigned int i = 0; i < c.frames(); ++i) {
			buffer[channel + (channels * i)] = c.data()[i];
		}
		
		nframes_t const ready_frames = ready_to_output();
		if (ready_frames) {
			ProcessContext<T> c_out (c, buffer, ready_frames, channels);
			ListedSource<T>::output (c_out);
			reset_channels ();
		}
	}

	nframes_t ready_to_output()
	{
		nframes_t ready_frames = inputs[0]->frames();
		if (!ready_frames) { return 0; }

		for (unsigned int i = 1; i < channels; ++i) {
			nframes_t const frames = inputs[i]->frames();
			if (!frames) { return 0; }
			if (frames != ready_frames) {
				init (channels, max_frames);
				throw Exception (*this, "Frames count out of sync");
			}
		}
		return ready_frames * channels;
	}

	typedef boost::shared_ptr<Input> InputPtr;
	std::vector<InputPtr> inputs;
	
	unsigned int channels;
	nframes_t max_frames;
	T * buffer;
};

} // namespace

#endif // AUDIOGRAPHER_INTERLEAVER_H
