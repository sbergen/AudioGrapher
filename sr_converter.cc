#include "sr_converter.h"
#include "exception.h"

#include <cmath>
#include <cstring>
#include <boost/format.hpp>

#define ENABLE_DEBUG 0

#if ENABLE_DEBUG
	#include <iostream>
	#define DEBUG(str) std::cout << str << std::endl;
#else
	#define DEBUG(str)
#endif

namespace AudioGrapher
{
using boost::format;
using boost::str;

SampleRateConverter::SampleRateConverter (uint32_t channels)
  : active (false)
  , channels (channels)
  , max_frames_in(0)
  , leftover_data (0)
  , leftover_frames (0)
  , max_leftover_frames (0)
  , data_out (0)
  , data_out_size (0)
  , src_state (0)
{
}

void
SampleRateConverter::init (nframes_t in_rate, nframes_t out_rate, int quality)
{
	reset();
	
	if (in_rate == out_rate) {
		return;
	}

	active = true;
	int err;
	if ((src_state = src_new (quality, channels, &err)) == 0) {
		throw Exception (*this, str (format ("Cannot initialize sample rate converter: %1%") % src_strerror (err)));
	}

	src_data.src_ratio = (double) out_rate / (double) in_rate;
}

SampleRateConverter::~SampleRateConverter ()
{
	reset();
}

nframes_t
SampleRateConverter::allocate_buffers (nframes_t max_frames)
{
	nframes_t max_frames_out = (nframes_t) ceil (max_frames * src_data.src_ratio * channels);
	if (data_out_size < max_frames_out) {

		delete[] data_out;
		data_out = new float[max_frames_out];
		src_data.data_out = data_out;

		max_leftover_frames = 4 * max_frames;
		leftover_data = (float *) realloc (leftover_data, max_leftover_frames * channels * sizeof (float));
		if (!leftover_data) {
			throw Exception (*this, "A memory allocation error occured");
		}

		max_frames_in = max_frames;
		data_out_size = max_frames_out;
	}
	
	return max_frames_out;
}

void
SampleRateConverter::process (float * in, nframes_t frames)
{
	if (!active) {
		output (in, frames);
		return;
	}

	if (frames > max_frames_in) {
		throw Exception (*this, "process() called with too many frames");
	}

	int err;
	bool first_time = true;

	do {
		src_data.output_frames = data_out_size / channels;
		src_data.data_out = data_out;

		if (leftover_frames > 0) {

			/* input data will be in leftover_data rather than data_in */

			src_data.data_in = leftover_data;

			if (first_time) {

				/* first time, append new data from data_in into the leftover_data buffer */

				memcpy (leftover_data + (leftover_frames * channels), in, frames * channels * sizeof(float));
				src_data.input_frames = frames + leftover_frames;
			} else {

				/* otherwise, just use whatever is still left in leftover_data; the contents
					were adjusted using memmove() right after the last SRC call (see
					below)
				*/

				src_data.input_frames = leftover_frames;
			}

		} else {
			src_data.data_in = in;
			src_data.input_frames = frames;
		}

		first_time = false;

		if ((err = src_process (src_state, &src_data)) != 0) {
			throw Exception (*this, str (format ("An error occured during sample rate conversion: %1%") % src_strerror (err)));
		}

		leftover_frames = src_data.input_frames - src_data.input_frames_used;

		if (leftover_frames > 0) {
			if (leftover_frames > max_leftover_frames) {
				throw Exception(*this, "leftover frames overflowed");
			}
			memmove (leftover_data, (char *) (src_data.data_in + (src_data.input_frames_used * channels)),
					leftover_frames * channels * sizeof(float));
		}

		output (data_out, src_data.output_frames_gen);

		DEBUG ("src_data.output_frames_gen: " << src_data.output_frames_gen << ", leftover_frames: " << leftover_frames);

	} while (leftover_frames > frames);
}

void SampleRateConverter::set_end_of_input ()
{
	src_data.end_of_input = true;
	
	float dummy;
	process (&dummy, 0);
	process (&dummy, 0);
}


void SampleRateConverter::reset ()
{
	active = false;
	max_frames_in = 0;
	src_data.end_of_input = false;
	
	if (src_state) {
		src_delete (src_state);
	}
	
	leftover_frames = 0;
	max_leftover_frames = 0;
	if (leftover_data) {
		free (leftover_data);
	}
	
	data_out_size = 0;
	delete [] data_out;
}

} // namespace
