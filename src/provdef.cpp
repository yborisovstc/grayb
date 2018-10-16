#include "provdef.h"
#include "chromox.h"
#include "vert.h"
#include "vertp.h"
#include "edge.h"
#include "prop.h"
#include "syst.h"
#include "systp.h"
#include "incaps.h"
#include "data.h"
#include "func.h"
#include "des.h"
#include "module.h"
#include "imports.h"

// TODO [YB] To import from build variable
const string KModulesPath = "/usr/share/grayb/modules/";



/** Generator of native agent factory registry item */
template<typename T> pair<string, ProvDef::TFact*> Item() {
    return pair<string, ProvDef::TFact*>
	(T::Type(), [](const string &name, MElem* parent, MEnv* env)->Elem* { return new T(name, parent, env);});
}


/** Native agents factory registry */
const ProvDef::TReg ProvDef::mReg ( {
    Item<Elem>(), Item<Vertp>(), Item<Syst>(), Item<Systp>(), Item<Vert>(),
    Item<ACapsule>(), Item<Edge>(), Item<Aedge>(), Item<AMod>(), Item<AImports>(),
    Item<Prop>(), Item<Description>(), Item<ExtenderAgent>(), Item<ExtenderAgentInp>(), Item<ExtenderAgentOut>(),
    Item<AExtender>(), Item<ASocket>(), Item<ASocketInp>(), Item<ASocketOut>(), Item<ASocketMc>(),
    Item<Incaps>(), Item<DataBase>(), Item<DVar>(), Item<FuncBase>(), Item<ATrBase>(),
    Item<AFunc>(), Item<StateAgent>(), Item<ADes>(), Item<DInt>(), Item<DVar>(),
    Item<DNInt>(), Item<AFunInt>(), Item<AFunVar>(), Item<AFAddVar>(), Item<AFMplVar>(),
    Item<AFMplncVar>(), Item<AFMplinvVar>(), Item<AFCastVar>(), Item<AFCpsMtrdVar>(), Item<AFCpsVectVar>(),
    Item<AFDivVar>(), Item<AFBcmpVar>(), Item<AFCmpVar>(), Item<AFAtVar>(), Item<AFBoolNegVar>(),
    Item<AFSwitchVar>(), Item<AIncInt>(), Item<AFunIntRes>(), Item<AAddInt>(), Item<AFuncInt>(),
    Item<AFAddInt>(), Item<AFSubInt>(), Item<AFConvInt>(), Item<AFLimInt>(), Item<AFDivInt>(),
    Item<AFGTInt>(), Item<AFBoolToInt>(), Item<ATrInt>(), Item<ATrVar>(), Item<ATrInt>(),
    Item<ATrSubInt>(), Item<ATrMplInt>(), Item<ATrDivInt>(), Item<ATrIncInt>(), Item<ATrAddVar>(),
    Item<ATrMplVar>(), Item<ATrDivVar>(), Item<ATrCpsVectVar>(), Item<ATrSwitchVar>(), Item<ATrAtVar>(),
    Item<ATrBcmpVar>(), Item<ConnPointBase>(), Item<ConnPointBaseInp>(), Item<ConnPointBaseOut>(), Item<ConnPointMc>()
});



ProvDef::ProvDef(const string& aName, MEnv* aEnv): GProvider(aName, aEnv)
{
    size_t bcount = mReg.bucket_count();
    size_t sz = mReg.size();
}

ProvDef::~ProvDef()
{
}

string ProvDef::GetParentName(const string& aUri)
{
    string res;
    size_t pos = aUri.find_last_of(GUri::KParentSep);
    res = aUri.substr(pos + 1);
    return res;
}

Elem* ProvDef::CreateNode(const string& aType, const string& aName, MElem* aMan, MEnv* aEnv)
{
    Elem* res = NULL;
    // Using only short parent name. Full name is not supported atm
    string pname = GetParentName(aType);
    res = CreateAgent(pname, aName, aMan, iEnv);

    if (res != NULL) {
	Elem* parent = GetNode(aType);
	if (parent != NULL) {
	    parent->AppendChild(res);
	}
    }
    return res;
}

Elem* ProvDef::GetNode(const string& aUri)
{
    Elem* res = NULL;
    if (iReg.count(aUri) > 0) {
	res = iReg.at(aUri);
    }
    else { 
	Elem* parent = NULL;
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
		parent->AppendChild(res);
	    }
	    iReg.insert(TRegVal(aUri, res));
	}
    }
    return res;
}

Chromo* ProvDef::CreateChromo()
{
    return new ChromoX();
}

void ProvDef::AppendNodesInfo(vector<string>& aInfo)
{
    aInfo.push_back(Elem::Type());
    aInfo.push_back(Edge::Type());
    aInfo.push_back(Aedge::Type());
    aInfo.push_back(Vert::Type());
    aInfo.push_back(Vertp::Type());
    aInfo.push_back(Prop::Type());
    aInfo.push_back(Description::Type());
    aInfo.push_back(ConnPointBase::Type());
    aInfo.push_back(ConnPointMc::Type());
    aInfo.push_back(Syst::Type());
    aInfo.push_back(Incaps::Type());
    aInfo.push_back(DInt::Type());
    aInfo.push_back(DNInt::Type());
    aInfo.push_back(AIncInt::Type());
    aInfo.push_back(AAddInt::Type());
    aInfo.push_back(AFAddInt::Type());
    aInfo.push_back(AFSubInt::Type());
    aInfo.push_back(AFLimInt::Type());
    aInfo.push_back(AFDivInt::Type());
    aInfo.push_back(AFGTInt::Type());
    aInfo.push_back(AFBoolToInt::Type());
    aInfo.push_back(AFConvInt::Type());
    aInfo.push_back(AFIntToVect::Type());
    aInfo.push_back(AFunIntRes::Type());
    aInfo.push_back(ExtenderAgent::Type());
    aInfo.push_back(AExtender::Type());
    aInfo.push_back(StateAgent::Type());
    aInfo.push_back(ATrIncInt::Type());
    aInfo.push_back(ATrSubInt::Type());
    aInfo.push_back(ATrMplInt::Type());
    aInfo.push_back(ATrDivInt::Type());
    aInfo.push_back(ASocket::Type());
    aInfo.push_back(ADes::Type());
}

const string& ProvDef::ModulesPath() const
{
    return KModulesPath;
}

Elem* ProvDef::CreateAgent(const string& aType, const string& aName, MElem* aMan, MEnv* aEnv) const
{
    Elem* res = NULL;
    if (mReg.count(aType) > 0) {
	TFact* fact = mReg.at(aType);
	res = fact(aName, aMan, aEnv);
    }
    return res;
}
