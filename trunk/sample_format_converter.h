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
	enum DitherType
	{
		D_None = GDitherNone,
		D_Rect = GDitherRect,
		D_Tri = GDitherTri,
		D_Shaped = GDitherShaped
	};
	
	SampleFormatConverter (uint32_t channels, DitherType type = D_None, int data_width = 0);
	~SampleFormatConverter ();
	
	/** Set whether or not clipping to [-1.0, 1.0] should occur when TOut = float */
	void set_clip_floats (bool yn) { clip_floats = yn; }
	
	/** Allocate buffers for processing a maximum of @a max_frames at a time.
	  * @note If the non-const version of process() is used with floats,
	  *       there is no need to call this function.
	  */
	void alloc_buffers (nframes_t max_frames);

	// Should be RT safe, check gdither
	void process (ProcessContext<float> const & c_in);
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
