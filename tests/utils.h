// Includes we want almost always

#include <cppunit/extensions/HelperMacros.h>
#include <boost/shared_ptr.hpp>

// includes used in this file

#include "../sink.h"

#include <vector>
#include <cstring>

template<typename T>
bool array_equals (T const * a, T const * b, nframes_t frames)
{
	for (nframes_t i = 0; i < frames; ++i) {
		if (a[i] != b[i]) { return false; }
	}
	return true;
}

template<typename T>
class VectorSink : public AudioGrapher::Sink<T>
{
  public:
	void process (T * in, nframes_t frames)
	{
		data.resize (frames);
		memcpy (&data[0], in, frames);
	}

	std::vector<T> const & get_data() const { return data; }

  private:
	std::vector<T> data;

};
