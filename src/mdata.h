#ifndef __GRAYB_MDATA_H
#define __GRAYB_MDATA_H

#include "plat.h"
#include <set>
#include <vector>

using namespace std;

class Base;

// Data interfaces
//
class MUpdatable
{
    public:
	static const char* Type() { return "MUpdatable";};
	virtual TBool Update() = 0;
};

class MDInt
{
    public:
	static const char* Type() { return "MDInt";};
	virtual TInt Data() const = 0;
	virtual void Set(TInt aData) = 0;
//	virtual void Update() = 0;
};

class MDFloat
{
    public:
	static const char* Type() { return "MDFloat";};
	virtual float Data() const = 0;
	virtual void Set(float aData) = 0;
};

class MDIntGet
{
    public:
	static const char* Type() { return "MDIntGet";};
	virtual TInt Value() = 0;
};

class MDFloatGet
{
    public:
	static const char* Type() { return "MDFloatGet";};
	virtual float Value() = 0;
};

class MDBoolGet
{
    public:
	static const char* Type() { return "MDBoolGet";};
	virtual TBool Value() = 0;
};

class MDIntSet
{
    public:
	static const char* Type() { return "MDIntSet";};
	virtual void SetValue(TInt aData) = 0;
};

class MDFloatSet
{
    public:
	static const char* Type() { return "MDFloatSet";};
	virtual void SetValue(float aData) = 0;
};

// Vector Int getting iface
class MVIntGet
{
    public:
	static const char* Type() { return "MVIntGet";};
	virtual vector<TInt> Value() = 0;
};



class MDataObserver
{
    public:
	static const char* Type() { return "MDataObserver";};
	virtual void OnDataChanged() = 0;
};

class MDVar
{
    public:
	static const char* Type() { return "MDVar";};
};

class Elem;
class MDVarGet
{
    public:
	static const char* Type() { return "MDVarGet";};
	virtual Elem* VarGetBase() = 0;
	// Get iface id. Is used for type negotiation from root to leafs
	virtual string VarGetIfid() const = 0;
};

class MDVarSet
{
    public:
	static const char* Type() { return "MDVarSet";};
	virtual Elem* VarSetBase() = 0;
};

// Vector: Float
typedef std::vector<float> VFloat;

class MVFloatGet
{
    public:
	static const char* Type() { return "MVFloatGet";};
	virtual void VFloatGet(VFloat& aData) = 0;
};





#endif
