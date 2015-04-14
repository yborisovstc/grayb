#include "prov.h"
#include "factr.h"
#include "provdef.h"

GFactory::GFactory(const string& aName, MEnv* aEnv): Base(aName), iEnv(aEnv)
{
    GProvider* baseprov = new ProvDef("ProvDef", iEnv);
    __ASSERT(baseprov != NULL);
    AddProvider(baseprov);
}

GFactory::~GFactory()
{
    while (!iProviders.empty()) {
	GProvider* prov = iProviders.begin()->second;
	delete prov;
	iProviders.erase(iProviders.begin());
    }
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
    Elem* res = NULL;
    for (map<string, GProvider*>::iterator it = iProviders.begin(); it != iProviders.end() && res == NULL; it++) {
	res = it->second->GetNode(aUri);
    }
    return res;
}

TBool GFactory::IsProvided(Elem* aElem) const
{
    TBool res = EFalse;
    for (map<string, GProvider*>::const_iterator it = iProviders.begin(); it != iProviders.end() && !res; it++) {
	res = it->second->IsProvided(aElem);
    }
    return res;

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

void GFactory::RemoveProvider(GProvider* aProv)
{
    __ASSERT(iProviders.count(aProv->Name()) > 0);
    iProviders.erase(aProv->Name());
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

