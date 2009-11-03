template<typename T>
DeInterleaver<T>::DeInterleaver()
  : channels (0)
  , max_frames (0)
  , buffer (0)
  {}
  
template<typename T>
void
DeInterleaver<T>::init (unsigned int num_channels, nframes_t max_frames_per_channel)
{
	reset();
	channels = num_channels;
	max_frames = max_frames_per_channel;
	buffer = new float[max_frames];
	
	for (unsigned int i = 0; i < channels; ++i) {
		outputs.push_back (OutputPtr (new IdentityVertex<T>));
	}
}

template<typename T>
typename DeInterleaver<T>::SourcePtr
DeInterleaver<T>::output (unsigned int channel)
{
	if (channel >= channels) {
		throw Exception ("DeInterleaver channel out of range");
	}
	
	return boost::static_pointer_cast<Source<T> > (outputs[channel]);
}

template<typename T>
void
DeInterleaver<T>::process (T * data, nframes_t frames)
{
	if (frames == 0) { return; }
	
	nframes_t const  frames_per_channel = frames / channels;
	
	if (frames % channels != 0) {
		throw Exception ("DeInterleaver: wrong amount of frames given to process()");
	}
	
	if (frames_per_channel > max_frames) {
		throw Exception ("DeInterleaver: too many frames given to process()");
	}
	
	unsigned int channel = 0;
	for (typename std::vector<OutputPtr>::iterator it = outputs.begin(); it != outputs.end(); ++it, ++channel) {
		if (!*it) { continue; }
		
		for (unsigned int i = 0; i < frames_per_channel; ++i) {
			buffer[i] = data[channel + (channels * i)];
		}
		(*it)->process (buffer, frames_per_channel);
	}
}

template<typename T>
void
DeInterleaver<T>::reset ()
{
	outputs.clear();
	delete [] buffer;
	buffer = 0;
	channels = 0;
	max_frames = 0;
}
