#ifndef AUDIOGRAPHER_IDENTITY_VERTEX_H
#define AUDIOGRAPHER_IDENTITY_VERTEX_H

#include "listed_source.h"
#include "sink.h"

namespace AudioGrapher
{

template<typename T>
class IdentityVertex : public ListedSource<T>, Sink<T>
{
  public:
	void process (T * data, nframes_t frames) { ListedSource<T>::output(data, frames); }
};


} // namespace

#endif // AUDIOGRAPHER_IDENTITY_VERTEX_H
