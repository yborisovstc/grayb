#ifndef __GRAYB_BASE_H
#define __GRAYB_BASE_H

#include <stdarg.h>
#include <string.h>
#include "plat.h"
#include <vector>
#include <map>
#include <string>

using namespace std;

// Base for all classes
class Base
{
    public:
	Base(const string& aName): iName(aName) {};
	virtual ~Base() {};
	const string& Name() const { return iName;}
	template <class T> T* GetObj(T* aInst) {return aInst = static_cast<T*>(DoGetObj(aInst->Type())); };
	void* GetObj(const char *aType) {return DoGetObj(aType); };
    protected:
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue) = 0;
    protected:
	string iName;
};

#endif
