#include "utils.h"

#include "audiographer/silence_trimmer.h"

using namespace AudioGrapher;

class SilenceTrimmerTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE (SilenceTrimmerTest);
  CPPUNIT_TEST (testExceptions);
  CPPUNIT_TEST (testFullBuffers);
  CPPUNIT_TEST (testPartialBuffers);
  CPPUNIT_TEST_SUITE_END ();

  public:
	void setUp()
	{
		frames = 128;
		random_data = TestUtils::init_random_data(frames);
		zero_data = new float[frames];
		memset(zero_data, 0, frames * sizeof(float));
		
		half_random_data = TestUtils::init_random_data(frames);
		memset(half_random_data, 0, (frames / 2) * sizeof(float));
		
		trimmer.reset (new SilenceTrimmer<float>());
		sink.reset (new AppendingVectorSink<float>());
	}

	void tearDown()
	{
		delete [] random_data;
		delete [] zero_data;
		delete [] half_random_data;
		
		AudioGrapher::Utils::free_resources();
	}

	void testFullBuffers()
	{
		trimmer->add_output (sink);
		AudioGrapher::Utils::init_zeros<float>(frames / 2);
		
		{
		ProcessContext<float> c (zero_data, frames);
		trimmer->process (c);
		nframes_t frames_processed = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL ((nframes_t) 0, frames_processed);
		}
		
		{
		ProcessContext<float> c (random_data, frames);
		trimmer->process (c);
		nframes_t frames_processed = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL (frames, frames_processed);
		CPPUNIT_ASSERT (TestUtils::array_equals (sink->get_array(), random_data, frames));
		}
		
		{
		ProcessContext<float> c (zero_data, frames);
		trimmer->process (c);
		nframes_t frames_processed = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL (frames, frames_processed);
		}
		
		{
		ProcessContext<float> c (random_data, frames);
		trimmer->process (c);
		nframes_t frames_processed = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL (3 * frames, frames_processed);
		CPPUNIT_ASSERT (TestUtils::array_equals (sink->get_array(), random_data, frames));
		CPPUNIT_ASSERT (TestUtils::array_equals (&sink->get_array()[frames], zero_data, frames));
		CPPUNIT_ASSERT (TestUtils::array_equals (&sink->get_array()[2 * frames], random_data, frames));
		}
		
		{
		ProcessContext<float> c (zero_data, frames);
		trimmer->process (c);
		nframes_t frames_processed = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL (3 * frames, frames_processed);
		}
	}
	
	void testPartialBuffers()
	{
		trimmer->add_output (sink);
		AudioGrapher::Utils::init_zeros<float>(frames / 4);
		
		{
		ProcessContext<float> c (half_random_data, frames);
		trimmer->process (c);
		nframes_t frames_processed = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL (frames / 2, frames_processed);
		CPPUNIT_ASSERT (TestUtils::array_equals (sink->get_array(), &half_random_data[frames / 2], frames / 2));
		}
		
		{
		ProcessContext<float> c (zero_data, frames);
		trimmer->process (c);
		nframes_t frames_processed = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL (frames / 2, frames_processed);
		}
		
		{
		ProcessContext<float> c (half_random_data, frames);
		trimmer->process (c);
		nframes_t frames_processed = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL (2 * frames + frames / 2, frames_processed);
		CPPUNIT_ASSERT (TestUtils::array_equals (&sink->get_array()[frames + frames / 2], half_random_data, frames));
		}
	}
	
	void testExceptions()
	{
		trimmer->add_output (sink);
		
		{
		ProcessContext<float> c (random_data, frames);
		trimmer->process (c);
		}
		
		{
		ProcessContext<float> c (zero_data, frames);
		trimmer->process (c);
		}
		
		{
		ProcessContext<float> c (random_data, frames);
		CPPUNIT_ASSERT_THROW (trimmer->process (c), Exception);
		}
	}

  private:
	boost::shared_ptr<SilenceTrimmer<float> > trimmer;
	boost::shared_ptr<AppendingVectorSink<float> > sink;

	float * random_data;
	float * zero_data;
	float * half_random_data;
	nframes_t frames;
};

CPPUNIT_TEST_SUITE_REGISTRATION (SilenceTrimmerTest);
