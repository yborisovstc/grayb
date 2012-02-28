#ifndef __GRAYB_PROVDEF_H
#define __GRAYB_PROVDEF_H

#include "prov.h"

class Vert;

class ProvDef: public GProvider
{
    public:
	ProvDef(const string &aName);
	// From MProvider
	virtual Elem* CreateNode(const string& aType, const string& aName, Elem* aMan, MEnv* aEnv);
	virtual Chromo* CreateChromo();
};


#endif
