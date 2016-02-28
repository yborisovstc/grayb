
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

void *ImportsMgr::DoGetObj(const char *aName)
{
    return (strcmp(aName, Type()) == 0) ? this : NULL;
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
	    if(fname.substr(fname.find_last_of(".") + 1) != "xml") continue;
	    // Get chromo root as the module name
	    Chromo *spec = mHost.Provider()->CreateChromo();
	    spec->Set(filepath.c_str());
	    string rname = spec->Root().Name();
	    mModsPaths.insert(pair<string, string>(rname, filepath));
	    delete spec;
	}
	closedir(dp);
    } else {
	mHost.Logger()->Write(MLogRec::EErr, NULL, "Collecting modules, cannot open imports dir [%s]", dirpath.c_str());
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

MElem* ImportsMgr::GetImportsContainer() const
{
    return mHost.Root()->GetNode(KImportsContainerUri);
}

#if 0
TBool ImportsMgr::Import(const string& aUri)
{
    TBool res = ETrue;
    GUri moduri(aUri);
    GUri::const_elem_iter it = moduri.Elems().begin();
    it++;
    GUri::TElem urie = *it;
    string modname = urie.second.second;
    string modpath = GetModulePath(modname);
    if (!modpath.empty()) {
	// Explicit chromo uri
	// Get the whole external chromo
	Elem* icontr = GetImportsContainer();
	__ASSERT(icontr != NULL);
	TBool res1 = icontr->AppendMutation(modpath);
	// Rebasing uri to mut root, and get the target node
	GUri selr(".");
	GUri::const_elem_iter it = moduri.Elems().begin();
	it++;
	selr.AppendTail(moduri, it);
	ChromoNode sel = icontr->Mutation().Root().GetNode(selr);
	if (sel.Handle() != NULL) {
	    // Reduce chromo to target node, mutate and check
	    icontr->Mutation().ReduceToSelection(sel);
	    icontr->Mutate(ETrue);
	    res = icontr->GetNode(selr);
	    if (res) {
		mHost.Logger()->Write(MLogRec::EInfo, NULL, "Imported node: [%s]", aUri.c_str());
	    } else {
		mHost.Logger()->Write(MLogRec::EErr, NULL, "Importing node: failed [%s]", aUri.c_str());
	    }
	} else {
	    mHost.Logger()->Write(MLogRec::EErr, NULL, "Importing to module: cannot find chromo node [%s]", aUri.c_str());
	}
    }
    return res;
}
#endif

TBool ImportsMgr::Import(const string& aUri)
{
    MElem* res = DoImport(aUri);
    return res != NULL;
}

// aUri - model hier uri
MElem* ImportsMgr::DoImport(const string& aUri)
{
    MElem* res = NULL;
    GUri moduri(aUri);
    GUri::const_elem_iter it = moduri.Elems().begin();
    it++;
    GUri::TElem urie = *it;
    string modname = urie.second.second;
    string modpath = GetModulePath(modname);
    if (!modpath.empty()) {
	// Explicit chromo uri
	// Get the whole external chromo
	MElem* icontr = GetImportsContainer();
	__ASSERT(icontr != NULL);
	Chromo* chromo = mHost.Provider()->CreateChromo();
	TBool res1 = chromo->Set(modpath);
	// Getting chromo nodes by model hier URI, ref ds_mod_prb_uri_chromo
	ChromoNode sel = chromo->Root().GetNodeByMhUri(moduri);
	if (sel.Handle() != NULL) {
	    // Reduce chromo to target node, mutate and check
	    chromo->ReduceToSelection(sel);
	    ImportToNode(icontr, chromo->Root(), sel);
	    // Rebasing uri to mut root, and get the target node
	    GUri selr(".");
	    GUri::const_elem_iter it = moduri.Elems().begin();
	    it++;
	    selr.AppendTail(moduri, it);
	    res = icontr->GetNode(selr);
	    if (res != NULL) {
		mHost.Logger()->Write(MLogRec::EInfo, NULL, "Imported node: [%s]", aUri.c_str());
	    } else {
		mHost.Logger()->Write(MLogRec::EErr, NULL, "Importing node: failed [%s]", aUri.c_str());
	    }
	} else {
	    mHost.Logger()->Write(MLogRec::EErr, NULL, "Importing to module: cannot find chromo node [%s]", aUri.c_str());
	}
	delete chromo;
    } else {
	mHost.Logger()->Write(MLogRec::EErr, NULL, "Importing [%s]: cannot find module [%s]", aUri.c_str(), modname.c_str());
    }
    return res;
}

void ImportsMgr::ImportToNode(MElem* aNode, const ChromoNode& aMut, const ChromoNode& aSel)
{
    if (aMut.Type() == ENt_Import) {
    } else if (aMut.Type() == ENt_Node) {
	GUri uri(".");
	uri.AppendElem(string(), aMut.Name());
	MElem* comp = aNode->GetNode(uri);
	if (comp == NULL) {
	    // Node doesn't exist yet, to mutate
	    // Using external mut instead of nodes mut to avoid mut update
	    // on recursive import
	    Chromo* mut = mHost.Provider()->CreateChromo();
	    mut->Init(ENt_Node);
	    mut->Root().AddChild(aMut);
	    aNode->Mutate(mut->Root(), ETrue, EFalse, EFalse, aNode);
	    delete mut;
	} else {
	    // Node already exists, go to lower layer if sel is not achieved yet
	    if (!(aMut == aSel)) {
		for (ChromoNode::Const_Iterator it = aMut.Begin(); it != aMut.End(); it++) {
		    ImportToNode(comp, *it, aSel);
		}
	    }
	}
    }
}

MElem* ImportsMgr::OnUriNotResolved(MElem* aNode, const GUri& aUri)
{
    MElem* res = NULL;
    MElem* icontr = GetImportsContainer();
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
	    suri = buri.GetUri(ETrue);
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

void *ChromoMgr::DoGetObj(const char *aName)
{
    return (strcmp(aName, Type()) == 0) ? this : NULL;
}

int ChromoMgr::GetMaxOrder() const
{
    TInt res = 0;
    Elem* eroot = mHost.Root();
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


Env::Env(const string& aSpecFile, const string& aLogFileName): Base(), iRoot(NULL), iLogger(NULL),
    iSpecChromo(NULL), mEnPerfTrace(EFalse), mEnIfTrace(EFalse)
{
    iLogger = new GLogRec(aLogFileName.empty() ? KLogFileName : aLogFileName);
    iProvider = new GFactory(string(), this);
    iProvider->LoadPlugins();
    iSpecFile = aSpecFile;
    srand(time(NULL));
    iChMgr = new ChromoMgr(*this);
    iImpMgr = new ImportsMgr(*this);
}

Env::Env(const string& aSpec, const string& aLogFileName, TBool aOpt): Base(), iRoot(NULL), iLogger(NULL),
    iSpecChromo(NULL), mEnPerfTrace(EFalse), mEnIfTrace(EFalse)
{
    iLogger = new GLogRec(aLogFileName.empty() ? KLogFileName : aLogFileName);
    //iLogger = new GLogRec("Logger", aName + ".log");
    iProvider = new GFactory(string(), this);
    iProvider->LoadPlugins();
    iSpec= aSpec;
    srand(time(NULL));
    iChMgr = new ChromoMgr(*this);
    iImpMgr = new ImportsMgr(*this);
}


Env::~Env()
{
    Logger()->Write(MLogRec::EInfo, iRoot, "Starting deleting system");
    delete iRoot;
    Logger()->Write(MLogRec::EInfo, NULL, "Finished deleting system");
    delete iChMgr;
    delete iImpMgr;
    delete iProvider;
    delete iLogger;
}

// TODO [YB] To integrate into env creation
void Env::ConstructSystem()
{
    // Create root system
    // TODO [YB] Potentially the root also can be inherited form parent
    Chromo *spec = iProvider->CreateChromo();
    iSpecChromo = spec;
    if (iSpecFile.empty() && iSpec.empty()) {
	iRoot = new Elem(KRootName, NULL, this);
    }
    else {
	if (!iSpecFile.empty()) {
	    spec->Set(iSpecFile.c_str());
	} else {
	    spec->SetFromSpec(iSpec);
	}
	const ChromoNode& root = spec->Root();
	string sparent = root.Attr(ENa_Parent);
	Elem* parent = iProvider->GetNode(sparent);
	iRoot = iProvider->CreateNode(sparent, root.Name(), NULL, this);
	if (iRoot != NULL) {
	    stringstream ss;
	    struct timeval tp;
	    gettimeofday(&tp, NULL);
	    long int beg_us = tp.tv_sec * 1000000 + tp.tv_usec;
	    Logger()->Write(MLogRec::EInfo, iRoot, "Started of creating system, spec [%s]", iSpecFile.c_str());
	    iRoot->SetMutation(root);
	    iRoot->Mutate(EFalse, EFalse, EFalse, iRoot);
	    gettimeofday(&tp, NULL);
	    long int fin_us = tp.tv_sec * 1000000 + tp.tv_usec;
	    ss << (fin_us - beg_us);
	    TInt cpc = iRoot->GetCapacity();
	    Logger()->Write(MLogRec::EInfo, iRoot, "Completed of creating system, nodes: %d, time, us: %s", cpc,  ss.str().c_str());
	    //Logger()->Write(MLogRec::EInfo, iRoot, "Components");
	    //iRoot->LogComps();
	}
	else {
	    Logger()->WriteFormat("Env: cannot create elem [%s] of type [%s]", root.Name().c_str(), sparent.c_str());
	}
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

Elem* Env::Root() 
{ 
    return iRoot; 
}

void Env::AddProvider(GProvider* aProv)
{
    iProvider->AddProvider(aProv);
    aProv->SetEnv(NULL);
    aProv->SetEnv(this);
}

void Env::RemoveProvider(GProvider* aProv)
{
    iProvider->RemoveProvider(aProv);
}

void *Env::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) res = this;
    else if (strcmp(aName, MEnv::Type()) == 0) res = (MEnv*) this;
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
    } else if (name == "ConstructSystem") {
	ConstructSystem();
    } else if (name == "SetEVar") {
	SetEVar(args.at(0), args.at(1));
    } else if (name == "GetEVar") {
	TBool rres = GetEVar(args.at(0), aRes);
	if (!rres) 
	    throw (runtime_error("Cannot find variable"));
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return res;
}

string Env::Uid() const
{
    return GUriBase::KIfaceSepS + MEnv::Type();
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

