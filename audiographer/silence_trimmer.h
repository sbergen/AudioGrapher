#ifndef AUDIOGRAPHER_SILENCE_TRIMMER_H
#define AUDIOGRAPHER_SILENCE_TRIMMER_H

#include "listed_source.h"
#include "sink.h"
#include "exception.h"

#include <cstring>

namespace AudioGrapher
{

template<typename T>
class SilenceTrimmer : public ListedSource<T>, Sink<T>
{
  public:

	SilenceTrimmer() : in_beginning (true), silence_frames (0), silence_buffer (0), silence_buffer_size (0) {}
	~SilenceTrimmer() { reset(); }

	void reset()
	{
		delete [] silence_buffer;
		silence_buffer = 0;
		silence_buffer_size = 0;
		in_beginning = true;
		silence_frames = 0;
	}

	void init (nframes_t buffer_size)
	{
		delete [] silence_buffer;
		silence_buffer = new T[buffer_size];
		silence_buffer_size = buffer_size;
		memset (silence_buffer, 0, buffer_size * sizeof (T));
	}

	void process (ProcessContext<T> const & c)
	{
		nframes_t frame_index = 0;
		if (in_beginning) {
			if (find_first_non_zero_sample (c, frame_index)) {
				in_beginning = false;
				ConstProcessContext<T> c_out (&c.data()[frame_index], c.frames() - frame_index);
				ListedSource<T>::output (c_out);
			} // else do nothing
		} else {
			if (find_first_non_zero_sample (c, frame_index)) {
				flush_intermediate_silence();
				ListedSource<T>::output (c);
			} else {
				silence_frames += c.frames();
			}
		}
	}

  private:
	bool find_first_non_zero_sample (ProcessContext<T> const & c, nframes_t & result_frame)
	{
		for (nframes_t i = 0; i < c.frames(); ++i) {
			if (c.data()[i] != static_cast<T>(0.0)) {
				result_frame = i;
				return true;
			}
		}
		return false;
	}

	void flush_intermediate_silence()
	{
		if (silence_buffer_size == 0) { throw Exception (*this, "No buffer reserved for silence"); }
		while (silence_frames > 0) {
			nframes_t frames = std::min (silence_buffer_size, silence_frames);
			silence_frames -= frames;
			ConstProcessContext<T> c_out (silence_buffer, frames);
			ListedSource<T>::output (c_out);
		}
	}


	bool      in_beginning;
	nframes_t silence_frames;

	T *       silence_buffer;
	nframes_t silence_buffer_size;
};


} // namespace

#endif // AUDIOGRAPHER_SILENCE_TRIMMER_H
