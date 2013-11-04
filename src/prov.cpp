#include "prov.h"

const string KModPath;

GProvider::GProvider(const string &aName): Base(aName)
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
