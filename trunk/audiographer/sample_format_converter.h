#ifndef AUDIOGRAPHER_SAMPLE_FORMAT_CONVERTER_H
#define AUDIOGRAPHER_SAMPLE_FORMAT_CONVERTER_H

#include "listed_source.h"
#include "sink.h"
#include "gdither/gdither_types.h"

namespace AudioGrapher
{

/** Sample format converter that does dithering.
  * This class can only convert floats to either \a float, \a int32_t, \a int16_t, or \a uint8_t 
  */
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
	  */
	SampleFormatConverter (uint32_t channels);
	~SampleFormatConverter ();
	
	/** Initialize and allocate buffers for processing.
	  * \param max_frames maximum number of frames that is allowed to be used in calls to \a process()
	  * \param type dither type from \a DitherType
	  * \param data_width data with in bits
	  * \note If the non-const version of process() is used with floats,
	  *       there is no need to call this function.
	  */
	void init (nframes_t max_frames, DitherType type, int data_width);

	/// Set whether or not clipping to [-1.0, 1.0] should occur when TOut = float. Clipping is off by default
	void set_clip_floats (bool yn) { clip_floats = yn; }
	
	/// Processes data without modifying it
	void process (ProcessContext<float> const & c_in);
	
	/// This version is only different in the case when \a TOut = float, and float clipping is on.
	void process (ProcessContext<float> & c_in);

  private:
	void reset();
	void init_common(nframes_t max_frames); // not-template-specialized part of init
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
