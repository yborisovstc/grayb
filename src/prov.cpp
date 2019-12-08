#include "prov.h"
#include "elem.h"
#include "chromox.h"

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

MChromo* GProvider::CreateChromo(const string& aRargs)
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






ProvBase::ProvBase(const string& aName, MEnv* aEnv): GProvider(aName, aEnv)
{
}

ProvBase::~ProvBase()
{
}

string ProvBase::GetParentName(const string& aUri)
{
    string res;
    size_t pos = aUri.find_last_of(GUri::KParentSep);
    res = aUri.substr(pos + 1);
    return res;
}

Unit* ProvBase::CreateNode(const string& aType, const string& aName, MUnit* aMan, MEnv* aEnv)
{
    Unit* res = NULL;
    // Using only short parent name. Full name is not supported atm
    string pname = GetParentName(aType);
    res = CreateAgent(pname, aName, aMan, iEnv);

    if (res != NULL) {
	Unit* parent = GetNode(aType);
	if (parent != NULL) {
	    MElem* eparent = parent->GetObj(eparent);
	    if (eparent != NULL) {
		eparent->AppendChild(res);
	    }
	}
    }
    return res;
}

Unit* ProvBase::GetNode(const string& aUri)
{
    Unit* res = NULL;
    if (iReg.count(aUri) > 0) {
	res = iReg.at(aUri);
    }
    else { 
	Unit* parent = NULL;
	// Using only short parent name. Full name is not supported atm
	string sname = GetParentName(aUri);
	res = CreateAgent(sname, string(), NULL, iEnv);

	if (res != NULL) {
	    if (parent == NULL) {
		string pname = GetParentName(res->PName());
		if (!pname.empty())
		    parent = iEnv->Provider()->GetNode(pname);
	    }
	    if (parent != NULL) {
		MElem* eparent = parent->GetObj(eparent);
		if (eparent != NULL) {
		    eparent->AppendChild(res);
		}
	    }
	    iReg.insert(TRegVal(aUri, res));
	}
    }
    return res;
}

MChromo* ProvBase::CreateChromo()
{
    return new ChromoX();
}

Unit* ProvBase::CreateAgent(const string& aType, const string& aName, MUnit* aMan, MEnv* aEnv) const
{
    Unit* res = NULL;
    if (FReg().count(aType) > 0) {
	TFact* fact = FReg().at(aType);
	res = fact(aName, aMan, aEnv);
    }
    return res;
}
