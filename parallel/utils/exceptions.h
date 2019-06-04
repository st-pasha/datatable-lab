#ifndef dt_EXCEPTIONS_h
#define dt_EXCEPTIONS_h
#include <exception>

class Error : public std::exception {
  public:
    Error& operator<<(const std::string&) { return *this; }
    Error& operator<<(int) { return *this; }
    Error& operator<<(size_t) { return *this; }
};

class RuntimeError : public Error {};


#endif
