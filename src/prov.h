#ifndef __GRAYB_PROV_H
#define __GRAYB_PROV_H

#include "mprov.h"
#include "base.h"

class Vert;

// Base class for graph providers
class GProvider: public Base, public MProvider
{
    public:
	typedef pair<string, Elem*> TRegVal;
	typedef map<string, Elem*> TReg;
    public:
	static const char* Type() { return "GProvider";};
	GProvider(const string &aName, MEnv* aEnv);
	virtual ~GProvider();
	void SetEnv(MEnv* aEnv);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MProvider
	virtual Elem* CreateNode(const string& aType, const string& aName, Elem* aMan, MEnv* aEnv);
	virtual Elem* GetNode(const string& aUri);
	virtual TBool IsProvided(Elem* aElem) const;
	virtual Chromo* CreateChromo();
	virtual void AppendNodesInfo(vector<string>& aInfo);
	virtual const string& ModulesPath() const;
    protected:
	MEnv* iEnv;
	TReg iReg;
};

#endif
