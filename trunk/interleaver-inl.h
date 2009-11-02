template<typename T>
void
Interleaver<T>::init (unsigned int num_channels, nframes_t max_frames_per_channel)
{
	reset();
	channels = num_channels;
	max_frames = max_frames_per_channel;
	
	buffer = new float[channels * max_frames];
	memset (buffer, 0, channels * max_frames);
	
	for (unsigned int i = 0; i < channels; ++i) {
		inputs.push_back (InputPtr (new Input (this, i)));
	}
}

template<typename T>
typename Source<T>::SinkPtr
Interleaver<T>::input (unsigned int channel)
{
	if (channel >= channels) {
		throw Exception ("Interleaver channel out of range");
	}
	
	return boost::static_pointer_cast<Source<T>::SinkPtr> (inputs[channel]);
}

template<typename T>
void
Interleaver<T>::reset ()
{
	inputs.clear();
	delete [] buffer;
	buffer = 0;
	channels = 0;
	max_frames = 0;
}

template<typename T>
void
Interleaver<T>::write_channel (T * data, nframes_t frames, unsigned int channel)
{
	if (frames > max_frames) {
		throw Exception ("Interleaver: too many frames given to an input");
	}
	
	for (unsigned int i = 0; i < frames; ++i) {
		buffer[channel + (channels * i)] = data[i];
	}
	
	nframes_t max_frames = 0;
	for (unsigned int i = 0; i < channels; ++i) {
		nframes_t const frames = inputs[i]->frames();
		if (!frames) { return; }
		max_frames = std::max (max_frames, frames);
	}
	
	output (buffer, max_frames);
	
	memset (buffer, 0, channels * max_frames);
}