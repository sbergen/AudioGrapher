#ifndef AUDIOGRAPHER_EXCEPTION_H
#define AUDIOGRAPHER_EXCEPTION_H

#include <exception>
#include <string>

namespace AudioGrapher
{

class ExceptionBase : public std::exception
{
  public:
	ExceptionBase (std::string const & reason)
		: reason (reason)
	{}

	virtual ~ExceptionBase () throw() { }

	/// Return a pointer to deriving class. This must be overriden by deriving classes!
	virtual ExceptionBase * clone() const throw() { return new ExceptionBase (reason); }

	const char* what() const throw()
	{
		return reason.c_str();
	}

  protected:

	std::string const reason;

};

class Exception : public ExceptionBase
{
  public:
	Exception (std::string const & reason) : ExceptionBase ("LibAudioGraph: " + reason) {}
	ExceptionBase * clone() const throw() { return new Exception (reason); }
};

} // namespace AudioGrapher

#endif // AUDIOGRAPHER_EXCEPTION_H