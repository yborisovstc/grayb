#ifndef __GRAYB_PROVDEF_H
#define __GRAYB_PROVDEF_H

#include "prov.h"

class Vert;

class ProvDef: public GProvider
{
    public:
	ProvDef(const string &aName);
	// From MProvider
	virtual Vert* CreateNode(const string& aType);
	virtual Chromo* CreateChromo();
};


#endif
