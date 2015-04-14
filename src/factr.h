#ifndef __GRAYB_FACT_H
#define __GRAYB_FACT_H

#include "base.h"
#include "mprov.h"

class GProvider;

class GFactory: public Base, public MProvider
{
    public:
	static const char* Type() { return "GFactory";};
	GFactory(const string& aName, MEnv* aEnv);
	virtual ~GFactory();
	void LoadPlugins();
	void AddProvider(GProvider* aProv);
	void RemoveProvider(GProvider* aProv);
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
	map<string, GProvider*> iProviders;
	MEnv* iEnv;
};


#endif
