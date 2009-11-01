#ifndef AUDIOGRAPHER_EXCEPTION_H
#define AUDIOGRAPHER_EXCEPTION_H

#include <exception>
#include <string>

namespace AudioGrapher
{

class Exception : public std::exception
{
  public:
	Exception (std::string const & reason)
		: reason (reason)
	{}

	~Exception () throw() { }

	const char* what() const throw()
	{
		return reason.c_str();
	}

  private:

	std::string const reason;

};

} // namespace AudioGrapher

#endif // AUDIOGRAPHER_EXCEPTION_H