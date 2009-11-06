#include "utils.h"
#include "audiographer/sample_format_converter.h"

using namespace AudioGrapher;

class SampleFormatConverterTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE (SampleFormatConverterTest);
  CPPUNIT_TEST (testFloats);
  CPPUNIT_TEST_SUITE_END ();

  public:
	void setUp()
	{
		frames = 128;
		random_data = Utils::init_random_data(frames);
	}

	void tearDown()
	{
		delete [] random_data;
	}

	void testFloats()
	{
		boost::shared_ptr<SampleFormatConverter<float> > converter (
			new SampleFormatConverter<float>(1, SampleFormatConverter<float>::D_Rect));
		boost::shared_ptr<VectorSink<float> > sink (new VectorSink<float>());
		nframes_t frames_output = 0;
		
		converter->add_output (sink);
		converter->set_clip_floats (true);
		
		ProcessContext<float> pc(random_data, frames);
		converter->process (pc);
		frames_output = sink->get_data().size();
		CPPUNIT_ASSERT_EQUAL (frames, frames_output);
		CPPUNIT_ASSERT (Utils::array_equals(sink->get_array(), random_data, frames));
		
		
	}

  private:

	float * random_data;
	nframes_t frames;
};

CPPUNIT_TEST_SUITE_REGISTRATION (SampleFormatConverterTest);

