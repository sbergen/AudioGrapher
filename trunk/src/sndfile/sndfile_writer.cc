#include "audiographer/sndfile/sndfile_writer.h"
#include "audiographer/exception.h"

#include <cstring>

#include <boost/format.hpp>

namespace AudioGrapher
{

using std::string;

template <typename T>
SndfileWriter<T>::SndfileWriter (ChannelCount channels, nframes_t samplerate, int format, string const & path) :
  SndfileBase (channels, samplerate, format, path)
{
	// init write function
	init ();
	
	add_supported_flag (ProcessContext<T>::EndOfInput);
}

template <>
void
SndfileWriter<float>::init ()
{
	write_func = &sf_write_float;
}

template <>
void
SndfileWriter<int>::init ()
{
	write_func = &sf_write_int;
}

template <>
void
SndfileWriter<short>::init ()
{
	write_func = &sf_write_short;
}

template <typename T>
void
SndfileWriter<T>::process (ProcessContext<T> const & c)
{
	check_flags (*this, c);
	
	if (throw_level (ThrowStrict) && c.channels() != sf_info.channels) {
		throw Exception (*this, boost::str (boost::format
			("Wrong number of channels given to process(), %1% instead of %2%")
			% c.channels() % sf_info.channels));
	}
	
	nframes_t written = (*write_func) (sndfile, c.data(), c.frames());
	if (throw_level (ThrowProcess) && written != c.frames()) {
		char errbuf[256];
		sf_error_str (sndfile, errbuf, sizeof (errbuf) - 1);
		throw Exception (*this, boost::str ( boost::format
			("Could not write data to output file (%1%)")
			% errbuf));
	}

	if (c.has_flag(ProcessContext<T>::EndOfInput)) {
		sf_write_sync (sndfile);
		//#ifdef HAVE_SIGCPP
		FileWritten (path);
		//#endif
	}
}

template class SndfileWriter<short>;
template class SndfileWriter<int>;
template class SndfileWriter<float>;

} // namespace
