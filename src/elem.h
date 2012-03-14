#ifndef __GRAYB_ELEM_H
#define __GRAYB_ELEM_H

#include "melem.h"
#include "menv.h"
#include "mlog.h"
#include "mmuta.h"
#include "base.h"
#include "chromo.h"

class Chromo;
class MProvider;

// Element of native hier - mutable
class Elem: public Base, public MMutable 
{
    public:
	// name, parent
	typedef pair<string, string> TCkey;
	typedef pair<TCkey, Elem*> TCelem;
    public:
	static const char* Type() { return "Elem";};
	Elem(const string &aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	Elem* GetNode(const GUri& aUri);
	virtual ~Elem();
	void SetEType(const string& aEType);
	void SetParent(const string& aParent);
	void SetMan(Elem* aMan);
	void SetMutation(const ChromoNode& aMuta);
	void Mutate(TBool aRunTimeOnly = EFalse);
	string PName() const;
	const map<TCkey, Elem*>& Comps() const;
    public:
	Elem* CreateHeir(const string& aName, Elem* aMan);
	const MChromo& Chromos() { return *iChromo;};
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MElem
	virtual const string& EType() const;
	virtual const set<string>& CompsTypes();
	virtual Elem* Clone(const string& aName, Elem* aMan, MEnv* aEnv) const;
	virtual Elem* GetMan();
	virtual Elem* GetNode(const string& aUri);
	virtual Elem* GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase);
	virtual TBool ChangeCont(const string& aVal); 
	virtual TBool AddNode(const ChromoNode& aSpec);
	virtual void OnCompDeleting(Elem& aComp);
	virtual void OnCompAdding(Elem& aComp);
	virtual void OnCompChanged(Elem& aComp);
	// From MMutable
	virtual void DoMutation(const ChromoNode& aCromo, TBool aRunTime);
    protected:
	Elem* AddElem(const ChromoNode& aSpec);
	static void Init();
	inline MLogRec* Logger();
	inline MProvider* Provider() const;
	TBool AppendComp(Elem* aComp);
	Elem* GetComp(const string& aParent, const string& aName);
	TBool DoMutChangeCont(const ChromoNode& aSpec);
	Elem* GetComp(TInt aInd);
    protected:
	// Element type - parent's name
	string iEType;
	// Environment
	MEnv* iEnv;
	// Managed (higher) element of hier
	Elem* iMan;
	// Chromo
	Chromo* iChromo;
	// Mutation
	Chromo* iMut;
	// Components types
	static set<string> iCompsTypes;
	// Sign of inited
	static bool iInit;
	// Components
	map<TCkey, Elem*> iComps;
};

inline MLogRec* Elem::Logger() {return iEnv ? iEnv->Logger(): NULL; }

inline MProvider* Elem::Provider() const {return iEnv ? iEnv->Provider(): NULL; }

#endif
