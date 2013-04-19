#ifndef __GRAYB_MDES_H
#define __GRAYB_MDES_H

#include "plat.h"
#include <set>

using namespace std;

class Base;

// DES interfaces


// Observer. Upper layer is observer of components 
class MDesObserver
{
    public:
	static const char* Type() { return "MDesObserver";};
	virtual void OnUpdated() = 0;
	virtual void OnActivated() = 0;

};

// Syncable
class MDesSyncable
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
};

/*
// Updatable
class MDesUpdatable
{
    public:
	static const char* Type() { return "MDesUpdatable";};
	virtual void Update() = 0;
	virtual TBool IsUpdated() = 0;
	virtual void SetUpdated() = 0;
	virtual void ResetUpdated() = 0;
};

// Activable
class MDesActivable
{
    public:
	static const char* Type() { return "MDesActivable";};
	virtual TBool IsActive() = 0;
	virtual void SetActive() = 0;
	virtual void ResetActive() = 0;
};
*/

#endif
