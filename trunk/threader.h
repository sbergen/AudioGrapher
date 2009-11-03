#ifndef AUDIOGRAPHER_THREADER_H
#define AUDIOGRAPHER_THREADER_H

#include <glibmm/threadpool.h>
#include <sigc++/slot.h>
#include <glib.h>
#include <vector>
#include <algorithm>

#include "source.h"
#include "sink.h"
#include "exception.h"

namespace AudioGrapher
{

template <typename T>
class Threader : public Source<T>, public Sink<T>
{
  private:
	typedef std::vector<typename Source<T>::SinkPtr> OutputVec;

  public:
	
	Threader (Glib::ThreadPool & thread_pool, long wait_timeout_milliseconds = 1000)
	  : thread_pool (thread_pool)
	  , readers (0)
	  , wait_timeout (wait_timeout_milliseconds)
	{ }
	
	virtual ~Threader () {}
	
	void add_output (typename Source<T>::SinkPtr output) { outputs.push_back (output); }
	void clear_outputs () { outputs.clear (); }
	void remove_output (typename Source<T>::SinkPtr output) {
		typename OutputVec::iterator new_end = std::remove(outputs.begin(), outputs.end(), output);
		outputs.erase (new_end, outputs.end());
	}
	
	void process (T * data, nframes_t frames)
	{
		wait_mutex.lock();
		
		unsigned int outs = outputs.size();
		g_atomic_int_add (&readers, outs);
		for (unsigned int i = 0; i < outs; ++i) {
			thread_pool.push (sigc::bind (sigc::mem_fun (this, &Threader::process_channel), data, frames, i));
		}
		
		Glib::TimeVal wait_time;
		wait_time.assign_current_time();
		wait_time.add_milliseconds(wait_timeout);
		
		wait_cond.timed_wait(wait_mutex, wait_time);
		bool timed_out = (g_atomic_int_get (&readers) != 0);
		wait_mutex.unlock();
		if (timed_out) { throw Exception ("Threader: wait timed out"); }
	}
	
  private:

	void process_channel(T * data, nframes_t frames, unsigned int channel)
	{
		outputs[channel]->process (data, frames);
		
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
	
	T * data;

};

} // namespace

#endif //AUDIOGRAPHER_THREADER_H