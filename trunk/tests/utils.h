#ifndef AUDIOGRAPHER_TESTS_UTILS_H
#define AUDIOGRAPHER_TESTS_UTILS_H

// Includes we want almost always

#include <cppunit/extensions/HelperMacros.h>
#include <boost/shared_ptr.hpp>

// includes used in this file

#include "audiographer/sink.h"
#include "audiographer/exception.h"

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
	virtual void process (T * in, nframes_t frames)
	{
		data.resize (frames);
		memcpy (&data[0], in, frames * sizeof(T));
	}
	
	virtual void process (AudioGrapher::ProcessContext<T> const & c)
	{
		data.resize (c.frames());
		memcpy (&data[0], c.data(), c.frames() * sizeof(T));
	}

	std::vector<T> const & get_data() const { return data; }
	T const * get_array() const { return &data[0]; }
	void reset() { data.clear(); }

  protected:
	std::vector<T> data;

};

template<typename T>
class AppendingVectorSink : public VectorSink<T>
{
  public:
	void process (T * in, nframes_t frames)
	{
		std::vector<T> & data (VectorSink<T>::data);
		data.resize (total_frames + frames);
		memcpy (&data[total_frames], in, frames * sizeof(T));
		total_frames += frames;
	}

	void reset ()
	{
		total_frames = 0;
		VectorSink<T>::reset();
	}

  private:
	nframes_t total_frames;
};


template<typename T>
class ThrowingSink : public AudioGrapher::Sink<T>
{
  public:
	void process (T *, nframes_t)
	{
		throw AudioGrapher::Exception(*this, "ThrowingSink threw!");
	}
};

#endif // AUDIOGRAPHER_TESTS_UTILS_H
