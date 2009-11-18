#include "utils.h"
#include "audiographer/deinterleaver.h"

using namespace AudioGrapher;

class DeInterleaverTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE (DeInterleaverTest);
  CPPUNIT_TEST (testUninitialized);
  CPPUNIT_TEST (testInvalidOutputIndex);
  CPPUNIT_TEST (testInvalidInputSize);
  CPPUNIT_TEST (testOutputSize);
  CPPUNIT_TEST (testZeroInput);
  CPPUNIT_TEST_SUITE_END ();

  public:
	void setUp()
	{
		channels = 3;
		frames_per_channel = 128;
		total_frames = channels * frames_per_channel;
		random_data = TestUtils::init_random_data (total_frames, 1.0);

		deinterleaver.reset (new DeInterleaver<float>());
		sink_a.reset (new VectorSink<float>());
		sink_b.reset (new VectorSink<float>());
		sink_c.reset (new VectorSink<float>());
	}

	void tearDown()
	{
		delete [] random_data;
	}

	void testUninitialized()
	{
		deinterleaver.reset (new DeInterleaver<float>());
		CPPUNIT_ASSERT_THROW (deinterleaver->output(0)->add_output (sink_a), Exception);
	}

	void testInvalidOutputIndex()
	{
		deinterleaver->init (3, frames_per_channel);
		CPPUNIT_ASSERT_THROW (deinterleaver->output(3)->add_output (sink_a), Exception);
	}

	void testInvalidInputSize()
	{
		deinterleaver->init (channels, frames_per_channel);
		
		ProcessContext<float> c (random_data, 0);
		
		// Too many, frames % channels == 0
		c.frames() = total_frames + channels;
		CPPUNIT_ASSERT_THROW (deinterleaver->process (c), Exception);
		
		// Too many, frames % channels != 0
		c.frames() =  total_frames + 1;
		CPPUNIT_ASSERT_THROW (deinterleaver->process (c), Exception);
		
		// Too few, frames % channels != 0
		c.frames() =  total_frames - 1;
		CPPUNIT_ASSERT_THROW (deinterleaver->process (c), Exception);
	}

	void assert_outputs (nframes_t expected_frames)
	{
		nframes_t generated_frames = 0;
		
		generated_frames = sink_a->get_data().size();
		CPPUNIT_ASSERT_EQUAL (expected_frames, generated_frames);
		
		generated_frames = sink_b->get_data().size();
		CPPUNIT_ASSERT_EQUAL (expected_frames, generated_frames);
		
		generated_frames = sink_c->get_data().size();
		CPPUNIT_ASSERT_EQUAL (expected_frames, generated_frames);
	}

	void testOutputSize()
	{
		deinterleaver->init (channels, frames_per_channel);
		
		deinterleaver->output (0)->add_output (sink_a);
		deinterleaver->output (1)->add_output (sink_b);
		deinterleaver->output (2)->add_output (sink_c);
		
		// Test maximum frame input
		ProcessContext<float> c (random_data, total_frames, channels);
		deinterleaver->process (c);
		assert_outputs (frames_per_channel);
		
		// Now with less frames
		nframes_t const less_frames = frames_per_channel / 4;
		c.frames() = less_frames * channels;
		deinterleaver->process (c);
		assert_outputs (less_frames);
	}
	
	void testZeroInput()
	{
		deinterleaver->init (channels, frames_per_channel);
		
		deinterleaver->output (0)->add_output (sink_a);
		deinterleaver->output (1)->add_output (sink_b);
		deinterleaver->output (2)->add_output (sink_c);
		
		// Input zero frames
		ProcessContext<float> c (random_data, 0, channels);
		deinterleaver->process (c);
		
		// ...and now test regular input
		c.frames() = total_frames;
		deinterleaver->process (c);
		assert_outputs (frames_per_channel);
	}


  private:
	boost::shared_ptr<DeInterleaver<float> > deinterleaver;

	boost::shared_ptr<VectorSink<float> > sink_a;
	boost::shared_ptr<VectorSink<float> > sink_b;
	boost::shared_ptr<VectorSink<float> > sink_c;

	float * random_data;
	nframes_t frames_per_channel;
	nframes_t total_frames;
	unsigned int channels;
};

CPPUNIT_TEST_SUITE_REGISTRATION (DeInterleaverTest);

