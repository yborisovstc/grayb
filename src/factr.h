#ifndef __GRAYB_FACT_H
#define __GRAYB_FACT_H

#include "base.h"
#include "mprov.h"

class GProvider;

class GFactory: public Base, public MProvider
{
    public:
	static const char* Type() { return "GFactory";};
	GFactory(const string& aName);
	void LoadPlugins();
	void AddProvider(GProvider* aProv);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MProvider
	virtual Elem* CreateNode(const string& aType, const string& aName, Elem* aMan, MEnv* aEnv);
	virtual Chromo* CreateChromo();
    protected:
	map<string, GProvider*> iProviders;
};


#endif
