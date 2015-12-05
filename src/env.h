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
class ChromoNode;

class ImportsMgr: public Base, public MImportMgr
{
    friend class Env;
    public:
	static const char* Type() { return "ImportsMgr";};
	ImportsMgr(const string& aName, Env& aHost);
	virtual ~ImportsMgr();
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue);
    public:
	// From MImportMgr
	virtual void GetModulesNames(vector<string>& aModules) const;
	virtual void ResetImportsPaths();
	virtual void AddImportsPaths(const string& aPaths);
	virtual string GetModulePath(const string& aModName) const;
	virtual TBool Import(const string& aUri);
	virtual Elem* OnUriNotResolved(Elem* aNode, const GUri& aUri);
    private:
	void AddImportModulesInfo(const string& aPath);
	Elem* GetImportsContainer() const;
	void ImportToNode(Elem* aNode, const ChromoNode& aMut, const ChromoNode& aSel);
	Elem* DoImport(const string& aUri);
    private:
	Env& mHost;
	vector<string> mImportsPaths;
	map<string, string> mModsPaths;
	static const string KDefImportPath;
	static const string KImportsContainerUri;
};

class ChromoMgr: public Base, public MChromoMgr
{
    friend class Env;
    public:
	static const char* Type() { return "ChromoMgr";};
	ChromoMgr(const string& aName, Env& aHost);
	virtual ~ChromoMgr();
    public:
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue);
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
	virtual bool EnableOptimization() const { return mEnableOptimization;};;
	virtual void SetEnableOptimization(bool aEnable);
    protected:
	TInt mLim;
	Env& mHost;
	TBool mEnablePhenoModif;
	TBool mEnableFixErrors;
	TBool mEnableReposMuts;
	TBool mEnableCheckSafety;
	TBool mEnableOptimization;
};

class Env: public Base, public MEnv
{
    friend class ChromoMgr;
public:
	static const char* Type() { return "GEnv";};
	Env(const string& aName, const string& aSpecFile, const string& aLogFileName = string());
	Env(const string& aName, const string& aSpec, const string& aLogFileName, TBool aOpt);
	virtual ~Env();
	// Separated from constructor because constr can be followed by second phase - setting providers etc.
	void ConstructSystem();
	void AddProvider(GProvider* aProv);
	void RemoveProvider(GProvider* aProv);
public:
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue);
	// From MEnv
	virtual MProvider *Provider() const;
	virtual MLogRec *Logger();
	virtual Elem* Root();
	virtual MChromoMgr* ChMgr();
	virtual MImportMgr* ImpsMgr();
	virtual TBool GetSBool(TSBool aId) const;
	virtual void SetSBool(TSBool aId, TBool aVal);
private:
	GLogRec* iLogger; 
	Elem* iRoot;
	GFactory *iProvider;
	ChromoMgr* iChMgr;
	ImportsMgr* iImpMgr;
	string iSpecFile;
	string iSpec;
	Chromo* iSpecChromo;
	TBool mEnPerfTrace;
	TBool mEnIfTrace;
};



#endif
