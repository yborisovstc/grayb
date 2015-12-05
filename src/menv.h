#ifndef __GRAYB_MENV_H
#define __GRAYB_MENV_H
class MProvider; class MLogRec;
class Elem;
class MChromo;
class GUri;

// Import manager

class MImportMgr
{
    public:
	virtual void GetModulesNames(vector<string>& aModules) const = 0;
	virtual void ResetImportsPaths() = 0;
	virtual void AddImportsPaths(const string& aPaths) = 0;
	virtual string GetModulePath(const string& aModName) const = 0;
	virtual TBool Import(const string& aUri) = 0;
	virtual Elem* OnUriNotResolved(Elem* aNode, const GUri& aUri) = 0;
};

// Chromo manager
class MChromoMgr
{
    public:
	virtual int GetSpecMaxOrder() const = 0;
	virtual int GetMaxOrder() const = 0;
	virtual int GetLim() const = 0;
	virtual void SetLim(int aLim) = 0;
	virtual bool EnablePhenoModif() const = 0;
	virtual void SetEnablePhenoModif(bool aEnable) = 0;
	virtual bool EnableFixErrors() const = 0;
	virtual void SetEnableFixErrors(bool aEnable) = 0;
	// Enable to re-postion owned mutations in order to resolve unsafety
	virtual bool EnableReposMuts() const = 0;
	virtual void SetEnableReposMuts(bool aEnable) = 0;
	virtual bool EnableCheckSafety() const = 0;
	virtual void SetEnableCheckSafety(bool aEnable) = 0;
	virtual bool EnableOptimization() const = 0;
	virtual void SetEnableOptimization(bool aEnable) = 0;
};

// Executive environment interface
class MEnv
{
    public:
	enum TSBool
	{
	    ESb_EnPerfTrace = 1,
	    ESb_EnIfTrace = 2 /* Enable tracing of interface providing */
	};
    public:
	virtual MProvider *Provider() const = 0;
	virtual MLogRec *Logger() = 0;
	virtual Elem* Root() = 0;
	virtual MChromoMgr* ChMgr() = 0;
	virtual MImportMgr* ImpsMgr() = 0;
	virtual TBool GetSBool(TSBool aId) const = 0;
	virtual void SetSBool(TSBool aId, TBool aVal) = 0;
};


#endif
