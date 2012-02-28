#ifndef __GRAYB_MPROV_H
#define __GRAYB_MPROV_H

#include <string>

using namespace std;

class Elem;
class Chromo;
class MEnv;

class MProvider
{
    public:
	virtual Elem* CreateNode(const string& aType, const string& aName, Elem* aMan, MEnv* aEnv) = 0;
	virtual Chromo* CreateChromo() = 0;
};

#endif
