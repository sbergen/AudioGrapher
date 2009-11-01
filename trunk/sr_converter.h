#ifndef AUDIOGRAPHER_SR_CONVERTER_H
#define AUDIOGRAPHER_SR_CONVERTER_H

#include <samplerate.h>

#include "types.h"
#include "vertex.h"

namespace AudioGrapher
{

class SampleRateConverter : public Vertex<float, float>
{
  public:
	SampleRateConverter (uint32_t channels, nframes_t in_rate, nframes_t out_rate, int quality);
	~SampleRateConverter ();

	void allocate_buffers (nframes_t max_frames);
	void process (float * data, nframes_t frames);

  private:
	bool           active;
	uint32_t       channels;
	nframes_t      max_frames_in;
	
	float *        leftover_data;
	nframes_t      leftover_frames;
	nframes_t      max_leftover_frames;

	float *        data_out;
	nframes_t      data_out_size;

	SRC_DATA       src_data;
	SRC_STATE*     src_state;
};

} // namespace

#endif // AUDIOGRAPHER_SR_CONVERTER_H
