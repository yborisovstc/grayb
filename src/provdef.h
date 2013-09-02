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
	virtual void AppendNodesInfo(vector<string>& aInfo);
    private:
	vector<string> iNodesInfo;
};


#endif
