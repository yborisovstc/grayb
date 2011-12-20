#include "provdef.h"
#include "chromox.h"
#include "vert.h"


ProvDef::ProvDef(const string &aName): GProvider(aName)
{
}

Vert* ProvDef::CreateNode(const string& aType)
{
    Vert* res = NULL;
    if (aType.compare(Vert::Type()) == 0) {
	res = new Vert();
    }
    return res;
}

Chromo* ProvDef::CreateChromo()
{
    return new ChromoX();
}

