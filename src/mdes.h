#ifndef __GRAYB_MDES_H
#define __GRAYB_MDES_H

#include <set>

#include "plat.h"
#include "miface.h"
#include "ifu.h"

using namespace std;

class Base;

// DES interfaces


// Observer. Upper layer is observer of components 
class MDesObserver: public MIface
{
    public:
	static const char* Type() { return "MDesObserver";};
	virtual void OnUpdated() = 0;
	// TODO Seems it is not used anymore. To delete.
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

// Iface stub to avoid clashing MIface methods
class MDesSyncable_Imd: public MDesSyncable
{
    virtual MIface* MDesSyncable_Call(const string& aSpec, string& aRes) = 0;
    virtual string MDesSyncable_Mid() const = 0;
    // From MIface
    virtual MIface* Call(const string& aSpec, string& aRes) { return MDesSyncable_Call(aSpec, aRes);};
    virtual string Mid() const { return MDesSyncable_Mid();};
};


#endif
