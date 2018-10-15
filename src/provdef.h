#ifndef __GRAYB_PROVDEF_H
#define __GRAYB_PROVDEF_H

#include <unordered_map>

#include "prov.h"

class Vert;

class ProvDef: public GProvider
{
    public:
	/** Native agent factory function */
	using TFact = Elem* (const string &aName, MElem* aMan, MEnv* aEnv);
	/** Registry of native agents factory function */
	using TReg = unordered_map<string, TFact*>;
    public:
	ProvDef(const string& aName, MEnv* aEnv);
	virtual ~ProvDef();
	// From MProvider
	virtual Elem* CreateNode(const string& aType, const string& aName, MElem* aMan, MEnv* aEnv);
	virtual Elem* GetNode(const string& aUri);
	virtual Chromo* CreateChromo();
	virtual void AppendNodesInfo(vector<string>& aInfo);
	virtual const string& ModulesPath() const;
    protected:
	/** Creates native agent */
	Elem* CreateAgent(const string& aType, const string& aName, MElem* aMan, MEnv* aEnv) const;
	static string GetParentName(const string& aUri);
    private:
	vector<string> iNodesInfo;
	static const TReg mReg;
};


#endif
