#ifndef __GRAYB_DATA_H
#define __GRAYB_DATA_H

#include "elem.h"
#include "mdata.h"

// Data is representing some static model
//
// Data of integer
class DInt: public Elem , public MDInt
{
    public:
	static const char* Type() { return "DInt";};
	DInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MDInt
	virtual TInt Data() const;
	virtual void Set(TInt aData);
    private:
	TInt mData;
};

#endif
