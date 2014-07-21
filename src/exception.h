#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <exception>

namespace ailab {

class exception_t : public std::exception {
	static size_t const LEN = 1024;

	char text[LEN];

public:
	template<typename ... T>
	exception_t(char const *fmt, T ... args) throw() {
		snprintf(text, LEN, fmt, args ...);
	}

	exception_t(char const *str) throw() {
		snprintf(text, LEN, "%s", str);
	}

	virtual char const *what() const throw() {
		return text;
	}

	virtual ~exception_t() throw() {
	}
};

}
