#ifndef __GRAYB_MDES_H
#define __GRAYB_MDES_H

#include "plat.h"
#include <set>

using namespace std;

class Base;

// DES interfaces


// Observer. Upper layer is observer of components 
class MDesObserver: public MIface
{
    public:
	static const char* Type() { return "MDesObserver";};
	virtual void OnUpdated() = 0;
	virtual void OnActivated() = 0;
    public:
	// From MIface
	virtual string Uid() const { return Mid() + "%" + Type();};
    protected:
	class EIfu: public Ifu {
	    public:
		EIfu();
	};
	// Interface methods utility
	static EIfu mIfu;
};

// Syncable
class MDesSyncable: public MIface
{
    public:
	static const char* Type() { return "MDesSyncable";};
	virtual void Update() = 0;
	virtual void Confirm() = 0;
	virtual TBool IsUpdated() = 0;
	virtual void SetUpdated() = 0;
	virtual void ResetUpdated() = 0;
	virtual TBool IsActive() = 0;
	virtual void SetActive() = 0;
	virtual void ResetActive() = 0;
    public:
	// From MIface
	virtual string Uid() const { return Mid() + "%" + Type();};
    protected:
	class EIfu: public Ifu {
	    public:
		EIfu();
	};
	// Interface methods utility
	static EIfu mIfu;
};

#endif
