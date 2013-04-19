#ifndef __GRAYB_MCONN_H
#define __GRAYB_MCONN_H

#include "plat.h"
#include <string>

using namespace std;

class Vert;

// Interface of connection point
class MConnPoint
{
    public:
	static const char* Type() { return "MConnPoint";};
};

#endif
