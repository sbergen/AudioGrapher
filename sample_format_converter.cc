#include "sample_format_converter.h"

/* see gdither.cc for why we have to do this */

#define	_ISOC9X_SOURCE	1
#define _ISOC99_SOURCE	1
#include <cmath>
#undef  _ISOC99_SOURCE
#undef  _ISOC9X_SOURCE
#undef  __USE_SVID
#define __USE_SVID 1
#include <cstdlib>
#undef  __USE_SVID

#include <unistd.h>
#include <inttypes.h>
#include <float.h>
#include <climits>

#include "gdither/gdither.h"
#include "exception.h"

namespace AudioGrapher
{

template <typename TOut>
SampleFormatConverter<TOut>::SampleFormatConverter (uint32_t channels, DitherType type, int data_width) :
  channels (channels),
  data_width (data_width),
  dither (0),
  data_out_size (0),
  data_out (0),
  clip_floats (false)
{
	if (data_width != 24) {
		data_width = sizeof (TOut) * 8;
	}

	GDitherSize dither_size = GDitherFloat;

	switch (data_width) {
	case 8:
		dither_size = GDither8bit;
		break;

	case 16:
		dither_size = GDither16bit;
		break;
	case 24:
		dither_size = GDither32bit;
	}

	dither = gdither_new ((GDitherType) type, channels, dither_size, data_width);
}

template <typename TOut>
SampleFormatConverter<TOut>::~SampleFormatConverter ()
{
	if (dither) {
		gdither_free (dither);
	}

	delete[] data_out;
}

template <typename TOut>
void
SampleFormatConverter<TOut>::alloc_buffers (nframes_t max_frames)
{
	size_t data_size = channels * max_frames;
	if (data_size  > data_out_size) {

		delete[] data_out;

		data_out = new TOut[data_size];
		data_out_size = data_size;
	}
}

template <typename TOut>
void
SampleFormatConverter<TOut>::process (float * data, nframes_t frames)
{
	/* Make sure we have enough memory allocated */

	size_t data_size = channels * frames;
	if (data_size  > data_out_size) {
		throw Exception ("SampleFormatConverter: too many frames given to process()");
	}

	/* Do conversion */

	if (data_width < 32) {
		for (uint32_t chn = 0; chn < channels; ++chn) {
			gdither_runf (dither, chn, frames, data, data_out);
		}
	} else {
		for (uint32_t chn = 0; chn < channels; ++chn) {

			TOut * ob = data_out;
			const double int_max = (float) INT_MAX;
			const double int_min = (float) INT_MIN;

			nframes_t i;
			for (nframes_t x = 0; x < frames; ++x) {
				i = chn + (x * channels);

				if (data[i] > 1.0f) {
					ob[i] = static_cast<TOut> (INT_MAX);
				} else if (data[i] < -1.0f) {
					ob[i] = static_cast<TOut> (INT_MIN);
				} else {
					if (data[i] >= 0.0f) {
						ob[i] = lrintf (int_max * data[i]);
					} else {
						ob[i] = - lrintf (int_min * data[i]);
					}
				}
			}
		}
	}

	/* Write forward */

	output (data_out, frames);
}

template<>
void
SampleFormatConverter<float>::process (float * data, nframes_t frames)
{
	if (clip_floats) {
		for (nframes_t x = 0; x < frames * channels; ++x) {
			if (data[x] > 1.0f) {
				data[x] = 1.0f;
			} else if (data[x] < -1.0f) {
				data[x] = -1.0f;
			}
		}
	}

	output (data, frames);
}

template class SampleFormatConverter<short>;
template class SampleFormatConverter<int>;
template class SampleFormatConverter<float>;

} // namespace
