#ifndef __GRAYB_MPROV_H
#define __GRAYB_MPROV_H

#include <plat.h>
#include <string>
#include <vector>

using namespace std;

class Unit;
class MUnit;
class MChromo;
class MEnv;

class MProvider
{
    public:
	static const char* Type() { return "MProvider";};
	virtual ~MProvider() {};
	virtual const string& Name() const = 0;
	virtual Unit* CreateNode(const string& aType, const string& aName, MUnit* aMan, MEnv* aEnv) = 0;
	virtual Unit* GetNode(const string& aUri) = 0;
	virtual TBool IsProvided(const MUnit* aElem) const = 0;
	/** @brief Sets chromo resolution arguments
	 * Just one argument, chromo spec file extension is used
	 * */
	virtual void SetChromoRslArgs(const string& aRargs) = 0;
	virtual void GetChromoRslArgs(string& aRargs) = 0;
	virtual MChromo* CreateChromo(const string& aRargs = string()) = 0;
	virtual void AppendNodesInfo(vector<string>& aInfo) = 0;
	virtual const string& ModulesPath() const = 0;
	virtual void SetEnv(MEnv* aEnv) = 0;
};

#endif
