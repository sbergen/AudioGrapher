#include "utils.h"
#include "../interleaver.h"

using namespace AudioGrapher;

class InterleaverTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE (InterleaverTest);
  CPPUNIT_TEST (testUninitialized);
  CPPUNIT_TEST (testInvalidInputIndex);
  CPPUNIT_TEST (testInvalidInputSize);
  CPPUNIT_TEST (testOutputSize);
  CPPUNIT_TEST (testZeroInput);
  CPPUNIT_TEST_SUITE_END ();

  public:
	void setUp()
	{
		channels = 3;
		frames = 1024;
		random_data = Utils::init_random_data (frames, 1.0);

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
		CPPUNIT_ASSERT_THROW (interleaver->input(0)->process (random_data, frames), Exception);
	}

	void testInvalidInputIndex()
	{
		CPPUNIT_ASSERT_THROW (interleaver->input (3)->process (random_data, frames), Exception);
	}

	void testInvalidInputSize()
	{
		CPPUNIT_ASSERT_THROW (interleaver->input (0)->process (random_data, frames + 1), Exception);
		
		interleaver->input (0)->process (random_data, frames);
		interleaver->input (1)->process (random_data, frames);
		CPPUNIT_ASSERT_THROW (interleaver->input (2)->process (random_data, frames - 1), Exception);

		interleaver->input (0)->process (random_data, frames - 1);
		interleaver->input (1)->process (random_data, frames - 1);
		CPPUNIT_ASSERT_THROW (interleaver->input (2)->process (random_data, frames), Exception);
	}

	void testOutputSize()
	{
		interleaver->add_output (sink);

		interleaver->input (0)->process (random_data, frames);
		interleaver->input (1)->process (random_data, frames);
		interleaver->input (2)->process (random_data, frames);

		nframes_t expected_frames = frames * channels;
		nframes_t generated_frames = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL (expected_frames, generated_frames);

		nframes_t less_frames = frames / 2;

		interleaver->input (0)->process (random_data, less_frames);
		interleaver->input (1)->process (random_data, less_frames);
		interleaver->input (2)->process (random_data, less_frames);

		expected_frames = less_frames * channels;
		generated_frames = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL (expected_frames, generated_frames);
	}

	void testZeroInput()
	{
		interleaver->add_output (sink);

		// input zero frames to all inputs
		
		interleaver->input (0)->process (random_data, 0);
		interleaver->input (1)->process (random_data, 0);
		interleaver->input (2)->process (random_data, 0);
		
		// NOTE zero input is allowed to be a NOP
		
		// ...now test regular input
		
		interleaver->input (0)->process (random_data, frames);
		interleaver->input (1)->process (random_data, frames);
		interleaver->input (2)->process (random_data, frames);

		nframes_t expected_frames = frames * channels;
		nframes_t generated_frames = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL (expected_frames, generated_frames);
	}


  private:
	boost::shared_ptr<Interleaver<float> > interleaver;

	boost::shared_ptr<VectorSink<float> > sink;

	nframes_t channels;
	float * random_data;
	nframes_t frames;
};

CPPUNIT_TEST_SUITE_REGISTRATION (InterleaverTest);

