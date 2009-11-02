#include "chunker.h"
#include "deinterleaver.h"
#include "identity_vertex.h"
#include "interleaver.h"
#include "listed_source.h"
#include "sample_format_converter.h"
#include "sndfile_writer.h"
#include "sr_converter.h"
#include "threader.h"

using namespace AudioGrapher;

int main()
{
	Chunker<float> c (1024);
	DeInterleaver<float> di;
	IdentityVertex<float> iv;
	Interleaver<float> il;
	ListedSource<float> ls;
	SampleFormatConverter<int> sfc (2);
	SndfileWriter<int> sndfw (2, 44100, 0, "foo");
	SampleRateConverter src (2);
	
	Glib::ThreadPool pool;
	Threader<float> th (pool);
	
	return 0;
}
