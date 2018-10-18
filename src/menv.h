#ifndef __GRAYB_MENV_H
#define __GRAYB_MENV_H

#include <plat.h>
#include <miface.h>
#include <mprofiler.h>
#include <melem.h>

#include <vector>
#include <string>

class MProvider; class MLogRec;
class MElem;
class Elem;
class MChromo;
class GUri;


    using namespace std;

    class MEnv;

    // External ifaces instances provider
    class MExtIfProv
    {
	public:
	    virtual MIface* GetEIface(const string& aIfaceId, const string& aIfaceType) = 0;
	    virtual void SetEnv(MEnv* aEnv) = 0;
    };

    // Import manager
    class MImportMgr
    {
	public:
	    virtual void GetModulesNames(vector<string>& aModules) const = 0;
	    virtual void ResetImportsPaths() = 0;
	    virtual void AddImportsPaths(const string& aPaths) = 0;
	    virtual string GetModulePath(const string& aModName) const = 0;
	    virtual TBool Import(const string& aUri) = 0;
	    virtual MElem* OnUriNotResolved(MElem* aNode, const GUri& aUri) = 0;
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
    class MEnv: public MIface
    {
	public:
	    static const char* Type() { return "MEnv";};
	public:
	    enum TSBool
	    {
		ESb_EnPerfTrace = 1,
		ESb_EnIfTrace = 2 /* Enable tracing of interface providing */
	    };
	public:
	    virtual MProvider *Provider() const = 0;
	    virtual MLogRec *Logger() = 0;
	    virtual MProfiler *Profiler() = 0;
	    virtual MElem* Root() = 0;
	    virtual MChromoMgr* ChMgr() = 0;
	    virtual MImportMgr* ImpsMgr() = 0;
	    virtual MExtIfProv* ExtIfProv() = 0;
	    virtual MIfaceResolver* IfaceResolver() = 0;
	    virtual MAgentObserver* Observer() = 0;
	    virtual TBool GetSBool(TSBool aId) const = 0;
	    virtual void SetSBool(TSBool aId, TBool aVal) = 0;
	    virtual void SetEVar(const string& aName, const string& aValue) = 0;
	    virtual TBool GetEVar(const string& aName, string& aValue) const = 0;
	    virtual void ConstructSystem() = 0;
	    // Notification of root deleted, ref ds_daa_rdo
	    virtual void OnRootDeleted() = 0;
	    virtual void SetObserver(MAgentObserver* aObserver) = 0;
	    virtual void UnsetObserver(MAgentObserver* aObserver) = 0;
	public:
	    // From MIface
	    virtual MIface* Call(const string& aSpec, string& aRes) = 0;
	    virtual string Uid() const { return Mid() + "%" + Type();};
    };

#endif
