#include "audiographer/sndfile_writer.h"
#include "audiographer/exception.h"

#include <string.h>

#include <boost/format.hpp>

namespace AudioGrapher
{

using std::string;
using boost::str;
using boost::format;

/* SndfileWriterBase */

SndfileWriterBase::SndfileWriterBase (int channels, nframes_t samplerate, int format, string const & path)
  : path (path)
  , end_of_input (false)
{
	char errbuf[256];

	sf_info.channels = channels;
	sf_info.samplerate = samplerate;
	sf_info.format = format;

	if (!sf_format_check (&sf_info)) {
		throw Exception (*this, "Invalid format in constructor");
	}

	if (path.length() == 0) {
		throw Exception (*this, "No output file specified");
	}

	/* TODO add checks that the directory path exists, and also
	   check if we are overwriting an existing file...
	*/

	// Open file TODO make sure we have enough disk space for the output
	if (path.compare ("temp")) {
		if ((sndfile = sf_open (path.c_str(), SFM_WRITE, &sf_info)) == 0) {
			sf_error_str (0, errbuf, sizeof (errbuf) - 1);
			throw Exception (*this, str (boost::format ("Cannot open output file \"%1%\" (%2%)") % path % errbuf));
		}
	} else {
		FILE * file;
		if (!(file = tmpfile ())) {
			throw Exception (*this, "Cannot open tempfile");
		}
		sndfile = sf_open_fd (fileno(file), SFM_RDWR, &sf_info, true);
	}
}

SndfileWriterBase::~SndfileWriterBase ()
{
	sf_close (sndfile);
}

/* SndfileWriter */

template <typename T>
SndfileWriter<T>::SndfileWriter (int channels, nframes_t samplerate, int format, string const & path) :
  SndfileWriterBase (channels, samplerate, format, path)
{
	// init write function
	init ();
}

template <>
void
SndfileWriter<float>::init ()
{
	process_func = &sf_writef_float;
}

template <>
void
SndfileWriter<int>::init ()
{
	process_func = &sf_writef_int;
}

template <>
void
SndfileWriter<short>::init ()
{
	process_func = &sf_writef_short;
}

template <typename T>
void
SndfileWriter<T>::process (T * data, nframes_t frames)
{
	char errbuf[256];
	nframes_t written = (*process_func) (sndfile, data, frames);
	if (written != frames) {
		sf_error_str (sndfile, errbuf, sizeof (errbuf) - 1);
		throw Exception (*this, str ( format("Could not write data to output file (%1%)") % errbuf));
	}

	if (end_of_input) {
		sf_write_sync (sndfile);
	}
}

template class SndfileWriter<short>;
template class SndfileWriter<int>;
template class SndfileWriter<float>;

} // namespace
