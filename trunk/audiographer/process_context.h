#ifndef AUDIOGRAPHER_PROCESS_CONTEXT_H
#define AUDIOGRAPHER_PROCESS_CONTEXT_H

#include "types.h"
#include <set>

namespace AudioGrapher
{

/**
 * Processing context. Constness only applies to data, not flags
 */

template <typename T>
class ProcessContext  {
	
public:

	typedef uint8_t Flag;
	typedef uint8_t ChannelCount;

	enum Flags {
		EndOfInput
	};
	
public:
	
	ProcessContext(T * data = 0, nframes_t frames = 0, ChannelCount channels = 1)
		: _data (data), _frames (frames), _channels (channels) {}
	
	/// \a data points to the array of data to process
	inline T const *            data()     const { return _data; }
	inline T *                  data()           { return _data; }
	
	/// \a frames tells how many frames the array pointed by data contains
	inline nframes_t const &    frames()   const { return _frames; }
	inline nframes_t &          frames()         { return _frames; }
	
	/** \a channels tells how many interleaved channels \a data contains
	  * If \a channels is greater than 1, each channel contains \a frames / \a channels frames of data
	  */
	inline ChannelCount const & channels() const { return _channels; }
	inline ChannelCount &       channels()       { return _channels; }
	
	/// Returns the amount of frames per channel
	inline nframes_t            frames_per_channel() const { return _frames / _channels; }

	/* Flags */
	
	inline bool has_flag (Flag flag)    const { return _flags.find (flag) != _flags.end(); }
	inline bool set_flag (Flag flag)    const { return _flags.insert (flag).second; }
	inline bool remove_flag (Flag flag) const { return _flags.erase (flag); }
	
private:
	T *                    _data;
	nframes_t              _frames;
	ChannelCount           _channels;
	mutable std::set<Flag> _flags;
};

} // namespace

#endif // AUDIOGRAPHER_PROCESS_CONTEXT_H
