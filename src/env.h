#ifndef __GRAYB_ENV_H
#define __GRAYB_ENV_H

#include "menv.h"
#include "mgraph.h"

class GProvider;
class GFactory;
class Elem;
class GLogRec;

class ChromoMgr: public Base, public MChromoMgr
{
    public:
	static const char* Type() { return "ChromoMgr";};
	ChromoMgr(const string& aName);
	virtual ~ChromoMgr();
    public:
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// Form MChromoMgr
	virtual TInt GetOrder(const MChromo& aChromo) const;
};

class Env: public Base, public MEnv
{
public:
	static const char* Type() { return "GEnv";};
	Env(const string& aName, const string& aSpecFile, const string& aLogFileName = string());
	virtual ~Env();
	// Separated from constructor because constr can be followed by second phase - setting providers etc.
	void ConstructSystem();
	void AddProvider(GProvider* aProv);
public:
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MEnv
	virtual MProvider *Provider() const;
	virtual MLogRec *Logger();
	virtual Elem* Root();
	virtual MChromoMgr* ChMgr();
private:
	GLogRec* iLogger; 
	Elem* iRoot;
	GFactory *iProvider;
	ChromoMgr* iChMgr;
	string iSystSpec;
};



#endif
