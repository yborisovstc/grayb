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
	TBool AddProvider(MProvider* aProv);
	void RemoveProvider(MProvider* aProv);
	// From Base
	virtual MIface *DoGetObj(const char *aName) override { return NULL;}
	// From MProvider
	virtual const string& Name() const { return iName;};
	virtual Unit* CreateNode(const string& aType, const string& aName, MUnit* aMan, MEnv* aEnv);
	virtual Unit* GetNode(const string& aUri);
	virtual TBool IsProvided(const MUnit* aElem) const;
	virtual void SetChromoRslArgs(const string& aRargs);
	virtual void GetChromoRslArgs(string& aRargs);
	virtual MChromo* CreateChromo(const string& aRargs = string());
	virtual void AppendNodesInfo(vector<string>& aInfo);
	virtual const string& ModulesPath() const;
	virtual void SetEnv(MEnv* aEnv);
    protected:
	static int FilterPlgDirEntries(const struct dirent *aEntry);
    protected:
	TProviders iProviders;
	MEnv* iEnv;
	string iName;
	string mChromoRargs; //!<  Chromo resolutin arguments
};



#endif
