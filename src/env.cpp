
#include "base.h"
#include "env.h"
#include "log.h"
#include "factr.h"
#include "elem.h"
#include "prov.h"
#include "chromo.h"
#include "ifu.h"
#include <stdlib.h>
#include <sys/time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdexcept> 
#include "profiler_events.h"

const string KLogFileName = "faplog.txt";
const char* KRootName = "Root";
// TODO [YB] to get from build env
const string ImportsMgr::KDefImportPath = "/usr/share/grayb/modules";
const string ImportsMgr::KImportsContainerUri = "./Modules";

Env::EIfu Env::mIfu;

// Ifu static initialisation
Env::EIfu::EIfu()
{
    RegMethod("Root", 0);
    RegMethod("Logger", 0);
    RegMethod("ConstructSystem", 0);
    RegMethod("SetEVar", 2);
    RegMethod("GetEVar", 1);
}

ImportsMgr::ImportsMgr(Env& aHost): Base(), mHost(aHost)
{
    AddImportsPaths(KDefImportPath);
}

ImportsMgr::~ImportsMgr()
{
}

void ImportsMgr::AddImportModulesInfo(const string& aPath)
{
    const string& dirpath = aPath;
    DIR* dp;
    struct dirent *dirp;
    struct stat filestat;
    dp = opendir(dirpath.c_str());
    if (dp != NULL) {
	while ((dirp = readdir(dp))) {
	    string fname(dirp->d_name);
	    string filepath = dirpath + "/" + fname;
	    // If the file is a directory (or is in some way invalid) we'll skip it
	    if (stat( filepath.c_str(), &filestat )) continue;
	    if (S_ISDIR( filestat.st_mode ))         continue;
	    string spec_ext;
	    mHost.ChMgr()->GetChromoRslArgs(spec_ext);
	    if(fname.substr(fname.find_last_of(".") + 1) != spec_ext) continue;
	    // Get chromo root as the module name
	    MChromo *spec = mHost.Provider()->CreateChromo();
	    __ASSERT(spec != NULL);
	    spec->SetFromFile(filepath);
	    string rname = spec->Root().Name();
	    mModsPaths.insert(pair<string, string>(rname, filepath));
	    delete spec;
	}
	closedir(dp);
    } else {
	mHost.Logger()->Write(EErr, NULL, "Collecting modules, cannot open imports dir [%s]", dirpath.c_str());
    }
}

void ImportsMgr::GetModulesNames(vector<string>& aModules) const
{
    for (map<string, string>::const_iterator it = mModsPaths.begin(); it != mModsPaths.end(); it++) {
	aModules.push_back(it->first);
    }
}

void ImportsMgr::ResetImportsPaths()
{
    mImportsPaths.clear();
    mModsPaths.clear();
}

void ImportsMgr::AddImportsPaths(const string& aPaths)
{
    mImportsPaths.push_back(aPaths);
    AddImportModulesInfo(aPaths);
}

string ImportsMgr::GetModulePath(const string& aModName) const
{
    string res;
    if (mModsPaths.count(aModName) != 0) {
	res = mModsPaths.at(aModName);
    }
    return res;
}

MUnit* ImportsMgr::GetImportsContainer() const
{
    return mHost.Root()->GetNode(KImportsContainerUri);
}

TBool ImportsMgr::Import(const string& aUri)
{
    MUnit* res = DoImport(aUri);
    return res != NULL;
}

// aUri - model hier uri
MUnit* ImportsMgr::DoImport(const string& aUri)
{
    mHost.Pdstat(PEvents::DurStat_DoImport, true);
    MUnit* res = NULL;
    GUri moduri(aUri);
    GUri::const_elem_iter it = moduri.Elems().begin();
    it++;
    GUri::TElem urie = *it;
    string modname = urie.name();
    string modpath = GetModulePath(modname);
    if (!modpath.empty()) {
	// Explicit chromo uri
	// Get the whole external chromo
	MUnit* icontr = GetImportsContainer();
	__ASSERT(icontr != NULL);
	MChromo* chromo = mHost.Provider()->CreateChromo();
	TBool res1 = chromo->Set(modpath);
	// Getting chromo nodes by model hier URI, ref ds_mod_prb_uri_chromo
	ChromoNode sel = chromo->Root().GetNodeByMhUri(moduri);
	if (sel.Handle() != NULL) {
	    // Reduce chromo to target node, mutate and check
	    chromo->ReduceToSelection(sel);
	    mHost.Pdstat(PEvents::DurStat_DoImport, false);
	    ImportToNode(icontr, chromo->Root(), sel);
	    // Rebasing uri to mut root, and get the target node
	    GUri selr(".");
	    GUri::const_elem_iter it = moduri.Elems().begin();
	    it++;
	    selr.AppendTail(moduri, it);
	    MUnit* node = icontr->GetNode(selr);
	    if (node != NULL) {
		mHost.Logger()->Write(EInfo, NULL, "Imported node: [%s]", aUri.c_str());
		res = node->GetObj(res);
	    } else {
		mHost.Logger()->Write(EErr, NULL, "Importing node: failed [%s]", aUri.c_str());
	    }
	} else {
	    mHost.Pdstat(PEvents::DurStat_DoImport, false);
	    mHost.Logger()->Write(EErr, NULL, "Importing to module: cannot find chromo node [%s]", aUri.c_str());
	}
	delete chromo;
    } else {
	mHost.Pdstat(PEvents::DurStat_DoImport, false);
	mHost.Logger()->Write(EErr, NULL, "Importing [%s]: cannot find module [%s]", aUri.c_str(), modname.c_str());
    }
    return res;
}

void ImportsMgr::ImportToNode(MUnit* aNode, const ChromoNode& aMut, const ChromoNode& aSel)
{
    if (aMut.Type() == ENt_Import) {
    } else if (aMut.Type() == ENt_Node) {
	mHost.Pdstat(PEvents::DurStat_MutImportToNode, true);
	GUri uri(".");
	uri.AppendElem(string(), aMut.Name());
	MUnit* comp = aNode->GetNode(uri);
	if (comp == NULL) {
	    // Node doesn't exist yet, to mutate
	    // Using external mut instead of nodes mut to avoid mut update
	    // on recursive import
	    MChromo* mut = mHost.Provider()->CreateChromo();
	    mut->Init(ENt_Node);
	    mut->Root().AddChild(aMut);
	    mHost.Pdstat(PEvents::DurStat_MutImportToNode, false);
	    MElem* enode = aNode->GetObj(enode);
	    if (enode != NULL) {
		TNs ns; MutCtx ctx(aNode, ns);
		enode->Mutate(mut->Root(), ETrue, EFalse, EFalse, ctx);
	    } else {
		mHost.Logger()->Write(EErr, NULL, "Node [%s] is not mutable", aNode->GetUri(NULL, ETrue).c_str());
	    }
	    delete mut;
	} else {
	    // Node already exists, go to lower layer if sel is not achieved yet
	    if (!(aMut == aSel)) {
		for (ChromoNode::Const_Iterator it = aMut.Begin(); it != aMut.End(); it++) {
		    ImportToNode(comp, *it, aSel);
		}
	    }
	}
	mHost.Pdstat(PEvents::DurStat_MutImportToNode, false);
    }
}

MUnit* ImportsMgr::OnUriNotResolved(MUnit* aNode, const GUri& aUri)
{
    MUnit* res = NULL;
    MUnit* icontr = GetImportsContainer();
    if (icontr != NULL && icontr->IsComp(aNode)) {
	GUri buri;
	buri.AppendElem("", "");
	GUri nuri;
	string suri;
	TBool rebased = EFalse;
	if (aUri.IsAbsolute()) {
	    rebased = icontr->RebaseUri(aUri, icontr, nuri);
	    buri.Append(nuri);
	} else {
	    // Combine full uri from path to base and base path to destination
	    aNode->GetUri(nuri, icontr);
	    buri.Append(nuri);
	    buri.Append(aUri);
	    rebased = ETrue;
	}
	if (rebased) {
	    suri = buri.toString(ETrue);
	    res = DoImport(suri);
	}
    }
    return res;
}



ChromoMgr::ChromoMgr(Env& aHost): Base(), mHost(aHost), mLim(0), 
    mEnablePhenoModif(EFalse), mEnableFixErrors(EFalse), mEnableReposMuts(EFalse),
    mEnableCheckSafety(EFalse), mEnableOptimization(ETrue) 
{
}

ChromoMgr::~ChromoMgr()
{
}

int ChromoMgr::GetMaxOrder() const
{
    TInt res = 0;
    MUnit* root = mHost.Root();
    MElem* eroot = root->GetObj(eroot);
    if (eroot != NULL) {
	ChromoNode& chrroot = eroot->Chromos().Root();
	ChromoNode sroot = *(chrroot.Root());
	res = sroot.GetOrder(ETrue);
    }
    return res;
}

int ChromoMgr::GetSpecMaxOrder() const
{
    TInt res = 0;
    ChromoNode& sroot = mHost.iSpecChromo->Root();
    res = sroot.Count();
    return res;
}

void ChromoMgr::SetEnablePhenoModif(TBool aEnable)
{
    if (mEnablePhenoModif != aEnable) {
	mEnablePhenoModif = aEnable;
    }
}

void ChromoMgr::SetEnableFixErrors(TBool aEnable)
{
    if (mEnableFixErrors != aEnable) {
	mEnableFixErrors = aEnable;
    }
}

void ChromoMgr::SetEnableReposMuts(bool aEnable)
{
    if (mEnableReposMuts != aEnable) {
	mEnableReposMuts = aEnable;
    }
}

void ChromoMgr::SetEnableCheckSafety(bool aEnable)
{
    if (mEnableCheckSafety != aEnable) {
	mEnableCheckSafety = aEnable;
    }
}

void ChromoMgr::SetEnableOptimization(bool aEnable)
{
    if (mEnableOptimization != aEnable) {
	mEnableOptimization = aEnable;
    }
}

void ChromoMgr::SetChromoRslArgs(const string& aArgs)
{
    mHost.iProvider->SetChromoRslArgs(aArgs);
}

void ChromoMgr::GetChromoRslArgs(string& aArgs)
{
    mHost.iProvider->GetChromoRslArgs(aArgs);
}

IfcResolver::IfcResolver(Env& aHost): mHost(aHost)
{
}

MIface* IfcResolver::GetIfaceByUid(const string& aUid)
{
    MIface* res = NULL;
    string suri, type;
    Ifu::ParseUid(aUid, suri, type);
    GUri uri(suri);
    if (!uri.IsErr()) {
	MUnit* node = NULL;
	__ASSERT(!uri.IsAbsolute());
	// Uid is generated as relative UID from local root, ref ds_daa_pxdup_birc
	node = mHost.Root()->GetNode(uri);
	if (node != NULL) {
	    res = (MIface*)(node->GetObj(type.c_str()));
	}
    }
    return res;
}


/** System observer */

void SystemObserver::SetObserver(MAgentObserver* aObserver)
{
    __ASSERT(aObserver != NULL);
    TBool found = EFalse;
    for (auto observer : mObservers) {
	if (observer == aObserver) {
	    found = ETrue; break;
	}
    }
    __ASSERT(!found);
    mObservers.push_back(aObserver);
}

void SystemObserver::UnsetObserver(MAgentObserver* aObserver)
{
    __ASSERT(aObserver != NULL);
    TBool found = EFalse;
    for (auto it = mObservers.begin(); it != mObservers.end(); it++) {
	if (*it == aObserver) {
	    mObservers.erase(it); found = ETrue; break;
	}
    }
    __ASSERT(found);
}


void SystemObserver::OnCompDeleting(MUnit& aComp, TBool aSoft, TBool aModif)
{
    for (auto observer : mObservers) {
	observer->OnCompDeleting(aComp, aSoft, aModif);
    }
}

void SystemObserver::OnCompAdding(MUnit& aComp, TBool aModif)
{
    for (auto observer : mObservers) {
	observer->OnCompAdding(aComp, aModif);
    }
}

TBool SystemObserver::OnCompChanged(MUnit& aComp, const string& aContName, TBool aModif)
{
    for (auto observer : mObservers) {
	observer->OnCompChanged(aComp, aContName, aModif);
    }
}

TBool SystemObserver::OnChanged(MUnit& aComp)
{
    for (auto observer : mObservers) {
	observer->OnChanged(aComp);
    }
}

TBool SystemObserver::OnCompRenamed(MUnit& aComp, const string& aOldName)
{
    for (auto observer : mObservers) {
	observer->OnCompRenamed(aComp, aOldName);
    }
}

void SystemObserver::OnCompMutated(const MUnit* aNode)
{
    for (auto observer : mObservers) {
	observer->OnCompMutated(aNode);
    }
}


// TODO to implement
MIface* SystemObserver::Call(const string& aSpec, string& aRes)
{
    return NULL;
}



Env::Env(const string& aSpecFile, const string& aLogFileName): Base(), iRoot(NULL), iLogger(NULL),
    iSpecChromo(NULL), mEnPerfTrace(EFalse), mEnIfTrace(EFalse), mExtIfProv(NULL), mIfResolver(NULL),
    mObserver(NULL), mProf(NULL)
{
    iLogger = new GLogRec(aLogFileName.empty() ? KLogFileName : aLogFileName);
    iProvider = new GFactory(string(), this);
    iProvider->LoadPlugins();
    iSpecFile = aSpecFile;
    srand(time(NULL));
    iChMgr = new ChromoMgr(*this);
    iImpMgr = new ImportsMgr(*this);
    mIfResolver = new IfcResolver(*this);    
    mObserver = new SystemObserver(*this);
    mProf = new GProfiler(this, KPInitData);
    string chromo_fext = iSpecFile.substr(iSpecFile.find_last_of(".") + 1);
    iChMgr->SetChromoRslArgs(chromo_fext);
    // Profilers events
    /*
    mPfid_Start_Constr = Profiler()->RegisterEvent(TPEvent("Start construction"));
    mPfid_Root_Created = Profiler()->RegisterEvent(TPEvent("Root created"));
    mPfid_Root_Created_From_Start = Profiler()->RegisterEvent(TPEvent("Root created from start", mPfid_Start_Constr));
    mPfid_End_Constr = Profiler()->RegisterEvent(TPEvent("End construction"));
    */
}

Env::Env(const string& aSpec, const string& aLogFileName, TBool aOpt): Base(), iRoot(NULL), iLogger(NULL),
    iSpecChromo(NULL), mEnPerfTrace(EFalse), mEnIfTrace(EFalse), mExtIfProv(NULL), mIfResolver(NULL),
    mObserver(NULL), mProf(NULL)
{
    iLogger = new GLogRec(aLogFileName.empty() ? KLogFileName : aLogFileName);
    //iLogger = new GLogRec("Logger", aName + ".log");
    iProvider = new GFactory(string(), this);
    iProvider->LoadPlugins();
    iSpec= aSpec;
    srand(time(NULL));
    iChMgr = new ChromoMgr(*this);
    iImpMgr = new ImportsMgr(*this);
    mIfResolver = new IfcResolver(*this);    
    mObserver = new SystemObserver(*this);
    mProf = new GProfiler(this, KPInitData);
    string chromo_fext = iSpecFile.substr(iSpecFile.find_last_of(".") + 1);
    iChMgr->SetChromoRslArgs(chromo_fext);
    // Profilers events
    /*
    mPfid_Start_Constr = Profiler()->RegisterEvent(TPEvent("Start construction"));
    mPfid_Root_Created = Profiler()->RegisterEvent(TPEvent("Root created"));
    mPfid_Root_Created_From_Start = Profiler()->RegisterEvent(TPEvent("Root created from start", mPfid_Start_Constr));
    mPfid_End_Constr = Profiler()->RegisterEvent(TPEvent("End construction"));
    */
}


Env::~Env()
{
    delete mExtIfProv;
    //Logger()->Write(EInfo, iRoot, "Starting deleting system");
    if (iRoot != NULL) {
	delete iRoot;
    }
    //Logger()->Write(EInfo, NULL, "Finished deleting system");
    delete iChMgr;
    delete iImpMgr;
    delete iProvider;
    delete iLogger;
    delete mIfResolver;
    delete mObserver;
    delete mProf;
}

// TODO [YB] To integrate into env creation
void Env::ConstructSystem()
{
    // Create root system
    // TODO [YB] Potentially the root also can be inherited form parent
    MChromo *spec = iProvider->CreateChromo();
    iSpecChromo = spec;
    if (iSpecFile.empty() && iSpec.empty()) {
	iRoot = new Elem(KRootName, NULL, this);
    }
    else {
	if (!iSpecFile.empty()) {
	    spec->SetFromFile(iSpecFile);
	} else {
	    spec->SetFromSpec(iSpec);
	}
	Pdur(PEvents::Dur_Profiler_Dur_Start);
	Pdur(PEvents::Dur_Profiler_Dur);
	Pclock(PEvents::Env_Start_Constr, iRoot);
	Pdur(PEvents::Dur_Env_Constr_Start);
	const ChromoNode& root = spec->Root();

	/**/
	string sparent = root.Attr(ENa_Parent);
	Unit* parent = iProvider->GetNode(sparent);
	iRoot = iProvider->CreateNode(sparent, root.Name(), NULL, this);
	/**/
	//iRoot = iProvider->CreateNode("Elem", "Root", NULL, this);
	MElem* eroot = (iRoot == NULL) ? NULL : iRoot->GetObj(eroot);
	if (eroot != NULL) {
	    Pclock(PEvents::Env_Root_Created, iRoot);
	    stringstream ss;
	    struct timeval tp;
	    gettimeofday(&tp, NULL);
	    long int beg_us = tp.tv_sec * 1000000 + tp.tv_usec;
	    Logger()->Write(EInfo, iRoot, "Started of creating system, spec [%s]", iSpecFile.c_str());
	    eroot->SetMutation(root);
	    TNs ns;
	    MutCtx mc(iRoot, ns);
	    //eroot->Mutate(EFalse, EFalse, EFalse, MutCtx(iRoot));
	    eroot->Mutate(EFalse, EFalse, EFalse, mc);
	    Pclock(PEvents::Env_End_Constr, iRoot);
	    gettimeofday(&tp, NULL);
	    long int fin_us = tp.tv_sec * 1000000 + tp.tv_usec;
	    ss << (fin_us - beg_us);
	    TInt cpc = iRoot->GetCapacity();
	    TInt cpi = iImpMgr->GetImportsContainer() ? iImpMgr->GetImportsContainer()->GetCapacity() : 0;
	    Logger()->Write(EInfo, iRoot, "Completed of creating system, nodes: %d, imported: %d, time, us: %s", cpc,  cpi, ss.str().c_str());
	    //Logger()->Write(EInfo, iRoot, "Components");
	    //iRoot->LogComps();
	}
	else {
	    Logger()->WriteFormat("Env: cannot create root elem");
	}
	Pdur(PEvents::Dur_Env_Constr);
    }
}

MChromoMgr* Env::ChMgr()
{
    return iChMgr;
}

MImportMgr* Env::ImpsMgr()
{
    return iImpMgr;
}

MProvider *Env::Provider() const
{
    return iProvider;
}

MLogRec *Env::Logger()
{
    return iLogger;
}

MUnit* Env::Root() 
{ 
    return iRoot; 
}

void Env::AddProvider(MProvider* aProv)
{
    __ASSERT(aProv != NULL);
    iProvider->AddProvider(aProv);
    aProv->SetEnv(NULL);
    aProv->SetEnv(this);
}

void Env::RemoveProvider(MProvider* aProv)
{
    iProvider->RemoveProvider(aProv);
}

MIface *Env::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MEnv::Type()) == 0) res = dynamic_cast<MEnv*>(this);
    return res;
}

TBool Env::GetSBool(TSBool aId) const
{
    TBool res = EFalse;
    if (aId == ESb_EnPerfTrace) res = mEnPerfTrace;
    else if (aId == ESb_EnIfTrace) res = mEnIfTrace;
    return res;
}

void Env::SetSBool(TSBool aId, TBool aVal)
{
    if (aId == ESb_EnPerfTrace) mEnPerfTrace = aVal;
    else if (aId == ESb_EnIfTrace) mEnIfTrace = aVal;
}

MIface* Env::Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = mIfu.CheckMname(name);
    if (!name_ok) 
	    throw (runtime_error("Wrong method name"));
    TBool args_ok = mIfu.CheckMpars(name, args.size());
    if (!args_ok) 
	    throw (runtime_error("Wrong arguments number"));
    if (name == "Root") {
	res = Root();
	if (res == NULL) 
	    throw (runtime_error("Cannot find root node"));
    } else if (name == "Logger") {
	res = Logger();
    } else if (name == "ConstructSystem") {
	ConstructSystem();
    } else if (name == "SetEVar") {
	SetEVar(args.at(0), args.at(1));
    } else if (name == "GetEVar") {
	TBool rres = GetEVar(args.at(0), aRes);
	if (!rres) 
	    throw (runtime_error("Cannot find variable"));
    } else if (name == "SetObserver") {
	MExtIfProv* prov = ExtIfProv();
	if (args.at(0) == GUri::Nil()) {
	    SetObserver(NULL);
	} else {
	    if (prov == NULL) {
		throw (runtime_error("Cannot get ext iface provider"));
	    }
	    MIface* iobs = prov->GetEIface(args.at(0), MAgentObserver::Type());
	    if (iobs == NULL) {
		throw (runtime_error("Cannot get agent observer iface" + args.at(0)));
	    }
	    MAgentObserver* obs = dynamic_cast<MAgentObserver*>(iobs);
	    if (obs == NULL) {
		throw (runtime_error("Cannot get agent observer" + args.at(0)));
	    }
	    SetObserver(obs);
	}
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return res;
}

void Env::SetEVar(const string& aName, const string& aValue)
{
    mEVars.insert(pair<string, string>(aName, aValue));
}

TBool Env::GetEVar(const string& aName, string& aValue) const
{
    TBool res = EFalse;
    if (mEVars.count(aName) > 0) {
	aValue = mEVars.at(aName);
	res = ETrue;
    }
    return res;
}

void Env::SetExtIfProv(MExtIfProv* aProv)
{
    __ASSERT(mExtIfProv == NULL);
    mExtIfProv = aProv;
    mExtIfProv->SetEnv(this);
}

MExtIfProv* Env::ExtIfProv()
{
    return mExtIfProv;
}

MIfaceResolver* Env::IfaceResolver()
{
    return mIfResolver;
}

// Handle notification of root deleted, ref ds_daa_rdo
void Env::OnRootDeleted()
{
    iRoot = NULL;
}

MProfiler* Env::Profiler()
{
    return mProf;
}
