#ifndef AUDIOGRAPHER_SNDFILE_WRITER_H
#define AUDIOGRAPHER_SNDFILE_WRITER_H

#include <sndfile.h>
#include <string>

#include "types.h"
#include "sink.h"

namespace AudioGrapher
{

/// Common interface for templated libsndfile writers
class SndfileWriterBase
{
  public:

	SNDFILE * get_sndfile () const { return sndfile; }

  protected:
	SndfileWriterBase (int channels, nframes_t samplerate, int format, std::string const & path);
	virtual ~SndfileWriterBase ();

	std::string    path;
	SF_INFO        sf_info;
	SNDFILE *      sndfile;
};


/// Template parameter specific parts of sndfile writer
template <typename T>
class SndfileWriter : public SndfileWriterBase, public Sink<T>
{
  public:
	SndfileWriter (int channels, nframes_t samplerate, int format, std::string const & path);
	void process (ProcessContext<T> const & c);
	virtual ~SndfileWriter () {}

  protected:

	sf_count_t (*process_func)(SNDFILE *, const T *, sf_count_t);

  private:
	void init (); // Inits write function
};

} // namespace

#endif // AUDIOGRAPHER_SNDFILE_WRITER_H