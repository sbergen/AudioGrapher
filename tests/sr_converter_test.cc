#include "utils.h"
#include "../sr_converter.h"

using namespace AudioGrapher;

class SampleRateConverterTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE (SampleRateConverterTest);
  CPPUNIT_TEST (testNoConversion);
  CPPUNIT_TEST_SUITE_END ();

  public:
	void setUp()
	{
		frames = 128;
		random_data = Utils::init_random_data(frames);
		sink.reset (new AppendingVectorSink<float>());
		converter.reset (new SampleRateConverter (1));
	}

	void tearDown()
	{
		delete [] random_data;
	}

	void testNoConversion()
	{
		converter->init (44100, 44100);
		converter->add_output (sink);
		
		assert (frames % 2 == 0);
		nframes_t const half_frames = frames / 2;
		nframes_t frames_output = 0;
		
		converter->process (random_data, half_frames);
		converter->set_end_of_input();
		converter->process (&random_data[half_frames], half_frames);
		
		frames_output = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL (frames, frames_output);
		
		CPPUNIT_ASSERT (Utils::array_equals (random_data, sink->get_array(), frames));
	}
	

  private:
	boost::shared_ptr<SampleRateConverter > converter;
	boost::shared_ptr<AppendingVectorSink<float> > sink;

	float * random_data;
	nframes_t frames;
};

CPPUNIT_TEST_SUITE_REGISTRATION (SampleRateConverterTest);

