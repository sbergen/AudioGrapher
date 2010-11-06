#include "tests/utils.h"

#include "audiographer/general/sr_converter.h"

using namespace AudioGrapher;

class SampleRateConverterTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE (SampleRateConverterTest);
  CPPUNIT_TEST (testNoConversion);
  CPPUNIT_TEST (testUpsampleLength);
  CPPUNIT_TEST (testDownsampleLength);
  CPPUNIT_TEST (testRespectsEndOfInput);
  CPPUNIT_TEST (testUpsampleDownsampleFuzzyCompare);
  CPPUNIT_TEST_SUITE_END ();

  public:
	void setUp()
	{
		frames = 128;
		
		random_data = TestUtils::init_random_data(frames);
		sine_data = TestUtils::init_sine_data(frames, frames / 4, 2);
		
		sink.reset (new AppendingVectorSink<float>());
		sink2.reset (new AppendingVectorSink<float>());
		grabber.reset (new ProcessContextGrabber<float>());
		converter.reset (new SampleRateConverter (2));
	}

	void tearDown()
	{
		delete [] random_data;
		delete [] sine_data;
	}

	void testNoConversion()
	{
		assert (frames % 4 == 0);
		nframes_t const half_frames = frames / 2;
		nframes_t frames_output = 0;
		
		converter->init (44100, 44100);
		converter->add_output (sink);
		
		ConstProcessContext<float> c (random_data, half_frames, 2);
		converter->process (c);
		ConstProcessContext<float> c2 (&random_data[half_frames], half_frames, 2);
		c2().set_flag (ProcessContext<float>::EndOfInput);
		converter->process (c2);
		
		frames_output = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL (frames, frames_output);
		
		CPPUNIT_ASSERT (TestUtils::array_equals (random_data, sink->get_array(), frames));
	}

	void testUpsampleLength()
	{
		assert (frames % 4 == 0);
		nframes_t const half_frames = frames / 2;
		nframes_t frames_output = 0;
		
		converter->init (44100, 88200);
		converter->allocate_buffers (half_frames);
		converter->add_output (sink);
		
		ConstProcessContext<float> c (random_data, half_frames, 2);
		converter->process (c);
		ConstProcessContext<float> c2 (&random_data[half_frames], half_frames, 2);
		c2().set_flag (ProcessContext<float>::EndOfInput);
		converter->process (c2);

		frames_output = sink->get_data().size();
		nframes_t tolerance = 3;
		CPPUNIT_ASSERT (TestUtils::fuzzy_compare(2 * frames, frames_output, tolerance));
	}

	void testDownsampleLength()
	{
		assert (frames % 4 == 0);
		nframes_t const half_frames = frames / 2;
		nframes_t frames_output = 0;
		
		converter->init (88200, 44100);
		converter->allocate_buffers (half_frames);
		converter->add_output (sink);
		
		ConstProcessContext<float> c (random_data, half_frames, 2);
		converter->process (c);
		ConstProcessContext<float> c2 (&random_data[half_frames], half_frames, 2);
		c2().set_flag (ProcessContext<float>::EndOfInput);
		converter->process (c2);
		
		frames_output = sink->get_data().size();
		nframes_t tolerance = 3;
		CPPUNIT_ASSERT (TestUtils::fuzzy_compare(half_frames, frames_output, tolerance));
	}
	
	void testRespectsEndOfInput()
	{
		assert (frames % 4 == 0);
		nframes_t const half_frames = frames / 2;
		
		converter->init (44100, 48000);
		converter->allocate_buffers (half_frames);
		converter->add_output (grabber);
		
		ConstProcessContext<float> c (random_data, half_frames, 2);
		converter->process (c);
		ConstProcessContext<float> c2 (&random_data[half_frames], half_frames / 2, 2);
		c2().set_flag (ProcessContext<float>::EndOfInput);
		converter->process (c2);
		
		for (std::list<ProcessContext<float> >::iterator it = grabber->contexts.begin(); it != grabber->contexts.end(); ++it) {
			std::list<ProcessContext<float> >::iterator next = it; ++next;
			if (next == grabber->contexts.end()) {
				CPPUNIT_ASSERT (it->has_flag (ProcessContext<float>::EndOfInput));
			} else {
				CPPUNIT_ASSERT (!it->has_flag (ProcessContext<float>::EndOfInput));
			}
		}
	}
	
	void testUpsampleDownsampleFuzzyCompare()
	{
		assert (frames % 4 == 0);
		nframes_t const half_frames = frames / 2;
		
		int repeat_times = 100;
		
		// Upsample
		{
		converter->init (44100, 88200);
		converter->allocate_buffers (half_frames);
		converter->add_output (sink);
		
		for (int i = 0; i < repeat_times - 1; ++i) {
			ConstProcessContext<float> c (sine_data, half_frames, 2);
			converter->process (c);
			ConstProcessContext<float> c2 (&sine_data[half_frames], half_frames, 2);
			converter->process (c2);
		}
		
		ConstProcessContext<float> c (sine_data, half_frames, 2);
		converter->process (c);
		ConstProcessContext<float> c2 (&sine_data[half_frames], half_frames, 2);
		c2().set_flag (ProcessContext<float>::EndOfInput);
		converter->process (c2);
		
		}

		// Downsample
		{
		converter->clear_outputs();
		converter->init (88200, 44100);
		converter->allocate_buffers (half_frames);
		converter->add_output (sink2);
		
		nframes_t current_frame = 0;
		nframes_t frames_left = sink->get_data().size();
		while (frames_left > half_frames) {
			ConstProcessContext<float> c (&sink->get_array()[current_frame], half_frames, 2);
			converter->process (c);
			current_frame += half_frames;
			frames_left = sink->get_data().size() - current_frame;
		}
		
		ConstProcessContext<float> c2 (&sink->get_array()[current_frame], frames_left, 2);
		c2().set_flag (ProcessContext<float>::EndOfInput);
		converter->process (c2);
		}
		
		// Compare
		
		nframes_t frames_output = sink2->get_data().size();
		nframes_t frame_tolerance = 3;
		float signal_tolerance = 0.01;
		
		CPPUNIT_ASSERT (TestUtils::fuzzy_compare(repeat_times * frames, frames_output, frame_tolerance));
		
		nframes_t current_frame = 0;
		while (current_frame < frames_output ) {
			CPPUNIT_ASSERT (TestUtils::array_equals_fuzzy (
				sine_data, &sink2->get_data()[current_frame], frames, signal_tolerance));
			current_frame += frames;
		}
	}

  private:
	boost::shared_ptr<SampleRateConverter > converter;
	boost::shared_ptr<AppendingVectorSink<float> > sink;
	boost::shared_ptr<AppendingVectorSink<float> > sink2;
	boost::shared_ptr<ProcessContextGrabber<float> > grabber;

	float const * random_data;
	float const * sine_data;
	nframes_t frames;
};

CPPUNIT_TEST_SUITE_REGISTRATION (SampleRateConverterTest);

