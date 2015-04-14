#ifndef __GRAYB_ENV_H
#define __GRAYB_ENV_H

#include "base.h"
#include "menv.h"

class GProvider;
class GFactory;
class Elem;
class GLogRec;
class Env;
class Chromo;

class ChromoMgr: public Base, public MChromoMgr
{
    friend class Env;
    public:
	static const char* Type() { return "ChromoMgr";};
	ChromoMgr(const string& aName, Env& aHost);
	virtual ~ChromoMgr();
    public:
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// Form MChromoMgr
	virtual int GetSpecMaxOrder() const;
	virtual int GetMaxOrder() const;
	virtual TInt GetLim() const { return mLim;};
	virtual void SetLim(TInt aLim) { mLim = aLim;};
	virtual TBool EnablePhenoModif() const {return mEnablePhenoModif;};
	virtual void SetEnablePhenoModif(TBool aEnable);
	virtual bool EnableFixErrors() const { return mEnableFixErrors;};
	virtual void SetEnableFixErrors(bool aEnable);
	virtual bool EnableReposMuts() const { return mEnableReposMuts;};
	virtual void SetEnableReposMuts(bool aEnable);
	virtual bool EnableCheckSafety() const { return mEnableCheckSafety;};
	virtual void SetEnableCheckSafety(bool aEnable);
    protected:
	TInt mLim;
	Env& mHost;
	TBool mEnablePhenoModif;
	TBool mEnableFixErrors;
	TBool mEnableReposMuts;
	TBool mEnableCheckSafety;
};

class Env: public Base, public MEnv
{
    friend class ChromoMgr;
public:
	static const char* Type() { return "GEnv";};
	Env(const string& aName, const string& aSpecFile, const string& aLogFileName = string());
	virtual ~Env();
	// Separated from constructor because constr can be followed by second phase - setting providers etc.
	void ConstructSystem();
	void AddProvider(GProvider* aProv);
	void RemoveProvider(GProvider* aProv);
public:
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MEnv
	virtual MProvider *Provider() const;
	virtual MLogRec *Logger();
	virtual Elem* Root();
	virtual MChromoMgr* ChMgr();
	virtual TBool GetSBool(TSBool aId) const;
	virtual void SetSBool(TSBool aId, TBool aVal);
private:
	GLogRec* iLogger; 
	Elem* iRoot;
	GFactory *iProvider;
	ChromoMgr* iChMgr;
	string iSystSpec;
	Chromo* iSpecChromo;
	TBool mEnPerfTrace;
};



#endif
