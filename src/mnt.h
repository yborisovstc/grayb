#ifndef __GRAYB_MNT_H
#define __GRAYB_MNT_H

#include "elem.h"
#include "env.h"

/** @brief Agent of mounting point
 * */
class AMntp : public Elem
{
    public:
	static const char* Type() { return "AMntp";};
	static string PEType();
	AMntp(const string &aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	virtual ~AMntp();
	// From Unit
	virtual TEhr ProcessCompChanged(const MUnit* aComp, const string& aContName) override;
    protected:
	//TBool ImportModel(const string& aModelPath);
	TBool CreateModel(const string& aSpecPath);
    private:
	Env* mEnv = NULL;
};

#endif // __GRAYB_MNT_H
