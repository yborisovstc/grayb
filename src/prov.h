#ifndef __GRAYB_PROV_H
#define __GRAYB_PROV_H

#include "mprov.h"
#include "base.h"


class Unit;

// Base class for graph providers
class GProvider: public Base, public MProvider
{
    public:
	typedef pair<string, Unit*> TRegVal;
	typedef map<string, Unit*> TReg;
    public:
	static const char* Type() { return "GProvider";};
	GProvider(const string& aName, MEnv* aEnv);
	virtual ~GProvider();
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MProvider
	virtual const string& Name() const { return iName;}
	virtual Unit* CreateNode(const string& aType, const string& aName, MUnit* aMan, MEnv* aEnv);
	virtual Unit* GetNode(const string& aUri);
	virtual TBool IsProvided(const MUnit* aElem) const;
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
