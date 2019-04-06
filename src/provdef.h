#ifndef __GRAYB_PROVDEF_H
#define __GRAYB_PROVDEF_H

#include <unordered_map>

#include "prov.h"

class MUnit;
class Unit;

class ProvDef: public GProvider
{
    public:
	/** Native agent factory function */
	using TFact = Unit* (const string &aName, MUnit* aMan, MEnv* aEnv);
	/** Registry of native agents factory function */
	using TReg = unordered_map<string, TFact*>;
    public:
	ProvDef(const string& aName, MEnv* aEnv);
	virtual ~ProvDef();
	// From MProvider
	virtual Unit* CreateNode(const string& aType, const string& aName, MUnit* aMan, MEnv* aEnv);
	virtual Unit* GetNode(const string& aUri);
	virtual Chromo* CreateChromo();
	virtual void AppendNodesInfo(vector<string>& aInfo);
	virtual const string& ModulesPath() const;
    protected:
	/** Creates native agent */
	Unit* CreateAgent(const string& aType, const string& aName, MUnit* aMan, MEnv* aEnv) const;
	static string GetParentName(const string& aUri);
    private:
	vector<string> iNodesInfo;
	static const TReg mReg;
};


#endif
