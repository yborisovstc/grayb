#include "prov.h"

const string KModPath;

GProvider::GProvider(const string &aName, MEnv* aEnv): Base(aName), iEnv(aEnv)
{
}

GProvider::~GProvider() 
{
}

void *GProvider::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    return (strcmp(aName, Type()) == 0) ? this : NULL;
}

Elem* GProvider::CreateNode(const string& aType, const string& aName, Elem* aMan, MEnv* aEnv)
{
    return NULL;
}

Elem* GProvider::GetNode(const string& aUri)
{
    return NULL;
}

Chromo* GProvider::CreateChromo()
{
    return NULL;
}

void GProvider::AppendNodesInfo(vector<string>& aInfo)
{
}

const string& GProvider::ModulesPath() const
{
    return KModPath;
}
