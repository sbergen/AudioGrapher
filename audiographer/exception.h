#ifndef AUDIOGRAPHER_EXCEPTION_H
#define AUDIOGRAPHER_EXCEPTION_H

#include <exception>
#include <string>

#include <boost/format.hpp>

#include "audiographer/debug_utils.h"

namespace AudioGrapher
{

class Exception : public std::exception
{
  public:
	template<typename T>
	Exception (T const & thrower, std::string const & reason)
	  : reason (boost::str (boost::format
			("Exception thrown by %1%: %2%")
			% DebugUtils::demangled_name (thrower) % reason))
	{}

	virtual ~Exception () throw() { }

	const char* what() const throw()
	{
		return reason.c_str();
	}

  private:
	std::string const reason;

};

} // namespace AudioGrapher

#endif // AUDIOGRAPHER_EXCEPTION_H