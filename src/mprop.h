#ifndef __GRAYB_MPROP_H
#define __GRAYB_MPROP_H

#include "plat.h"
#include <string>
#include "ifu.h"
#include "miface.h"

using namespace std;

// Interface of property
class MProp: public MIface
{
    public:
	static const char* Type() { return "MProp";};
	virtual const string& Value() const = 0;
	// From MIface
	virtual MIface* MProp_Call(const string& aSpec, string& aRes) { return NULL;}
	virtual string MProp_Mid() const {return "?";}
	MIface* Call(const string& aSpec, string& aRes) override {return MProp_Call(aSpec, aRes);}
	string Mid() const override { return MProp_Mid();}
    protected:
	class EIfu: public Ifu {
	    public:
		EIfu();
	};
	// Interface methods utility
	static EIfu mIfu;
};

#endif
