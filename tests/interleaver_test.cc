#include "utils.h"
#include "../interleaver.h"
#include "../identity_vertex.h"

using namespace AudioGrapher;

class InterleaverTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE (InterleaverTest);
  CPPUNIT_TEST (testUninitialized);
  CPPUNIT_TEST (testInvalidInputIndex);
  CPPUNIT_TEST (testInvalidInputSize);
  CPPUNIT_TEST (testOutputSize);
  CPPUNIT_TEST_SUITE_END ();

  public:
	void setUp()
	{
		frames = 1024;
		random_data = new float[frames];

		interleaver.reset (new Interleaver<float>());
		sink.reset (new VectorSink<float>());
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
		interleaver->init (3, frames);
		CPPUNIT_ASSERT_THROW (interleaver->input (3)->process (random_data, frames), Exception);
	}

	void testInvalidInputSize()
	{
		interleaver->init (3, frames);
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
		interleaver->init (3, frames);

		interleaver->add_output (sink);

		interleaver->input (0)->process (random_data, frames);
		interleaver->input (1)->process (random_data, frames);
		interleaver->input (2)->process (random_data, frames);

		nframes_t expected_frames = frames * 3;
		nframes_t generated_frames = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL (expected_frames, generated_frames);

		nframes_t less_frames = frames / 2;

		interleaver->input (0)->process (random_data, less_frames);
		interleaver->input (1)->process (random_data, less_frames);
		interleaver->input (2)->process (random_data, less_frames);

		expected_frames = less_frames * 3;
		generated_frames = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL (expected_frames, generated_frames);
	}


  private:
	boost::shared_ptr<Interleaver<float> > interleaver;

	boost::shared_ptr<VectorSink<float> > sink;

	float * random_data;
	nframes_t frames;
};

CPPUNIT_TEST_SUITE_REGISTRATION (InterleaverTest);

