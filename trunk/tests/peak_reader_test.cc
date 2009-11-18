#include "utils.h"
#include "audiographer/peak_reader.h"

using namespace AudioGrapher;

class PeakReaderTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE (PeakReaderTest);
  CPPUNIT_TEST (testProcess);
  CPPUNIT_TEST_SUITE_END ();

  public:
	void setUp()
	{
		frames = 128;
		random_data = TestUtils::init_random_data(frames);
	}

	void tearDown()
	{
		delete [] random_data;
	}

	void testProcess()
	{
		reader.reset (new PeakReader());
		ProcessContext<float> c (random_data, frames);
		
		float peak = 1.5;
		random_data[10] = peak;
		reader->process (c);
		CPPUNIT_ASSERT_EQUAL(peak, reader->get_peak());
		
		peak = 2.0;
		random_data[10] = peak;
		reader->process (c);
		CPPUNIT_ASSERT_EQUAL(peak, reader->get_peak());
		
		peak = -2.1;
		random_data[10] = peak;
		reader->process (c);
		float expected = fabs(peak);
		CPPUNIT_ASSERT_EQUAL(expected, reader->get_peak());
	}

  private:
	boost::shared_ptr<PeakReader> reader;

	float * random_data;
	nframes_t frames;
};

CPPUNIT_TEST_SUITE_REGISTRATION (PeakReaderTest);
