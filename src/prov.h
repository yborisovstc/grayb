#ifndef __GRAYB_PROV_H
#define __GRAYB_PROV_H

#include "mprov.h"
#include "base.h"

class Vert;

// Base class for graph providers
class GProvider: public Base, public MProvider
{
    public:
	static const char* Type() { return "GProvider";};
	GProvider(const string &aName);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MProvider
	virtual Vert* CreateNode(const string& aType);
	virtual Chromo* CreateChromo();
};

#endif
