#include "provdef.h"
#include "chromox.h"
#include "vert.h"
#include "edge.h"
#include "prop.h"
#include "syst.h"
#include "incaps.h"
#include "data.h"
#include "func.h"
#include "des.h"

// TODO [YB] To import from build variable
const string KModulesPath = "/usr/share/grayb/modules/";

ProvDef::ProvDef(const string &aName): GProvider(aName)
{
}

Elem* ProvDef::CreateNode(const string& aType, const string& aName, Elem* aMan, MEnv* aEnv)
{
    Elem* res = NULL;
    if (aType.compare(Vert::Type()) == 0) {
	res = new Vert(aName, aMan, aEnv);
    }
    else if (aType.compare(Edge::Type()) == 0) {
	res = new Edge(aName, aMan, aEnv);
    }
    else if (aType.compare(Prop::Type()) == 0) {
	res = new Prop(aName, aMan, aEnv);
    }
    else if (aType.compare(Description::Type()) == 0) {
	res = new Description(aName, aMan, aEnv);
    }
    else if (aType.compare(Elem::Type()) == 0) {
	res = new Elem(aName, aMan, aEnv);
    }
    else if (aType.compare(ConnPointBase::Type()) == 0) {
	res = new ConnPointBase(aName, aMan, aEnv);
    }
    else if (aType.compare(Syst::Type()) == 0) {
	res = new Syst(aName, aMan, aEnv);
    }
    else if (aType.compare(Incaps::Type()) == 0) {
	res = new Incaps(aName, aMan, aEnv);
    }
    else if (aType.compare(DInt::Type()) == 0) {
	res = new DInt(aName, aMan, aEnv);
    }
    else if (aType.compare(DNInt::Type()) == 0) {
	res = new DNInt(aName, aMan, aEnv);
    }
    else if (aType.compare(AIncInt::Type()) == 0) {
	res = new AIncInt(aName, aMan, aEnv);
    }
    else if (aType.compare(AAddInt::Type()) == 0) {
	res = new AAddInt(aName, aMan, aEnv);
    }
    else if (aType.compare(AFAddInt::Type()) == 0) {
	res = new AFAddInt(aName, aMan, aEnv);
    }
    else if (aType.compare(AFSubInt::Type()) == 0) {
	res = new AFSubInt(aName, aMan, aEnv);
    }
    else if (aType.compare(AFLimInt::Type()) == 0) {
	res = new AFLimInt(aName, aMan, aEnv);
    }
    else if (aType.compare(AFDivInt::Type()) == 0) {
	res = new AFDivInt(aName, aMan, aEnv);
    }
    else if (aType.compare(AFGTInt::Type()) == 0) {
	res = new AFGTInt(aName, aMan, aEnv);
    }
    else if (aType.compare(AFBoolToInt::Type()) == 0) {
	res = new AFBoolToInt(aName, aMan, aEnv);
    }
    else if (aType.compare(AFConvInt::Type()) == 0) {
	res = new AFConvInt(aName, aMan, aEnv);
    }
    else if (aType.compare(AFIntToVect::Type()) == 0) {
	res = new AFIntToVect(aName, aMan, aEnv);
    }
    else if (aType.compare(AFunIntRes::Type()) == 0) {
	res = new AFunIntRes(aName, aMan, aEnv);
    }
    else if (aType.compare(ExtenderAgent::Type()) == 0) {
	res = new ExtenderAgent(aName, aMan, aEnv);
    }
    else if (aType.compare(StateAgent::Type()) == 0) {
	res = new StateAgent(aName, aMan, aEnv);
    }
    else if (aType.compare(ATrIncInt::Type()) == 0) {
	res = new ATrIncInt(aName, aMan, aEnv);
    }
    else if (aType.compare(ASocket::Type()) == 0) {
	res = new ASocket(aName, aMan, aEnv);
    }
    else if (aType.compare(ADes::Type()) == 0) {
	res = new ADes(aName, aMan, aEnv);
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
    aInfo.push_back(Vert::Type());
    aInfo.push_back(Prop::Type());
    aInfo.push_back(Description::Type());
    aInfo.push_back(ConnPointBase::Type());
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
    aInfo.push_back(StateAgent::Type());
    aInfo.push_back(ATrIncInt::Type());
    aInfo.push_back(ASocket::Type());
    aInfo.push_back(ADes::Type());
}

const string& ProvDef::ModulesPath() const
{
    return KModulesPath;
}
