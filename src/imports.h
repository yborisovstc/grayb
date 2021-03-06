#ifndef __GRAYB_IMPORTS_H
#define __GRAYB_IMPORTS_H

#include "base.h"
#include "guri.h"
#include "elem.h"
#include "mmod.h"

// Imports container agent
class AImports: public Elem, public MImports
{
    public:
	static const char* Type() { return "AImports";};
	static string PEType();
	AImports(const string& aName, MUnit* aMan, MEnv* aEnv);
	AImports(MUnit* aMan, MEnv* aEnv);
	virtual ~AImports();
	// From MMod
	virtual TBool Import(const string& aUri);
};

#endif
