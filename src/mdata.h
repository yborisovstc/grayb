#ifndef __GRAYB_MDATA_H
#define __GRAYB_MDATA_H

#include "plat.h"
#include <set>

using namespace std;

class Base;

// Data interfaces
//
class MUpdatable
{
    public:
	static const char* Type() { return "MUpdatable";};
	virtual void Update() = 0;
};

class MDInt
{
    public:
	static const char* Type() { return "MDInt";};
	virtual TInt Data() const = 0;
	virtual void Set(TInt aData) = 0;
	virtual void Update() = 0;
};

class MDIntGet
{
    public:
	static const char* Type() { return "MDIntGet";};
	virtual TInt Value() = 0;
};

class MDIntSet
{
    public:
	static const char* Type() { return "MDIntSet";};
	virtual void SetValue(TInt aData) = 0;
};


class MDataObserver
{
    public:
	static const char* Type() { return "MDataObserver";};
	virtual void OnDataChanged() = 0;
};


#endif
