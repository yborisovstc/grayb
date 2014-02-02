#ifndef __GRAYB_PROVDEF_H
#define __GRAYB_PROVDEF_H

#include "prov.h"

class Vert;

class ProvDef: public GProvider
{
    public:
	typedef pair<string, Elem*> TRegVal;
	typedef map<string, Elem*> TReg;
    public:
	ProvDef(const string &aName, MEnv* aEnv);
	virtual ~ProvDef();
	// From MProvider
	virtual Elem* CreateNode(const string& aType, const string& aName, Elem* aMan, MEnv* aEnv);
	virtual Elem* GetNode(const string& aUri);
	virtual Chromo* CreateChromo();
	virtual void AppendNodesInfo(vector<string>& aInfo);
	virtual const string& ModulesPath() const;
    private:
	vector<string> iNodesInfo;
	Elem* iElem;
	TReg iReg;
};


#endif
