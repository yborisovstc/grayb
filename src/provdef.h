#ifndef __GRAYB_PROVDEF_H
#define __GRAYB_PROVDEF_H


#include "prov.h"

class MUnit;
class Unit;

class ProvDef: public ProvBase
{
    public:
	ProvDef(const string& aName, MEnv* aEnv);
	virtual ~ProvDef();
	virtual const TFReg& FReg() const {return mReg;}
	// From MProvider
	virtual void AppendNodesInfo(vector<string>& aInfo);
	virtual const string& ModulesPath() const;
	virtual void SetChromoRslArgs(const string& aRargs) {}
	virtual void GetChromoRslArgs(string& aRargs) {}
	virtual MChromo* CreateChromo(const string& aRargs = string());
    private:
	static const TFReg mReg;
};


#endif
