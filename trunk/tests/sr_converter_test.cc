#include "utils.h"
#include "audiographer/sr_converter.h"

using namespace AudioGrapher;

class SampleRateConverterTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE (SampleRateConverterTest);
  CPPUNIT_TEST (testNoConversion);
  CPPUNIT_TEST (testUpsampleLength);
  CPPUNIT_TEST (testDownsampleLength);
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
		assert (frames % 2 == 0);
		nframes_t const half_frames = frames / 2;
		nframes_t frames_output = 0;
		
		converter->init (44100, 44100);
		converter->add_output (sink);
		
		ProcessContext<float> c (random_data, half_frames);
		converter->process (c);
		ProcessContext<float> c2 (&random_data[half_frames], half_frames);
		converter->process (c2);
		converter->set_end_of_input();
		
		frames_output = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL (frames, frames_output);
		
		CPPUNIT_ASSERT (Utils::array_equals (random_data, sink->get_array(), frames));
	}

	void testUpsampleLength()
	{
		assert (frames % 2 == 0);
		nframes_t const half_frames = frames / 2;
		nframes_t frames_output = 0;
		
		converter->init (44100, 88200);
		converter->allocate_buffers (half_frames);
		converter->add_output (sink);
		
		ProcessContext<float> c (random_data, half_frames);
		converter->process (c);
		ProcessContext<float> c2 (&random_data[half_frames], half_frames);
		converter->process (c2);
		converter->set_end_of_input();

		frames_output = sink->get_data().size();
		nframes_t tolerance = 3;
		CPPUNIT_ASSERT (2 * frames - tolerance < frames_output && frames_output < 2 * frames + tolerance);
	}

	void testDownsampleLength()
	{
		assert (frames % 2 == 0);
		nframes_t const half_frames = frames / 2;
		nframes_t frames_output = 0;
		
		converter->init (88200, 44100);
		converter->allocate_buffers (half_frames);
		converter->add_output (sink);
		
		ProcessContext<float> c (random_data, half_frames);
		converter->process (c);
		ProcessContext<float> c2 (&random_data[half_frames], half_frames);
		converter->process (c2);
		converter->set_end_of_input();
		
		frames_output = sink->get_data().size();
		nframes_t tolerance = 3;
		CPPUNIT_ASSERT (half_frames - tolerance < frames_output && frames_output < half_frames + tolerance);
	}
	

  private:
	boost::shared_ptr<SampleRateConverter > converter;
	boost::shared_ptr<AppendingVectorSink<float> > sink;

	float * random_data;
	nframes_t frames;
};

CPPUNIT_TEST_SUITE_REGISTRATION (SampleRateConverterTest);

