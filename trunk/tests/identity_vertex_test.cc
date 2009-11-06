#include "utils.h"
#include "audiographer/identity_vertex.h"

using namespace AudioGrapher;

class IdentityVertexTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE (IdentityVertexTest);
  CPPUNIT_TEST (testProcess);
  CPPUNIT_TEST (testRemoveOutput);
  CPPUNIT_TEST (testClearOutputs);
  CPPUNIT_TEST_SUITE_END ();

  public:
	void setUp()
	{
		frames = 128;
		random_data = Utils::init_random_data(frames);
		
		zero_data = new float[frames];
		memset (zero_data, 0, frames * sizeof(float));

		sink_a.reset (new VectorSink<float>());
		sink_b.reset (new VectorSink<float>());
	}

	void tearDown()
	{
		delete [] random_data;
		delete [] zero_data;
	}

	void testProcess()
	{
		vertex.reset (new IdentityVertex<float>());
		vertex->add_output (sink_a);
		vertex->add_output (sink_b);
		
		nframes_t frames_output = 0;
		
		vertex->process (random_data, frames);
		
		frames_output = sink_a->get_data().size();
		CPPUNIT_ASSERT_EQUAL (frames, frames_output);
		
		frames_output = sink_b->get_data().size();
		CPPUNIT_ASSERT_EQUAL (frames, frames_output);
		
		CPPUNIT_ASSERT (Utils::array_equals (random_data, sink_a->get_array(), frames));
		CPPUNIT_ASSERT (Utils::array_equals (random_data, sink_b->get_array(), frames));
	}
	
	void testRemoveOutput()
	{
		vertex.reset (new IdentityVertex<float>());
		vertex->add_output (sink_a);
		vertex->add_output (sink_b);
		
		vertex->process (random_data, frames);
		
		vertex->remove_output (sink_a);
		vertex->process (zero_data, frames);
		
		CPPUNIT_ASSERT (Utils::array_equals (random_data, sink_a->get_array(), frames));
		CPPUNIT_ASSERT (Utils::array_equals (zero_data, sink_b->get_array(), frames));
	}
	
	void testClearOutputs()
	{
		vertex.reset (new IdentityVertex<float>());
		vertex->add_output (sink_a);
		vertex->add_output (sink_b);
		
		vertex->process (random_data, frames);
		
		vertex->clear_outputs ();
		vertex->process (zero_data, frames);
		
		CPPUNIT_ASSERT (Utils::array_equals (random_data, sink_a->get_array(), frames));
		CPPUNIT_ASSERT (Utils::array_equals (random_data, sink_b->get_array(), frames));
	}

  private:
	boost::shared_ptr<IdentityVertex<float> > vertex;
	boost::shared_ptr<VectorSink<float> > sink_a;
	boost::shared_ptr<VectorSink<float> > sink_b;

	float * random_data;
	float * zero_data;
	nframes_t frames;
};

CPPUNIT_TEST_SUITE_REGISTRATION (IdentityVertexTest);

