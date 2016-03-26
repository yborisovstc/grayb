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
