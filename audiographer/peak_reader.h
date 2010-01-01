#ifndef AUDIOGRAPHER_PEAK_READER_H
#define AUDIOGRAPHER_PEAK_READER_H

#include "audiographer/core/sink.h"
#include "audiographer/core/routines.h"
#include "listed_source.h"

namespace AudioGrapher
{

class PeakReader : public ListedSource<float>, public Sink<float>
{
  public:
	PeakReader() : peak (0.0) {}
	
	float get_peak() { return peak; }
	void  reset() { peak = 0.0; }

	void process (ProcessContext<float> const & c)
	{
		peak = Routines::compute_peak (c.data(), c.frames(), peak);
		ListedSource<float>::output(c);
	}
	
	void process (ProcessContext<float> & c)
	{
		peak = Routines::compute_peak (c.data(), c.frames(), peak);
		ListedSource<float>::output(c);
	}
	
  private:
	float peak;
};


} // namespace

#endif // AUDIOGRAPHER_PEAK_READER_H
