#ifndef __GRAYB_MMOD_H
#define __GRAYB_MMOD_H

#include "plat.h"


    class MUnit;
    class GUri;

    // Importing module
    class MMod
    {
	public:
	    static const char* Type() { return "MMod";};
	    virtual MUnit* Import(const GUri& aUri) = 0;
    };

    // Importing modules container
    class MImports
    {
	public:
	    static const char* Type() { return "MImports";};
	    virtual TBool Import(const string& aUri) = 0;
    };

#endif
