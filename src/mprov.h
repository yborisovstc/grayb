#ifndef __GRAYB_MPROV_H
#define __GRAYB_MPROV_H

#include <string>

using namespace std;

class Vert;
class Chromo;

class MProvider
{
    public:
	virtual Vert* CreateNode(const string& aType) = 0;
	virtual Chromo* CreateChromo() = 0;
};

#endif
