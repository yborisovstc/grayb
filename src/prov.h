#ifndef __GRAYB_PROV_H
#define __GRAYB_PROV_H

#include <unordered_map>

#include "mprov.h"
#include "base.h"


class Unit;

// Base class for graph providers
class GProvider: public Base, public MProvider
{
    public:
	typedef pair<string, Unit*> TRegVal;
	typedef map<string, Unit*> TReg;
    public:
	static const char* Type() { return "GProvider";};
	GProvider(const string& aName, MEnv* aEnv);
	virtual ~GProvider();
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MProvider
	virtual const string& Name() const { return iName;}
	virtual Unit* CreateNode(const string& aType, const string& aName, MUnit* aMan, MEnv* aEnv);
	virtual Unit* GetNode(const string& aUri);
	virtual TBool IsProvided(const MUnit* aElem) const;
	virtual MChromo* CreateChromo();
	virtual void AppendNodesInfo(vector<string>& aInfo);
	virtual const string& ModulesPath() const;
	virtual void SetEnv(MEnv* aEnv);
    protected:
	MEnv* iEnv;
	TReg iReg;
	string iName;
};

/** @brief Provider base using factory regiser
 * */
class ProvBase: public GProvider
{
    public:
	/** Native agent factory function */
	using TFact = Unit* (const string &aName, MUnit* aMan, MEnv* aEnv);
	/** Registry of native agents factory function */
	using TFReg = unordered_map<string, TFact*>;
    public:
	static const char* Type() { return "ProvBase";};
	ProvBase(const string& aName, MEnv* aEnv);
	virtual ~ProvBase();
	virtual const TFReg& FReg() const = 0;
	// From MProvider
	virtual Unit* CreateNode(const string& aType, const string& aName, MUnit* aMan, MEnv* aEnv);
	virtual Unit* GetNode(const string& aUri);
	virtual MChromo* CreateChromo();
    protected:
	/** Creates native agent */
	Unit* CreateAgent(const string& aType, const string& aName, MUnit* aMan, MEnv* aEnv) const;
	static string GetParentName(const string& aUri);
	template<typename T> static pair<string, ProvBase::TFact*> Item();
    private:
	vector<string> iNodesInfo;
};

/** Generator of native agent factory registry item */
template<typename T> pair<string, ProvBase::TFact*> ProvBase::Item() {
    return pair<string, ProvBase::TFact*>
	(T::Type(), [](const string &name, MUnit* parent, MEnv* env)->Unit* { return new T(name, parent, env);});
}




#endif
