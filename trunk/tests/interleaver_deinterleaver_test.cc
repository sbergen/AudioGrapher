#include "utils.h"
#include "../interleaver.h"
#include "../deinterleaver.h"

using namespace AudioGrapher;

class InterleaverDeInterleaverTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE (InterleaverDeInterleaverTest);
  CPPUNIT_TEST (testInterleavedInput);
  CPPUNIT_TEST (testDeInterleavedInput);
  CPPUNIT_TEST_SUITE_END ();

  public:
	void setUp()
	{
		channels = 3;
		frames_per_channel = 128;
		total_frames = channels * frames_per_channel;
		
		random_data_a = Utils::init_random_data (total_frames, 1.0);
		random_data_b = Utils::init_random_data (frames_per_channel, 1.0);
		random_data_c = Utils::init_random_data (frames_per_channel, 1.0);

		deinterleaver.reset (new DeInterleaver<float>());
		interleaver.reset (new Interleaver<float>());
		
		sink_a.reset (new VectorSink<float>());
		sink_b.reset (new VectorSink<float>());
		sink_c.reset (new VectorSink<float>());
	}

	void tearDown()
	{
		delete [] random_data_a;
		delete [] random_data_b;
		delete [] random_data_c;
	}

	void testInterleavedInput()
	{
		deinterleaver->init (channels, frames_per_channel);
		interleaver->init (channels, frames_per_channel);
		
		deinterleaver->output (0)->add_output (interleaver->input (0));
		deinterleaver->output (1)->add_output (interleaver->input (1));
		deinterleaver->output (2)->add_output (interleaver->input (2));
		
		interleaver->add_output (sink_a);
		
		// Process and assert
		deinterleaver->process (random_data_a, total_frames);
		CPPUNIT_ASSERT (Utils::array_equals (random_data_a, sink_a->get_array(), total_frames));
		
		// And a second round...
		nframes_t less_frames = (frames_per_channel / 10) * channels;
		deinterleaver->process (random_data_a, less_frames);
		CPPUNIT_ASSERT (Utils::array_equals (random_data_a, sink_a->get_array(), less_frames));
	}
	
	void testDeInterleavedInput()
	{
		deinterleaver->init (channels, frames_per_channel);
		interleaver->init (channels, frames_per_channel);
		
		interleaver->add_output (deinterleaver);
		
		deinterleaver->output (0)->add_output (sink_a);
		deinterleaver->output (1)->add_output (sink_b);
		deinterleaver->output (2)->add_output (sink_c);
		
		// Process and assert
		interleaver->input (0)->process (random_data_a, frames_per_channel);
		interleaver->input (1)->process (random_data_b, frames_per_channel);
		interleaver->input (2)->process (random_data_c, frames_per_channel);
		
		CPPUNIT_ASSERT (Utils::array_equals (random_data_a, sink_a->get_array(), frames_per_channel));
		CPPUNIT_ASSERT (Utils::array_equals (random_data_b, sink_b->get_array(), frames_per_channel));
		CPPUNIT_ASSERT (Utils::array_equals (random_data_c, sink_c->get_array(), frames_per_channel));
		
		// And a second round...
		nframes_t less_frames = frames_per_channel / 5;
		interleaver->input (0)->process (random_data_a, less_frames);
		interleaver->input (1)->process (random_data_b, less_frames);
		interleaver->input (2)->process (random_data_c, less_frames);
		
		CPPUNIT_ASSERT (Utils::array_equals (random_data_a, sink_a->get_array(), less_frames));
		CPPUNIT_ASSERT (Utils::array_equals (random_data_b, sink_b->get_array(), less_frames));
		CPPUNIT_ASSERT (Utils::array_equals (random_data_c, sink_c->get_array(), less_frames));
		
	}

  private:
	boost::shared_ptr<Interleaver<float> > interleaver;
	boost::shared_ptr<DeInterleaver<float> > deinterleaver;
	
	boost::shared_ptr<VectorSink<float> > sink_a;
	boost::shared_ptr<VectorSink<float> > sink_b;
	boost::shared_ptr<VectorSink<float> > sink_c;

	float * random_data_a;
	float * random_data_b;
	float * random_data_c;
	
	nframes_t frames_per_channel;
	nframes_t total_frames;
	unsigned int channels;
};

CPPUNIT_TEST_SUITE_REGISTRATION (InterleaverDeInterleaverTest);

