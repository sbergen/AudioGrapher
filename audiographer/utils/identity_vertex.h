#ifndef AUDIOGRAPHER_IDENTITY_VERTEX_H
#define AUDIOGRAPHER_IDENTITY_VERTEX_H

#include "audiographer/utils/listed_source.h"
#include "audiographer/sink.h"

namespace AudioGrapher
{

template<typename T = DefaultSampleType>
class IdentityVertex : public ListedSource<T>, Sink<T>
{
  public:
	void process (ProcessContext<T> const & c) { ListedSource<T>::output(c); }
	void process (ProcessContext<T> & c) { ListedSource<T>::output(c); }
};


} // namespace

#endif // AUDIOGRAPHER_IDENTITY_VERTEX_H
