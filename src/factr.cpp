#include "prov.h"
#include "factr.h"
#include "provdef.h"

GFactory::GFactory(const string& aName): Base(aName)
{
    GProvider* baseprov = new ProvDef("ProvDef");
    __ASSERT(baseprov != NULL);
    AddProvider(baseprov);
}

void *GFactory::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    return (strcmp(aName, Type()) == 0) ? this : NULL;
}

Elem* GFactory::CreateNode(const string& aType, const string& aName, Elem* aMan, MEnv* aEnv)
{
    Elem* res = NULL;
    for (map<string, GProvider*>::iterator it = iProviders.begin(); it != iProviders.end() && res == NULL; it++) {
	res = it->second->CreateNode(aType, aName, aMan, aEnv);
    }
    return res;
}

Elem* GFactory::GetNode(const string& aUri)
{
}

Chromo* GFactory::CreateChromo()
{
    Chromo* res = NULL;
    for (map<string, GProvider*>::iterator it = iProviders.begin(); it != iProviders.end() && res == NULL; it++) {
	res = it->second->CreateChromo();
    }
    return res;
}

void GFactory::LoadPlugins()
{
}

void GFactory::AddProvider(GProvider* aProv)
{
    map<string, GProvider*>::const_iterator res = iProviders.find(aProv->Name());
    __ASSERT(res == iProviders.end());
    iProviders.insert(pair<string, GProvider*>(aProv->Name(), aProv));
}

void GFactory::AppendNodesInfo(vector<string>& aInfo)
{
    for (map<string, GProvider*>::iterator it = iProviders.begin(); it != iProviders.end(); it++) {
	it->second->AppendNodesInfo(aInfo);
    }
}

const string& GFactory::ModulesPath() const
{
    GProvider* defprov = iProviders.at("ProvDef");
    __ASSERT(defprov != NULL);
    return defprov->ModulesPath();
}

