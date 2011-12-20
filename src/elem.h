#ifndef __GRAYB_ELEM_H
#define __GRAYB_ELEM_H

#include "melem.h"
#include "menv.h"
#include "mlog.h"
#include "base.h"

class Chromo;

// Element of native graph hier - mutable
class Elem: public Base, public MElem 
{
    public:
	static const char* Type() { return "Elem";};
	Elem(const string &aName = string());
	void SetMutation(const ChromoNode& aMuta);
	void Mutate(TBool aRunTimeOnly = EFalse);
    public:
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MElem
	virtual MElem* GetNode(const GUri& aUri);
	virtual MElem* GetNode(const GUri& aUri, GUri::const_elem_iter aPathBase);
	virtual TBool AddNode(const ChromoNode& aSpec);
	virtual const set<TNodeType>& CompsTypes();
    protected:
	void MutAddNode(const ChromoNode& aSpec);
	inline MLogRec* Logger();
    protected:
	// Environment
	MEnv* iEnv;
	// Managed (higher) element of hier
	Elem* iMan;
	// Components types
	static const set<TNodeType> iCompsTypes;
};

inline MLogRec* Elem::Logger() {return iEnv ? iEnv->Logger(): NULL; }

#endif
