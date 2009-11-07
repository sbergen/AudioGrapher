#ifndef AUDIOGRAPHER_LISTED_SOURCE_H
#define AUDIOGRAPHER_LISTED_SOURCE_H

#include "types.h"
#include "source.h"

#include <list>

namespace AudioGrapher
{

template<typename T>
class ListedSource : public Source<T>
{
  public:
	void add_output (typename Source<T>::SinkPtr output) { outputs.push_back(output); }
	void clear_outputs () { outputs.clear(); }
	void remove_output (typename Source<T>::SinkPtr output) { outputs.remove(output); }
	
  protected:
	
	typedef std::list<typename Source<T>::SinkPtr> SinkList;
	
	/// Helper for derived classes
	void output (ProcessContext<T> const & c)
	{
		for (typename SinkList::iterator i = outputs.begin(); i != outputs.end(); ++i) {
			(*i)->process (c);
		}
	}
 
	SinkList outputs;
};

} // namespace

#endif //AUDIOGRAPHER_LISTED_SOURCE_H

