#ifndef __GRAYB_FACT_H
#define __GRAYB_FACT_H

#include "base.h"
#include "mprov.h"


class GProvider;

class GFactory: public Base, public MProvider
{
    public:
	typedef map<string, MProvider*> TProviders;
	typedef pair<string, MProvider*> TProvidersElem;
    public:
	static const char* Type() { return "GFactory";};
	GFactory(const string& aName, MEnv* aEnv);
	virtual ~GFactory();
	TBool LoadPlugin(const string& aName);
	void LoadPlugins();
	void AddProvider(MProvider* aProv);
	void RemoveProvider(MProvider* aProv);
	// From Base
	virtual MIface *DoGetObj(const char *aName) override { return NULL;}
	// From MProvider
	virtual const string& Name() const { return iName;};
	virtual Elem* CreateNode(const string& aType, const string& aName, MElem* aMan, MEnv* aEnv);
	virtual Elem* GetNode(const string& aUri);
	virtual TBool IsProvided(const MElem* aElem) const;
	virtual Chromo* CreateChromo();
	virtual void AppendNodesInfo(vector<string>& aInfo);
	virtual const string& ModulesPath() const;
	virtual void SetEnv(MEnv* aEnv);
    protected:
	static int FilterPlgDirEntries(const struct dirent *aEntry);
    protected:
	TProviders iProviders;
	MEnv* iEnv;
	string iName;
};



#endif
