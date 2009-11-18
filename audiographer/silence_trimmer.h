#ifndef AUDIOGRAPHER_SILENCE_TRIMMER_H
#define AUDIOGRAPHER_SILENCE_TRIMMER_H

#include "listed_source.h"
#include "sink.h"
#include "exception.h"
#include "utils.h"

#include <cstring>

using namespace AudioGrapher;

template<typename T>
class SilenceTrimmer : public ListedSource<T>, Sink<T>
{
  public:

	SilenceTrimmer() : in_beginning (true), silence_frames (0) {}
	~SilenceTrimmer() { reset(); }

	void reset()
	{
		in_beginning = true;
		silence_frames = 0;
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
		nframes_t silence_buffer_size = Utils::get_zero_buffer_size<T>();
		if (silence_buffer_size == 0) { throw Exception (*this, "Utils::init_zeros has not been called!"); }
		while (silence_frames > 0) {
			nframes_t frames = std::min (silence_buffer_size, silence_frames);
			silence_frames -= frames;
			ConstProcessContext<T> c_out (Utils::get_zeros<T>(frames), frames);
			ListedSource<T>::output (c_out);
		}
	}


	bool      in_beginning;
	nframes_t silence_frames;
};

#endif // AUDIOGRAPHER_SILENCE_TRIMMER_H
