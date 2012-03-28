#include "provdef.h"
#include "chromox.h"
#include "vert.h"
#include "edge.h"
#include "prop.h"
#include "syst.h"
#include "incaps.h"


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
    else if (aType.compare(Elem::Type()) == 0) {
	res = new Elem(aName, aMan, aEnv);
    }
    else if (aType.compare(Syst::Type()) == 0) {
	res = new Syst(aName, aMan, aEnv);
    }
    else if (aType.compare(Incaps::Type()) == 0) {
	res = new Incaps(aName, aMan, aEnv);
    }
    return res;
}

Chromo* ProvDef::CreateChromo()
{
    return new ChromoX();
}

