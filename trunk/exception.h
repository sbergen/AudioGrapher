#ifndef AUDIOGRAPHER_EXCEPTION_H
#define AUDIOGRAPHER_EXCEPTION_H

#include <exception>
#include <string>

namespace AudioGrapher
{

template<typename T>
class ExceptionBase : public std::exception
{
  public:
	ExceptionBase (std::string const & reason)
		: reason (reason)
	{}

	virtual ~ExceptionBase () throw() { }

	T * clone() const
	{
		return new T (dynamic_cast<T const &>(*this));
	}

	const char* what() const throw()
	{
		return reason.c_str();
	}

  private:

	std::string const reason;

};

class Exception : public ExceptionBase<Exception>
{
  public:
	Exception (std::string const & reason) : ExceptionBase<Exception> (reason) {}
};

} // namespace AudioGrapher

#endif // AUDIOGRAPHER_EXCEPTION_H