#include "tests/utils.h"

#include "audiographer/general/chunker.h"

#include <cassert>

using namespace AudioGrapher;

class ChunkerTest : public CppUnit::TestFixture
{
	// TODO: Test EndOfInput handling

  CPPUNIT_TEST_SUITE (ChunkerTest);
  CPPUNIT_TEST (testSynchronousProcess);
  CPPUNIT_TEST (testAsynchronousProcess);
  CPPUNIT_TEST (testChoppingProcess);
  CPPUNIT_TEST_SUITE_END ();

  public:
	void setUp()
	{
		frames = 128;
		random_data = TestUtils::init_random_data(frames);
		sink.reset (new VectorSink<float>());
		chunker.reset (new Chunker<float>(frames * 2));
	}

	void tearDown()
	{
		delete [] random_data;
	}

	void testSynchronousProcess()
	{
		chunker->add_output (sink);
		nframes_t frames_output = 0;
		
		ProcessContext<float> const context (random_data, frames, 1);
		
		chunker->process (context);
		frames_output = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL ((nframes_t) 0, frames_output);
		
		chunker->process (context);
		frames_output = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL (2 * frames, frames_output);
		CPPUNIT_ASSERT (TestUtils::array_equals (random_data, sink->get_array(), frames));
		CPPUNIT_ASSERT (TestUtils::array_equals (random_data, &sink->get_array()[frames], frames));
		
		sink->reset();
		
		chunker->process (context);
		frames_output = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL ((nframes_t) 0, frames_output);
		
		chunker->process (context);
		frames_output = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL (2 * frames, frames_output);
		CPPUNIT_ASSERT (TestUtils::array_equals (random_data, sink->get_array(), frames));
		CPPUNIT_ASSERT (TestUtils::array_equals (random_data, &sink->get_array()[frames], frames));
	}
	
	void testAsynchronousProcess()
	{
		assert (frames % 2 == 0);
		
		chunker->add_output (sink);
		nframes_t frames_output = 0;
		
		ProcessContext<float> const half_context (random_data, frames / 2, 1);
		ProcessContext<float> const context (random_data, frames, 1);
		
		// 0.5
		chunker->process (half_context);
		frames_output = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL ((nframes_t) 0, frames_output);
		
		// 1.5
		chunker->process (context);
		frames_output = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL ((nframes_t) 0, frames_output);
		
		// 2.5
		chunker->process (context);
		frames_output = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL (2 * frames, frames_output);
		CPPUNIT_ASSERT (TestUtils::array_equals (random_data, sink->get_array(), frames / 2));
		CPPUNIT_ASSERT (TestUtils::array_equals (random_data, &sink->get_array()[frames / 2], frames));
		CPPUNIT_ASSERT (TestUtils::array_equals (random_data, &sink->get_array()[ 3 * frames / 2], frames / 2));
		
		sink->reset();
		
		// 3.5
		chunker->process (context);
		frames_output = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL ((nframes_t) 0, frames_output);
		
		// 4.0
		chunker->process (half_context);
		frames_output = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL (2 * frames, frames_output);
		CPPUNIT_ASSERT (TestUtils::array_equals (&random_data[frames / 2], sink->get_array(), frames / 2));
		CPPUNIT_ASSERT (TestUtils::array_equals (random_data, &sink->get_array()[frames / 2], frames));
		CPPUNIT_ASSERT (TestUtils::array_equals (random_data, &sink->get_array()[ 3 * frames / 2], frames / 2));
	}
	
	void testChoppingProcess()
	{
		sink.reset (new AppendingVectorSink<float>());
		
		assert (frames % 2 == 0);
		chunker.reset (new Chunker<float>(frames / 4));
		
		chunker->add_output (sink);
		nframes_t frames_output = 0;
		
		ProcessContext<float> const half_context (random_data, frames / 2, 1);
		ProcessContext<float> const context (random_data, frames, 1);
		
		// 0.5
		chunker->process (half_context);
		frames_output = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL ((nframes_t) frames / 2, frames_output);
		
		// 1.5
		chunker->process (context);
		frames_output = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL ((nframes_t) frames / 2 * 3, frames_output);
		
		// 2.5
		chunker->process (context);
		frames_output = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL (frames / 2 * 5, frames_output);
		CPPUNIT_ASSERT (TestUtils::array_equals (random_data, sink->get_array(), frames / 2));
		CPPUNIT_ASSERT (TestUtils::array_equals (random_data, &sink->get_array()[frames / 2], frames));
		CPPUNIT_ASSERT (TestUtils::array_equals (random_data, &sink->get_array()[ 3 * frames / 2], frames / 2));
	}

  private:
	boost::shared_ptr<Chunker<float> > chunker;
	boost::shared_ptr<VectorSink<float> > sink;

	float * random_data;
	nframes_t frames;
};

CPPUNIT_TEST_SUITE_REGISTRATION (ChunkerTest);

