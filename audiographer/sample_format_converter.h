#ifndef AUDIOGRAPHER_SAMPLE_FORMAT_CONVERTER_H
#define AUDIOGRAPHER_SAMPLE_FORMAT_CONVERTER_H

#include "listed_source.h"
#include "sink.h"
#include "gdither/gdither_types.h"

namespace AudioGrapher
{

template <typename TOut>
class SampleFormatConverter : public Sink<float>, public ListedSource<TOut>
{
  public:
	/// Dither types from the gdither library
	enum DitherType
	{
		D_None   = GDitherNone,   ///< No didtering
		D_Rect   = GDitherRect,   ///< Rectangular dithering, i.e. white noise
		D_Tri    = GDitherTri,    ///< Triangular dithering
		D_Shaped = GDitherShaped  ///< Actually noise shaping, only works for 46kHzish signals
	};
	
	/** Constructor
	  * \param channels number of channels in stream
	  * \param type dither type from \a DitherType
	  * \param data_width data with in bits. Only necessary for odd data widths. Only 24 is supported.
	  */
	SampleFormatConverter (uint32_t channels, DitherType type = D_None, int data_width = 0);
	~SampleFormatConverter ();
	
	/** Allocate buffers for processing a maximum of \a max_frames at a time.
	  * \note If the non-const version of process() is used with floats,
	  *       there is no need to call this function.
	  */
	void alloc_buffers (nframes_t max_frames);

	/// Set whether or not clipping to [-1.0, 1.0] should occur when TOut = float. Clipping is off by default
	void set_clip_floats (bool yn) { clip_floats = yn; }
	
	/// Processes data without modifying it
	void process (ProcessContext<float> const & c_in);
	
	/// This version is only different in the case when \a TOut = float, and float clipping is on.
	void process (ProcessContext<float> & c_in);

  private:
	void check_frame_count(nframes_t frames);

	uint32_t     channels;
	int          data_width;
	GDither      dither;
	nframes_t    data_out_size;
	TOut *       data_out;

	bool         clip_floats;

};

} // namespace

#endif // AUDIOGRAPHER_SAMPLE_FORMAT_CONVERTER_H
