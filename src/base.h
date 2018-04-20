#ifndef __GRAYB_BASE_H
#define __GRAYB_BASE_H

#include <stdarg.h>
#include <string.h>
#include "plat.h"
#include <vector>
#include <map>
#include <string>
#include <sstream>

using namespace std;

class MIface;

// Base supporting iface provider
class Base
{
    public:
	virtual ~Base() {};
	template <class T> T* GetObj(T* aInst) {return aInst = static_cast<T*>(DoGetObj(aInst->Type()));};
	MIface* GetObj(const char *aType) {return DoGetObj(aType); };
	virtual MIface *DoGetObj(const char *aName) = 0;
};

#endif
