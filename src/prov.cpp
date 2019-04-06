#include "prov.h"
#include "elem.h"

const string KModPath;

GProvider::GProvider(const string& aName, MEnv* aEnv): Base(), iName(aName), iEnv(aEnv)
{
}

GProvider::~GProvider() 
{
    while (!iReg.empty()) {
	Unit* elem = iReg.begin()->second;
	delete elem;
	iReg.erase(iReg.begin());
    }
}

void GProvider::SetEnv(MEnv* aEnv)
{
    assert(iEnv == NULL || aEnv == NULL);
    iEnv = aEnv;
}

MIface *GProvider::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MProvider::Type()) == 0) {
	res = (MIface*) (MProvider*) this;
    }
    return res;
}

Unit* GProvider::CreateNode(const string& aType, const string& aName, MUnit* aMan, MEnv* aEnv)
{
    return NULL;
}

Unit* GProvider::GetNode(const string& aUri)
{
    return NULL;
}

TBool GProvider::IsProvided(const MUnit* aElem) const
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
