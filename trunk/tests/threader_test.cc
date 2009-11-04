#include "utils.h"
#include "../threader.h"

using namespace AudioGrapher;

class ThreaderTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE (ThreaderTest);
  CPPUNIT_TEST (testProcess);
  CPPUNIT_TEST (testRemoveOutput);
  CPPUNIT_TEST (testClearOutputs);
  CPPUNIT_TEST (testExceptions);
  CPPUNIT_TEST_SUITE_END ();

  public:
	void setUp()
	{
		frames = 1024;
		random_data = Utils::init_random_data (frames, 1.0);
		
		zero_data = new float[frames];
		memset (zero_data, 0, frames * sizeof(float));
		
		thread_pool = new Glib::ThreadPool (3);
		threader.reset (new Threader<float> (*thread_pool));
		
		sink_a.reset (new VectorSink<float>());
		sink_b.reset (new VectorSink<float>());
		sink_c.reset (new VectorSink<float>());
		sink_d.reset (new VectorSink<float>());
		sink_e.reset (new VectorSink<float>());
		sink_f.reset (new VectorSink<float>());
		
		throwing_sink.reset (new ThrowingSink<float>());
	}

	void tearDown()
	{
		delete [] random_data;
		delete [] zero_data;
		
		thread_pool->shutdown();
		delete thread_pool;
	}

	void testProcess()
	{
		threader->add_output (sink_a);
		threader->add_output (sink_b);
		threader->add_output (sink_c);
		threader->add_output (sink_d);
		threader->add_output (sink_e);
		threader->add_output (sink_f);
		
		threader->process (random_data, frames);
		
		CPPUNIT_ASSERT (Utils::array_equals(random_data, sink_a->get_array(), frames));
		CPPUNIT_ASSERT (Utils::array_equals(random_data, sink_b->get_array(), frames));
		CPPUNIT_ASSERT (Utils::array_equals(random_data, sink_c->get_array(), frames));
		CPPUNIT_ASSERT (Utils::array_equals(random_data, sink_d->get_array(), frames));
		CPPUNIT_ASSERT (Utils::array_equals(random_data, sink_e->get_array(), frames));
		CPPUNIT_ASSERT (Utils::array_equals(random_data, sink_f->get_array(), frames));
	}
	
	void testRemoveOutput()
	{
		threader->add_output (sink_a);
		threader->add_output (sink_b);
		threader->add_output (sink_c);
		threader->add_output (sink_d);
		threader->add_output (sink_e);
		threader->add_output (sink_f);
		
		threader->process (random_data, frames);
		
		// Remove a, b and f
		threader->remove_output (sink_a);
		threader->remove_output (sink_b);
		threader->remove_output (sink_f);
		
		threader->process (zero_data, frames);
		
		CPPUNIT_ASSERT (Utils::array_equals(random_data, sink_a->get_array(), frames));
		CPPUNIT_ASSERT (Utils::array_equals(random_data, sink_b->get_array(), frames));
		CPPUNIT_ASSERT (Utils::array_equals(zero_data, sink_c->get_array(), frames));
		CPPUNIT_ASSERT (Utils::array_equals(zero_data, sink_d->get_array(), frames));
		CPPUNIT_ASSERT (Utils::array_equals(zero_data, sink_e->get_array(), frames));
		CPPUNIT_ASSERT (Utils::array_equals(random_data, sink_f->get_array(), frames));
	}
	
	void testClearOutputs()
	{
		threader->add_output (sink_a);
		threader->add_output (sink_b);
		threader->add_output (sink_c);
		threader->add_output (sink_d);
		threader->add_output (sink_e);
		threader->add_output (sink_f);
		
		threader->process (random_data, frames);
		
		threader->clear_outputs();
		threader->process (zero_data, frames);
		
		CPPUNIT_ASSERT (Utils::array_equals(random_data, sink_a->get_array(), frames));
		CPPUNIT_ASSERT (Utils::array_equals(random_data, sink_b->get_array(), frames));
		CPPUNIT_ASSERT (Utils::array_equals(random_data, sink_c->get_array(), frames));
		CPPUNIT_ASSERT (Utils::array_equals(random_data, sink_d->get_array(), frames));
		CPPUNIT_ASSERT (Utils::array_equals(random_data, sink_e->get_array(), frames));
		CPPUNIT_ASSERT (Utils::array_equals(random_data, sink_f->get_array(), frames));
	}
	
	void testExceptions()
	{
		threader->add_output (sink_a);
		threader->add_output (sink_b);
		threader->add_output (sink_c);
		threader->add_output (throwing_sink);
		threader->add_output (sink_e);
		threader->add_output (throwing_sink);
		
		CPPUNIT_ASSERT_THROW (threader->process (random_data, frames), Exception);
		
		CPPUNIT_ASSERT (Utils::array_equals(random_data, sink_a->get_array(), frames));
		CPPUNIT_ASSERT (Utils::array_equals(random_data, sink_b->get_array(), frames));
		CPPUNIT_ASSERT (Utils::array_equals(random_data, sink_c->get_array(), frames));
		CPPUNIT_ASSERT (Utils::array_equals(random_data, sink_e->get_array(), frames));
	}

  private:
	Glib::ThreadPool * thread_pool;
	
	boost::shared_ptr<Threader<float> > threader;
	boost::shared_ptr<VectorSink<float> > sink_a;
	boost::shared_ptr<VectorSink<float> > sink_b;
	boost::shared_ptr<VectorSink<float> > sink_c;
	boost::shared_ptr<VectorSink<float> > sink_d;
	boost::shared_ptr<VectorSink<float> > sink_e;
	boost::shared_ptr<VectorSink<float> > sink_f;
	
	boost::shared_ptr<ThrowingSink<float> > throwing_sink;

	float * random_data;
	float * zero_data;
	nframes_t frames;
};

CPPUNIT_TEST_SUITE_REGISTRATION (ThreaderTest);

