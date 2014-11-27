#ifndef __GRAYB_MENV_H
#define __GRAYB_MENV_H
class MProvider; class MLogRec;
class Elem;
class MChromo;

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
};

// Executive environment interface
class MEnv
{
    public:
	virtual MProvider *Provider() const = 0;
	virtual MLogRec *Logger() = 0;
	virtual Elem* Root() = 0;
	virtual MChromoMgr* ChMgr() = 0;
};


#endif
