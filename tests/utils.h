#ifndef AUDIOGRAPHER_TESTS_UTILS_H
#define AUDIOGRAPHER_TESTS_UTILS_H

// Includes we want almost always

#include <cppunit/extensions/HelperMacros.h>
#include <boost/shared_ptr.hpp>

// includes used in this file

#include "../sink.h"

#include <vector>
#include <cstring>
#include <cstdlib>
#include <ctime>

struct Utils
{
	template<typename T>
	static bool array_equals (T const * a, T const * b, nframes_t frames)
	{
		for (nframes_t i = 0; i < frames; ++i) {
			if (a[i] != b[i]) {
				return false;
			}
		}
		return true;
	}

	static float * init_random_data (nframes_t frames, float range = 1.0)
	{
		unsigned int const granularity = 4096;
		float * data = new float[frames];
		srand (std::time (NULL));
		
		for (nframes_t i = 0; i < frames; ++i) {
			int biased_int = (rand() % granularity) - (granularity / 2);
			data[i] = (range * biased_int) / granularity;
		}
		return data;
	}
};

template<typename T>
class VectorSink : public AudioGrapher::Sink<T>
{
  public:
	void process (T * in, nframes_t frames)
	{
		data.resize (frames);
		memcpy (&data[0], in, frames * sizeof(T));
	}

	std::vector<T> const & get_data() const { return data; }

  private:
	std::vector<T> data;

};

#endif // AUDIOGRAPHER_TESTS_UTILS_H
