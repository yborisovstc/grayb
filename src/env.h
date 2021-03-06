#ifndef __GRAYB_ENV_H
#define __GRAYB_ENV_H

#include "base.h"
#include "menv.h"
#include "mlauncher.h"
#include "ifu.h"
#include "guri.h"
#include "profiler.h"

class GProvider;
class GFactory;
class Unit;
class GLogRec;
class Env;
class Chromo;
class ChromoNode;

class ImportsMgr: public Base, public MImportMgr
{
    friend class Env;
    public:
    static const char* Type() { return "ImportsMgr";};
    ImportsMgr(Env& aHost);
    virtual ~ImportsMgr();
    // From Base
    virtual MIface *DoGetObj(const char *aName) override { return NULL;}
    public:
    // From MImportMgr
    virtual void GetModulesNames(vector<string>& aModules) const;
    virtual void ResetImportsPaths();
    virtual void AddImportsPaths(const string& aPaths);
    virtual string GetModulePath(const string& aModName) const;
    virtual TBool Import(const string& aUri);
    virtual MUnit* OnUriNotResolved(MUnit* aNode, const GUri& aUri);
    private:
    void AddImportModulesInfo(const string& aPath);
    MUnit* GetImportsContainer() const;
    void ImportToNode(MUnit* aNode, const ChromoNode& aMut, const ChromoNode& aSel);
    MUnit* DoImport(const string& aUri);
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
    ChromoMgr(Env& aHost);
    virtual ~ChromoMgr();
    public:
    // From Base
    virtual MIface *DoGetObj(const char *aName) override { return NULL;}
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
    virtual void SetChromoRslArgs(const string& aArgs);
    virtual void GetChromoRslArgs(string& aArgs);
    protected:
    TInt mLim;
    Env& mHost;
    TBool mEnablePhenoModif;
    TBool mEnableFixErrors;
    TBool mEnableReposMuts;
    TBool mEnableCheckSafety;
    TBool mEnableOptimization;
};

// Iface resolver. Ref ds_irm
class IfcResolver: public MIfaceResolver
{
    public:
	IfcResolver(Env& aHost);
	virtual MIface* GetIfaceByUid(const string& aUid);
    protected:
	Env& mHost;
};

/** System observer 
 * Acts as proxy between the model and observers 
 */
class SystemObserver: public MAgentObserver
{
    public:
	SystemObserver(Env& aHost): mHost(aHost) {}
	TBool SetObserver(MAgentObserver* aObserver);
	TBool UnsetObserver(MAgentObserver* aObserver);
	// From MAgentObserver
	virtual void OnCompDeleting(const MUnit* aComp, TBool aSoft = ETrue, TBool aModif = EFalse) override;
	virtual void OnCompAdding(const MUnit* aComp, TBool aModif = EFalse) override;
	virtual TBool OnCompChanged(const MUnit* aComp, const string& aContName = string(), TBool aModif = EFalse) override;
	virtual TBool OnChanged(const MUnit* aComp) override;
	virtual TBool OnCompRenamed(const MUnit* aComp, const string& aOldName) override;
	virtual void OnCompMutated(const MUnit* aNode) override;
	virtual void OnError(const MUnit* aComp) override;
	// From MIface
	virtual MIface* Call(const string& aSpec, string& aRes);
    private:
	Env& mHost;
	/** Model observers */
	vector<MAgentObserver*> mObservers;
};

class Env: public Base, public MEnv
{
    friend class ChromoMgr;
    friend class SystemObserver;

    public:
    static const char* Type() { return "GEnv";};
    Env(const string& aSpecFile, const string& aLogFileName = string());
    Env(const string& aSpec, const string& aLogFileName, TBool aOpt);
    virtual ~Env();
    // Separated from constructor because constr can be followed by second phase - setting providers etc.
    TBool AddProvider(MProvider* aProv);
    void RemoveProvider(MProvider* aProv);
    // TODO [YB] To implement external iface provider as env module (plugin), to add provider of env modules
    void SetExtIfProv(MExtIfProv* aProv);
    public:
    // From Base
    virtual MIface *DoGetObj(const char *aName);
    // From MEnv
    virtual MProvider *Provider() const;
    virtual MLogRec *Logger();
    virtual MProfiler *Profiler() override;
    virtual MUnit* Root();
    virtual MChromoMgr* ChMgr();
    virtual MImportMgr* ImpsMgr();
    virtual MExtIfProv* ExtIfProv();
    virtual MIfaceResolver* IfaceResolver();
    virtual MAgentObserver* Observer() override { return mObserver;}
    virtual TBool GetSBool(TSBool aId) const;
    virtual void SetSBool(TSBool aId, TBool aVal);
    virtual void SetEVar(const string& aName, const string& aValue);
    virtual TBool GetEVar(const string& aName, string& aValue) const;
    virtual void ConstructSystem() override;
    virtual TBool RunSystem(TInt aCount = 0) override;
    virtual TBool StopSystem() override;
    virtual void OnRootDeleted();
    virtual TBool SetObserver(MAgentObserver* aObserver) override { return mObserver->SetObserver(aObserver);}
    virtual TBool UnsetObserver(MAgentObserver* aObserver) override { return mObserver->UnsetObserver(aObserver);}
    // From MIface
    virtual MIface* Call(const string& aSpec, string& aRes);
    string Mid() const override { return string();}
    public:
    /** Profiler helpers */
    inline void Pclock(PEvent::TId aEventId, MUnit* aNode) { Profiler()->Clock()(aEventId, aNode);}
    inline void Pdur(PEvent::TId aEventId) { Profiler()->Dur()(aEventId, nullptr);}
    inline void Pdstat(PEvent::TId aEventId, bool aStart) { Profiler()->DurStat()(aEventId, aStart);}

    protected:
    class EIfu: public Ifu {
	public:
	    EIfu();
    };

    private:
    GLogRec* iLogger; 
    Unit* iRoot;
    GFactory *iProvider;
    ChromoMgr* iChMgr;
    ImportsMgr* iImpMgr;
    string iSpecFile;
    string iSpec;
    MChromo* iSpecChromo;
    TBool mEnPerfTrace;
    TBool mEnIfTrace;
    map<string, string> mEVars;
    static EIfu mIfu;
    MExtIfProv* mExtIfProv;
    IfcResolver* mIfResolver;
    /** System observer */
    SystemObserver* mObserver;
    /** Profiler */
    GProfiler* mProf;
    MLauncher* mLauncher = NULL; /*!< Model's launcher */
};


#endif
