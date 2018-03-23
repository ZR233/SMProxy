#pragma once
#include <cstdint>
#include "boost/asio.hpp"



class Utils
{
	Utils() {};
	~Utils() {};
public:
	static uint64_t htonll(uint64_t x)
	{
		uint64_t a = htonl(uint32_t(x));
		a = a << 32;// HHGGFFEE
		uint64_t b = htonl(uint32_t(x>>32));// DDCCBBAA
		x = a + b;
		return x;
	}
	static uint64_t ntohll(uint64_t x)
	{
		uint64_t a = ntohl(uint32_t(x));
		a = a << 32;// HHGGFFEE
		uint64_t b = ntohl(uint32_t(x >> 32));// DDCCBBAA
		x = a + b;
		return x;
	}
};
