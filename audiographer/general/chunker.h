#ifndef AUDIOGRAPHER_CHUNKER_H
#define AUDIOGRAPHER_CHUNKER_H

#include "audiographer/flag_debuggable.h"
#include "audiographer/sink.h"
#include "audiographer/type_utils.h"
#include "audiographer/utils/listed_source.h"

namespace AudioGrapher
{

/// A class that chunks process cycles into equal sized frames
template<typename T = DefaultSampleType>
class Chunker
  : public ListedSource<T>
  , public Sink<T>
  , public FlagDebuggable<>
{
  public:
	/** Constructs a new Chunker with a constant chunk size.
	  * \n NOT RT safe
	  */
	Chunker (nframes_t chunk_size)
	  : chunk_size (chunk_size)
	  , position (0)
	{
		buffer = new T[chunk_size];
		add_supported_flag (ProcessContext<T>::EndOfInput);
	}
	
	~Chunker()
	{
		delete [] buffer;
	}
	
	/** Outputs data in \a context in chunks with the size specified in the constructor.
	  * Note that some calls might not produce any output, while others may produce several.
	  * \n RT safe
	  */
	void process (ProcessContext<T> const & context)
	{
		check_flags (*this, context);
		
		nframes_t frames_left = context.frames();
		nframes_t input_position = 0;
		
		while (position + frames_left >= chunk_size) {
			// Copy from context to buffer
			nframes_t const frames_to_copy = chunk_size - position;
			TypeUtils<T>::copy (&context.data()[input_position], &buffer[position], frames_to_copy);
			
			// Output whole buffer
			ProcessContext<T> c_out (context, buffer, chunk_size);
			ListedSource<T>::output (c_out);
			
			// Update counters
			position = 0;
			input_position += frames_to_copy;
			frames_left -= frames_to_copy;
		}
		
		if (frames_left) {
			// Copy the rest of the data
			TypeUtils<T>::copy (&context.data()[input_position], &buffer[position], frames_left);
			position += frames_left;
		}
		
		if (context.has_flag (ProcessContext<T>::EndOfInput)) {
			ProcessContext<T> c_out (context, buffer, position);
			ListedSource<T>::output (c_out);
		}
	}
	using Sink<T>::process;
	
  private:
	nframes_t chunk_size;
	nframes_t position;
	T * buffer;
	
};

} // namespace

#endif // AUDIOGRAPHER_CHUNKER_H

