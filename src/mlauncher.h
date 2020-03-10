#ifndef __GRAYB_MLAUNCHER_H
#define __GRAYB_MLAUNCHER_H

#include <plat.h>
#include <miface.h>

/** @ Model launcher interface
 * */
class MLauncher: public MIface
{
    public:
	static const char* Type() { return "MLauncher";};
	virtual TBool Run() = 0;
	virtual TBool Stop() = 0;
	// From MIface
	virtual MIface* MLauncher_Call(const string& aSpec, string& aRes) { return NULL;}
	virtual string MLauncher_Mid() const {return "?";}
	MIface* Call(const string& aSpec, string& aRes) override {return MLauncher_Call(aSpec, aRes);}
	string Mid() const override { return MLauncher_Mid();}
};

#endif

