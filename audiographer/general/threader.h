#ifndef AUDIOGRAPHER_THREADER_H
#define AUDIOGRAPHER_THREADER_H

#include <glibmm/threadpool.h>
#include <sigc++/slot.h>
#include <boost/format.hpp>

#include <glib.h>
#include <vector>
#include <algorithm>

#include "audiographer/source.h"
#include "audiographer/sink.h"
#include "audiographer/exception.h"

namespace AudioGrapher
{

/// Class that stores exceptions thrown from different threads
class ThreaderException : public Exception
{
  public:
	template<typename T>
	ThreaderException (T const & thrower, std::exception const & e)
		: Exception (thrower,
			boost::str ( boost::format
			("\n\t- Dynamic type: %1%\n\t- what(): %2%")
			% DebugUtils::demangled_name (e) % e.what() ))
	{ }
};

/// Class for distributing processing across several threads
template <typename T = DefaultSampleType>
class Threader : public Source<T>, public Sink<T>
{
  private:
	typedef std::vector<typename Source<T>::SinkPtr> OutputVec;

  public:
	
	/** Constructor
	  * \n RT safe
	  * \param thread_pool a thread pool from which all tasks are scheduled
	  * \param wait_timeout_milliseconds maximum time allowed for threads to use in processing
	  */
	Threader (Glib::ThreadPool & thread_pool, long wait_timeout_milliseconds = 1000)
	  : thread_pool (thread_pool)
	  , readers (0)
	  , wait_timeout (wait_timeout_milliseconds)
	{ }
	
	virtual ~Threader () {}
	
	/// Adds output \n not RT safe
	void add_output (typename Source<T>::SinkPtr output) { outputs.push_back (output); }
	
	/// Clears outputs \n RT safe
	void clear_outputs () { outputs.clear (); }
	
	/// Removes a specific output \n RT safe
	void remove_output (typename Source<T>::SinkPtr output) {
		typename OutputVec::iterator new_end = std::remove(outputs.begin(), outputs.end(), output);
		outputs.erase (new_end, outputs.end());
	}
	
	/** Processes context concurrently by scheduling each output separately to the given thread pool
	  * \n not RT safe
	  */
	void process (ProcessContext<T> const & c)
	{
		wait_mutex.lock();
		
		exception.reset();
		
		unsigned int outs = outputs.size();
		g_atomic_int_add (&readers, outs);
		for (unsigned int i = 0; i < outs; ++i) {
			thread_pool.push (sigc::bind (sigc::mem_fun (this, &Threader::process_output), c, i));
		}
		
		wait();
	}
	
	using Sink<T>::process;
	
  private:

	void wait()
	{
		Glib::TimeVal wait_time;
		wait_time.assign_current_time();
		wait_time.add_milliseconds(wait_timeout);
		
		wait_cond.timed_wait(wait_mutex, wait_time);
		bool timed_out = (g_atomic_int_get (&readers) != 0);
		wait_mutex.unlock();
		if (timed_out) { throw Exception (*this, "wait timed out"); }
		
		if (exception) {
			throw *exception;
		}
	}
	
	void process_output(ProcessContext<T> const & c, unsigned int output)
	{
		try {
			outputs[output]->process (c);
		} catch (std::exception const & e) {
			// Only first exception will be passed on
			exception_mutex.lock();
			if(!exception) { exception.reset (new ThreaderException (*this, e)); }
			exception_mutex.unlock();
		}
		
		if (g_atomic_int_dec_and_test (&readers)) {
			wait_cond.signal();
		}
	}

	OutputVec outputs;

	Glib::ThreadPool & thread_pool;
	Glib::Mutex wait_mutex;
	Glib::Cond  wait_cond;
	gint        readers;
	long        wait_timeout;
	
	Glib::Mutex exception_mutex;
	boost::shared_ptr<ThreaderException> exception;

};

} // namespace

#endif //AUDIOGRAPHER_THREADER_H