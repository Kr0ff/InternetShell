#pragma once
#include <Windows.h>

struct rstring {
	DWORD Length;
	char* Buffer;
} _rstring;