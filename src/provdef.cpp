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

ProvDef::ProvDef(const string &aName, MEnv* aEnv): GProvider(aName, aEnv)
{
}

ProvDef::~ProvDef()
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
    else if (aType.compare(ConnPointBaseInp::Type()) == 0) {
	res = new ConnPointBaseInp(aName, aMan, aEnv);
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
    else if (aType.compare(DVar::Type()) == 0) {
	res = new DVar(aName, aMan, aEnv);
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
    else if (aType.compare(AFAddVar::Type()) == 0) {
	res = new AFAddVar(aName, aMan, aEnv);
    }
    else if (aType.compare(AFMplVar::Type()) == 0) {
	res = new AFMplVar(aName, aMan, aEnv);
    }
    else if (aType.compare(AFDivVar::Type()) == 0) {
	res = new AFDivVar(aName, aMan, aEnv);
    }
    else if (aType.compare(AFBcmpVar::Type()) == 0) {
	res = new AFBcmpVar(aName, aMan, aEnv);
    }
    else if (aType.compare(AFAtVar::Type()) == 0) {
	res = new AFAtVar(aName, aMan, aEnv);
    }
    else if (aType.compare(AFSwitchVar::Type()) == 0) {
	res = new AFSwitchVar(aName, aMan, aEnv);
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
    else if (aType.compare(ATrSubInt::Type()) == 0) {
	res = new ATrSubInt(aName, aMan, aEnv);
    }
    else if (aType.compare(ATrMplInt::Type()) == 0) {
	res = new ATrMplInt(aName, aMan, aEnv);
    }
    else if (aType.compare(ATrDivInt::Type()) == 0) {
	res = new ATrDivInt(aName, aMan, aEnv);
    }
    else if (aType.compare(ATrAddVar::Type()) == 0) {
	res = new ATrAddVar(aName, aMan, aEnv);
    }
    else if (aType.compare(ATrMplVar::Type()) == 0) {
	res = new ATrMplVar(aName, aMan, aEnv);
    }
    else if (aType.compare(ATrDivVar::Type()) == 0) {
	res = new ATrDivVar(aName, aMan, aEnv);
    }
    else if (aType.compare(ATrCpsVectVar::Type()) == 0) {
	res = new ATrCpsVectVar(aName, aMan, aEnv);
    }
    else if (aType.compare(ATrSwitchVar::Type()) == 0) {
	res = new ATrSwitchVar(aName, aMan, aEnv);
    }
    else if (aType.compare(ATrAtVar::Type()) == 0) {
	res = new ATrAtVar(aName, aMan, aEnv);
    }
    else if (aType.compare(ATrBcmpVar::Type()) == 0) {
	res = new ATrBcmpVar(aName, aMan, aEnv);
    }
    else if (aType.compare(ASocket::Type()) == 0) {
	res = new ASocket(aName, aMan, aEnv);
    }
    else if (aType.compare(ADes::Type()) == 0) {
	res = new ADes(aName, aMan, aEnv);
    }
    if (res != NULL) {
	Elem* parent = GetNode(aType);
	if (parent != NULL) {
	    parent->AppendChild(res);
	}
    }
    return res;
}

// TODO [YB] To create parent based on PEType
Elem* ProvDef::GetNode(const string& aUri)
{
    Elem* res = NULL;
    if (iReg.count(aUri) > 0) {
	res = iReg.at(aUri);
    }
    else { 
	Elem* parent = NULL;
	if (aUri.compare(Elem::Type()) == 0) {
	    res = new Elem(NULL, iEnv);
	    iReg.insert(TRegVal(aUri, res));
	}
	else if (aUri.compare(Vert::Type()) == 0) {
	    parent = GetNode("Elem");
	    res = new Vert(NULL, iEnv);
	}
	else if (aUri.compare(Edge::Type()) == 0) {
	    parent = GetNode("Elem");
	    res = new Edge(NULL, iEnv);
	}
	else if (aUri.compare(Prop::Type()) == 0) {
	    parent = GetNode("Elem");
	    res = new Prop(NULL, iEnv);
	}
	else if (aUri.compare(ExtenderAgent::Type()) == 0) {
	    parent = GetNode("Elem");
	    res = new ExtenderAgent(NULL, iEnv);
	}
	else if (aUri.compare(ASocket::Type()) == 0) {
	    parent = GetNode("Elem");
	    res = new ASocket(NULL, iEnv);
	}
	else if (aUri.compare(Incaps::Type()) == 0) {
	    parent = GetNode("Elem");
	    res = new Incaps(NULL, iEnv);
	}
	else if (aUri.compare(DataBase::Type()) == 0) {
	    parent = GetNode("Elem");
	    res = new DataBase(NULL, iEnv);
	}
	else if (aUri.compare(DVar::Type()) == 0) {
	    parent = GetNode("Elem");
	    res = new DVar(NULL, iEnv);
	}
	else if (aUri.compare(FuncBase::Type()) == 0) {
	    parent = GetNode("Elem");
	    res = new FuncBase(NULL, iEnv);
	}
	else if (aUri.compare(ATrBase::Type()) == 0) {
	    parent = GetNode("Elem");
	    res = new ATrBase(NULL, iEnv);
	}
	else if (aUri.compare(AFunc::Type()) == 0) {
	    parent = GetNode("Elem");
	    res = new AFunc(NULL, iEnv);
	}
	else if (aUri.compare(StateAgent::Type()) == 0) {
	    parent = GetNode("Elem");
	    res = new StateAgent(NULL, iEnv);
	}
	else if (aUri.compare(ADes::Type()) == 0) {
	    parent = GetNode("Elem");
	    res = new ADes(NULL, iEnv);
	}
	else if (aUri.compare(Syst::Type()) == 0) {
	    parent = GetNode("Vert");
	    res = new Syst(NULL, iEnv);
	}
	else if (aUri.compare(ConnPointBase::Type()) == 0) {
	    parent = GetNode("Vert");
	    res = new ConnPointBase(NULL, iEnv);
	}
	else if (aUri.compare(ConnPointBaseInp::Type()) == 0) {
	    parent = GetNode("ConnPointBase");
	    res = new ConnPointBaseInp(NULL, iEnv);
	}
	else if (aUri.compare(Description::Type()) == 0) {
	    parent = GetNode("Prop");
	    res = new Description(NULL, iEnv);
	}
	else if (aUri.compare(DInt::Type()) == 0) {
	    parent = GetNode("DataBase");
	    res = new DInt(NULL, iEnv);
	}
	else if (aUri.compare(DVar::Type()) == 0) {
	    parent = GetNode("DataBase");
	    res = new DVar(NULL, iEnv);
	}
	else if (aUri.compare(DNInt::Type()) == 0) {
	    parent = GetNode("DInt");
	    res = new DNInt(NULL, iEnv);
	}
	else if (aUri.compare(AFunInt::Type()) == 0) {
	    parent = GetNode("FuncBase");
	    res = new AFunInt(NULL, iEnv);
	}
	else if (aUri.compare(AFunVar::Type()) == 0) {
	    parent = GetNode("FuncBase");
	    res = new AFunVar(NULL, iEnv);
	}
	else if (aUri.compare(AFAddVar::Type()) == 0) {
	    parent = GetNode("AFunVar");
	    res = new AFAddVar(NULL, iEnv);
	}
	else if (aUri.compare(AFMplVar::Type()) == 0) {
	    parent = GetNode("AFunVar");
	    res = new AFMplVar(NULL, iEnv);
	}
	else if (aUri.compare(AFDivVar::Type()) == 0) {
	    parent = GetNode("AFunVar");
	    res = new AFDivVar(NULL, iEnv);
	}
	else if (aUri.compare(AFBcmpVar::Type()) == 0) {
	    parent = GetNode("AFunVar");
	    res = new AFBcmpVar(NULL, iEnv);
	}
	else if (aUri.compare(AFAtVar::Type()) == 0) {
	    parent = GetNode("AFunVar");
	    res = new AFAtVar(NULL, iEnv);
	}
	else if (aUri.compare(AFSwitchVar::Type()) == 0) {
	    parent = GetNode("AFunVar");
	    res = new AFSwitchVar(NULL, iEnv);
	}
	else if (aUri.compare(AIncInt::Type()) == 0) {
	    parent = GetNode("AFunInt");
	    res = new AIncInt(NULL, iEnv);
	}
	else if (aUri.compare(AFunIntRes::Type()) == 0) {
	    parent = GetNode("AFunInt");
	    res = new AFunIntRes(NULL, iEnv);
	}
	else if (aUri.compare(AAddInt::Type()) == 0) {
	    parent = GetNode("AFunInt");
	    res = new AAddInt(NULL, iEnv);
	}
	else if (aUri.compare(AFuncInt::Type()) == 0) {
	    parent = GetNode("AFunc");
	    res = new AFuncInt(NULL, iEnv);
	}
	else if (aUri.compare(AFAddInt::Type()) == 0) {
	    parent = GetNode("AFunc");
	    res = new AFAddInt(NULL, iEnv);
	}
	else if (aUri.compare(AFSubInt::Type()) == 0) {
	    parent = GetNode("AFuncInt");
	    res = new AFSubInt(NULL, iEnv);
	}
	else if (aUri.compare(AFConvInt::Type()) == 0) {
	    parent = GetNode("AFuncInt");
	    res = new AFConvInt(NULL, iEnv);
	}
	else if (aUri.compare(AFLimInt::Type()) == 0) {
	    parent = GetNode("AFunc");
	    res = new AFLimInt(NULL, iEnv);
	}
	else if (aUri.compare(AFDivInt::Type()) == 0) {
	    parent = GetNode("AFunc");
	    res = new AFDivInt(NULL, iEnv);
	}
	else if (aUri.compare(AFGTInt::Type()) == 0) {
	    parent = GetNode("AFunc");
	    res = new AFGTInt(NULL, iEnv);
	}
	else if (aUri.compare(AFBoolToInt::Type()) == 0) {
	    parent = GetNode("AFunc");
	    res = new AFBoolToInt(NULL, iEnv);
	}
	else if (aUri.compare(ATrInt::Type()) == 0) {
	    parent = GetNode("ATrBase");
	    res = new ATrInt(NULL, iEnv);
	}
	else if (aUri.compare(ATrVar::Type()) == 0) {
	    parent = GetNode("ATrBase");
	    res = new ATrVar(NULL, iEnv);
	}
	else if (aUri.compare(ATrIncInt::Type()) == 0) {
	    parent = GetNode("ATrInt");
	    res = new ATrIncInt(NULL, iEnv);
	}
	else if (aUri.compare(ATrSubInt::Type()) == 0) {
	    parent = GetNode("ATrInt");
	    res = new ATrSubInt(NULL, iEnv);
	}
	else if (aUri.compare(ATrMplInt::Type()) == 0) {
	    parent = GetNode("ATrInt");
	    res = new ATrMplInt(NULL, iEnv);
	}
	else if (aUri.compare(ATrDivInt::Type()) == 0) {
	    parent = GetNode("ATrInt");
	    res = new ATrDivInt(NULL, iEnv);
	}
	else if (aUri.compare(ATrAddVar::Type()) == 0) {
	    parent = GetNode("ATrVar");
	    res = new ATrAddVar(NULL, iEnv);
	}
	else if (aUri.compare(ATrMplVar::Type()) == 0) {
	    parent = GetNode("ATrVar");
	    res = new ATrMplVar(NULL, iEnv);
	}
	else if (aUri.compare(ATrDivVar::Type()) == 0) {
	    parent = GetNode("ATrVar");
	    res = new ATrDivVar(NULL, iEnv);
	}
	else if (aUri.compare(ATrCpsVectVar::Type()) == 0) {
	    parent = GetNode("ATrVar");
	    res = new ATrCpsVectVar(NULL, iEnv);
	}
	else if (aUri.compare(ATrSwitchVar::Type()) == 0) {
	    parent = GetNode("ATrVar");
	    res = new ATrSwitchVar(NULL, iEnv);
	}
	else if (aUri.compare(ATrAtVar::Type()) == 0) {
	    parent = GetNode("ATrVar");
	    res = new ATrAtVar(NULL, iEnv);
	}
	else if (aUri.compare(ATrBcmpVar::Type()) == 0) {
	    parent = GetNode("ATrVar");
	    res = new ATrBcmpVar(NULL, iEnv);
	}
	if (res != NULL) {
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
