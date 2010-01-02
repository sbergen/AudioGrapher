#ifndef AUDIOGRAPHER_SNDFILE_BASE_H
#define AUDIOGRAPHER_SNDFILE_BASE_H

#include <string>
#include <sndfile.h>

#include "audiographer/throwing.h"
#include "audiographer/types.h"

namespace AudioGrapher {

/// Common interface for templated libsndfile readers/writers
class SndfileBase
  : public Throwing<>
{
  protected:
	SndfileBase (ChannelCount channels, nframes_t samplerate, int format, std::string const & path);
	virtual ~SndfileBase ();

	std::string    path;
	SF_INFO        sf_info;
	SNDFILE *      sndfile;
};

} // namespace

#endif // AUDIOGRAPHER_SNDFILE_BASE_H