#ifndef __GRAYB_MODULE_H
#define __GRAYB_MODULE_H

#include "base.h"
#include "guri.h"
#include "elem.h"
#include "mmod.h"

// Importing module agent
class AMod: public Elem, public MMod
{
    public:
	static const char* Type() { return "AMod";};
	static string PEType();
	AMod(const string& aName, MElem* aMan, MEnv* aEnv);
	AMod(MElem* aMan, MEnv* aEnv);
	virtual ~AMod();
	// From Base
	virtual MIface* DoGetObj(const char *aName);
	// From MMod
	virtual MElem* Import(const GUri& aUri);
	// Fom MElem
	virtual TBool GetCont(string& aValue, const string& aName = string()) const;
	virtual TBool ChangeCont(const string& aVal, TBool aRtOnly = ETrue, const string& aName=string());
	virtual TInt GetContCount() const;
    protected:
	string mCont;
	Chromo* mSpec;
};


#endif
