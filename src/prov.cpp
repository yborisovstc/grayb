#include "prov.h"
#include "elem.h"

const string KModPath;

GProvider::GProvider(const string& aName, MEnv* aEnv): Base(), iName(aName), iEnv(aEnv)
{
}

GProvider::~GProvider() 
{
    while (!iReg.empty()) {
	Elem* elem = iReg.begin()->second;
	delete elem;
	iReg.erase(iReg.begin());
    }
}

void GProvider::SetEnv(MEnv* aEnv)
{
    assert(iEnv == NULL || aEnv == NULL);
    iEnv = aEnv;
}

void *GProvider::DoGetObj(const char *aName)
{
    return (strcmp(aName, Type()) == 0) ? this : NULL;
}

Elem* GProvider::CreateNode(const string& aType, const string& aName, MElem* aMan, MEnv* aEnv)
{
    return NULL;
}

Elem* GProvider::GetNode(const string& aUri)
{
    return NULL;
}

TBool GProvider::IsProvided(MElem* aElem) const
{
    TBool res = false;
    for (TReg::const_iterator it = iReg.begin(); it != iReg.end() && !res; it++) {
	res = aElem == it->second;
    }
    return res;
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
