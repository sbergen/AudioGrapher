#include "utils.h"
#include "audiographer/interleaver.h"

using namespace AudioGrapher;

class InterleaverTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE (InterleaverTest);
  CPPUNIT_TEST (testUninitialized);
  CPPUNIT_TEST (testInvalidInputIndex);
  CPPUNIT_TEST (testInvalidInputSize);
  CPPUNIT_TEST (testOutputSize);
  CPPUNIT_TEST (testZeroInput);
  CPPUNIT_TEST (testChannelSync);
  CPPUNIT_TEST_SUITE_END ();

  public:
	void setUp()
	{
		channels = 3;
		frames = 128;
		random_data = TestUtils::init_random_data (frames, 1.0);

		interleaver.reset (new Interleaver<float>());
		sink.reset (new VectorSink<float>());
		
		interleaver->init (channels, frames);
	}

	void tearDown()
	{
		delete [] random_data;
	}

	void testUninitialized()
	{
		interleaver.reset (new Interleaver<float>());
		ProcessContext<float> c (random_data, frames);
		CPPUNIT_ASSERT_THROW (interleaver->input(0)->process (c), Exception);
	}

	void testInvalidInputIndex()
	{
		ProcessContext<float> c (random_data, frames);
		CPPUNIT_ASSERT_THROW (interleaver->input (3)->process (c), Exception);
	}

	void testInvalidInputSize()
	{
		ProcessContext<float> c (random_data, frames + 1);
		CPPUNIT_ASSERT_THROW (interleaver->input (0)->process (c), Exception);
		
		c.frames() = frames;
		interleaver->input (0)->process (c);
		interleaver->input (1)->process (c);
		c.frames() = frames -1;
		CPPUNIT_ASSERT_THROW (interleaver->input (2)->process (c), Exception);

		interleaver->input (0)->process (c);
		interleaver->input (1)->process (c);
		c.frames() = frames;
		CPPUNIT_ASSERT_THROW (interleaver->input (2)->process (c), Exception);
	}

	void testOutputSize()
	{
		interleaver->add_output (sink);

		ProcessContext<float> c (random_data, frames);
		interleaver->input (0)->process (c);
		interleaver->input (1)->process (c);
		interleaver->input (2)->process (c);

		nframes_t expected_frames = frames * channels;
		nframes_t generated_frames = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL (expected_frames, generated_frames);

		nframes_t less_frames = frames / 2;
		c.frames() = less_frames;
		interleaver->input (0)->process (c);
		interleaver->input (1)->process (c);
		interleaver->input (2)->process (c);

		expected_frames = less_frames * channels;
		generated_frames = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL (expected_frames, generated_frames);
	}

	void testZeroInput()
	{
		interleaver->add_output (sink);

		// input zero frames to all inputs
		ProcessContext<float> c (random_data, 0);
		interleaver->input (0)->process (c);
		interleaver->input (1)->process (c);
		interleaver->input (2)->process (c);
		
		// NOTE zero input is allowed to be a NOP
		
		// ...now test regular input
		c.frames() = frames;
		interleaver->input (0)->process (c);
		interleaver->input (1)->process (c);
		interleaver->input (2)->process (c);

		nframes_t expected_frames = frames * channels;
		nframes_t generated_frames = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL (expected_frames, generated_frames);
	}

	void testChannelSync()
	{
		interleaver->add_output (sink);
		ProcessContext<float> c (random_data, frames);
		interleaver->input (0)->process (c);
		CPPUNIT_ASSERT_THROW (interleaver->input (0)->process (c), Exception);		
	}


  private:
	boost::shared_ptr<Interleaver<float> > interleaver;

	boost::shared_ptr<VectorSink<float> > sink;

	nframes_t channels;
	float * random_data;
	nframes_t frames;
};

CPPUNIT_TEST_SUITE_REGISTRATION (InterleaverTest);

