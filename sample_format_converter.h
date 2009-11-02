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

	
	void set_clip_floats (bool yn) { clip_floats = yn; }
	void alloc_buffers (nframes_t max_frames);

	// Should be RT safe, check gdither
	void process (float * data, nframes_t frames);

  private:
	uint32_t     channels;
	int          data_width;
	GDither      dither;
	nframes_t    data_out_size;
	TOut *       data_out;

	bool         clip_floats;

};

} // namespace

#endif // AUDIOGRAPHER_SAMPLE_FORMAT_CONVERTER_H
