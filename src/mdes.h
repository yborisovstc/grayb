#ifndef __GRAYB_MDES_H
#define __GRAYB_MDES_H

#include <set>

#include "plat.h"
#include "miface.h"
#include "ifu.h"

using namespace std;

class Base;

// DES interfaces


/** @brief Components Observer. Upper layer is observer of components 
 *
 * Components notifies upper layer of status changes
 * */
class MDesObserver: public MIface
{
    public:
	static const char* Type() { return "MDesObserver";};
	/** @brief Notification that component was activated */
	virtual void OnActivated() = 0;
	/** @brief Notification that component was changed */
	virtual void OnUpdated() = 0;
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

/** @brief Inputs Observer
 *
 * State is notified by its inputs of inputs update
 * */
class MDesInpObserver: public MIface
{
    public:
	static const char* Type() { return "MDesInpObserver";};
	/** @brief Notification that input state was changed */
	virtual void OnInpUpdated() = 0;
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

// Iface stub for MDesInpObserver
class MDesInpObserver_Imd: public MDesInpObserver
{
    virtual MIface* MDesInpObserver_Call(const string& aSpec, string& aRes) = 0;
    virtual string MDesInpObserver_Mid() const = 0;
    // From MIface
    virtual MIface* Call(const string& aSpec, string& aRes) { return MDesInpObserver_Call(aSpec, aRes);}
    virtual string Mid() const { return MDesInpObserver_Mid();}
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
	// Debug
	virtual void DumpActive() = 0;
	virtual void DumpUpdated() = 0;
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
