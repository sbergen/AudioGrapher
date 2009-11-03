#ifndef AUDIOGRAPHER_CHUNKER_H
#define AUDIOGRAPHER_CHUNKER_H

#include "listed_source.h"
#include "sink.h"
#include <cstring>

namespace AudioGrapher
{

template<typename T>
class Chunker : public ListedSource<T>, public Sink<T>
{
  public:
	Chunker (nframes_t chunk_size)
	  : chunk_size (chunk_size)
	  , position (0)
	{
		buffer = new T[chunk_size];
	}
	
	~Chunker()
	{
		delete [] buffer;
	}
	
	void process (T * data, nframes_t frames)
	{
		if (position + frames < chunk_size) {
			memcpy (&buffer[position], data, frames * sizeof(T));
			position += frames;
		} else {
			nframes_t const frames_to_copy = chunk_size - position;
			memcpy (&buffer[position], data, frames_to_copy * sizeof(T));
			output (buffer, chunk_size);
			
			memcpy (buffer, &data[frames_to_copy], (frames - frames_to_copy) * sizeof(T));
			position =  frames - frames_to_copy;
		}
	}
	
  private:
	nframes_t chunk_size;
	nframes_t position;
	T * buffer;
	
};

} // namespace

#endif // AUDIOGRAPHER_CHUNKER_H

