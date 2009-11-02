#ifndef AUDIOGRAPHER_THREADER_H
#define AUDIOGRAPHER_THREADER_H

#include <glibmm/threadpool.h>
#include <sigc++/slot.h>
#include <glib.h>
#include <vector>

#include "source.h"
#include "sink.h"

namespace AudioGrapher
{

template <typename T>
class Threader : public Source<T>, public Sink<T>
{
  public:
	
	Threader (Glib::ThreadPool & thread_pool)
	  : thread_pool (thread_pool)
	{}
	
	virtual ~Threader () {}
	
	void add_output (typename Source<T>::SinkPtr output) { outputs.push_back (output); }
	void clear_outputs () { outputs.clear (); }
	void remove_output (typename Source<T>::SinkPtr output) { outputs.remove (output); }
	
	void process (T * data, nframes_t frames)
	{
		unsigned int outs = outputs.count();
		g_atomic_int_add (&readers, outs);
		for (unsigned int i = 0; i < outs; ++i) {
			thread_pool.push (sigc::bind (sigc::mem_fun (this, &Threader::process), data, frames, i));
		}
		
		wait_mutex.lock();
		wait_cond.wait(wait_mutex);
	}
	
  private:

	void process(T * data, nframes_t frames, unsigned int channel)
	{
		outputs[channel].process (data, frames);
		
		if (g_atomic_int_dec_and_test (&readers)) {
			wait_cond.signal();
		}
	}

	std::vector<typename Source<T>::SinkPtr> outputs;

	Glib::ThreadPool & thread_pool;	
	Glib::Mutex wait_mutex;
	Glib::Cond  wait_cond;
	int         readers;
	
	T * data;

};

} // namespace

#endif //AUDIOGRAPHER_THREADER_H