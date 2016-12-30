#ifndef __GRAYB_PROV_H
#define __GRAYB_PROV_H

#include "mprov.h"
#include "base.h"

class Elem;

// Base class for graph providers
class GProvider: public Base, public MProvider
{
    public:
	typedef pair<string, Elem*> TRegVal;
	typedef map<string, Elem*> TReg;
    public:
	static const char* Type() { return "GProvider";};
	GProvider(const string& aName, MEnv* aEnv);
	virtual ~GProvider();
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MProvider
	virtual const string& Name() const { return iName;}
	virtual Elem* CreateNode(const string& aType, const string& aName, MElem* aMan, MEnv* aEnv);
	virtual Elem* GetNode(const string& aUri);
	virtual TBool IsProvided(const MElem* aElem) const;
	virtual Chromo* CreateChromo();
	virtual void AppendNodesInfo(vector<string>& aInfo);
	virtual const string& ModulesPath() const;
	virtual void SetEnv(MEnv* aEnv);
    protected:
	MEnv* iEnv;
	TReg iReg;
	string iName;
};

#endif
