#include "utils.h"
#include "../identity_vertex.h"

using namespace AudioGrapher;

class IdentityVertexTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE (IdentityVertexTest);
  CPPUNIT_TEST (testProcess);
  CPPUNIT_TEST_SUITE_END ();

  public:
	void setUp()
	{
		frames = 1024;
		random_data = new float[frames];

		vertex.reset (new IdentityVertex<float>());
		sink_a.reset (new VectorSink<float>());
		sink_b.reset (new VectorSink<float>());

		vertex->add_output (sink_a);
		vertex->add_output (sink_b);
	}

	void tearDown()
	{
		delete [] random_data;
	}

	void testProcess()
	{
		vertex->process (random_data, frames);
		CPPUNIT_ASSERT (array_equals (random_data, &sink_a->get_data()[0], frames));
		CPPUNIT_ASSERT (array_equals (random_data, &sink_b->get_data()[0], frames));
	}

  private:
	boost::shared_ptr<IdentityVertex<float> > vertex;
	boost::shared_ptr<VectorSink<float> > sink_a;
	boost::shared_ptr<VectorSink<float> > sink_b;

	float * random_data;
	nframes_t frames;
};

CPPUNIT_TEST_SUITE_REGISTRATION (IdentityVertexTest);

