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
	
	ProcessContext(T * data = 0, nframes_t frames = 0) : _data (data), _frames (frames) {}
	
	inline T const * data() const { return data; }
	inline T * data() { return data; }

	inline nframes_t const & frames() const { return _frames; }
	inline nframes_t & frames() { return _frames; }

	typedef uint8_t Flag;
	
	enum Flags {
		EndOfInput
	}
	
	bool has_flag (Flag flag) const { return _flags.find (flag) != _flags.end(); }
	bool set_flag (Flag flag) const { return _flags.insert (flag).second; }
	bool remove_flag (Flag flag) const { return _flags.erase (flag); }
	
  private:
	T *                    _data;
	nframes_t              _frames;
	mutable std::set<Flag> _flags;
};

} // namespace

#endif // AUDIOGRAPHER_PROCESS_CONTEXT_H
