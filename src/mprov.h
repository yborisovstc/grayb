#ifndef __GRAYB_MPROV_H
#define __GRAYB_MPROV_H

#include <plat.h>
#include <string>
#include <vector>

using namespace std;

class Elem;
class MElem;
class Chromo;
class MEnv;

class MProvider
{
    public:
	virtual const string& Name() const = 0;
	virtual Elem* CreateNode(const string& aType, const string& aName, MElem* aMan, MEnv* aEnv) = 0;
	virtual Elem* GetNode(const string& aUri) = 0;
	virtual TBool IsProvided(MElem* aElem) const = 0;
	virtual Chromo* CreateChromo() = 0;
	virtual void AppendNodesInfo(vector<string>& aInfo) = 0;
	virtual const string& ModulesPath() const = 0;
};

#endif
