#include "prov.h"
#include "factr.h"
#include "provdef.h"
#include "mplugin.h"
#include <dlfcn.h>
#include <dirent.h>


// Plugins default dir
string KPluginDir = "/usr/lib/fap2/plugins/";

GFactory::GFactory(const string& aName, MEnv* aEnv): Base(), iName(aName), iEnv(aEnv)
{
    GProvider* baseprov = new ProvDef("ProvDef", iEnv);
    __ASSERT(baseprov != NULL);
    AddProvider(baseprov);
}

GFactory::~GFactory()
{
    while (!iProviders.empty()) {
	MProvider* prov = iProviders.begin()->second;
	delete prov;
	iProviders.erase(iProviders.begin());
    }
}

void *GFactory::DoGetObj(const char *aName)
{
    return (strcmp(aName, Type()) == 0) ? this : NULL;
}

Elem* GFactory::CreateNode(const string& aType, const string& aName, MElem* aMan, MEnv* aEnv)
{
    Elem* res = NULL;
    for (map<string, MProvider*>::iterator it = iProviders.begin(); it != iProviders.end() && res == NULL; it++) {
	res = it->second->CreateNode(aType, aName, aMan, aEnv);
    }
    return res;
}

Elem* GFactory::GetNode(const string& aUri)
{
    Elem* res = NULL;
    for (TProviders::iterator it = iProviders.begin(); it != iProviders.end() && res == NULL; it++) {
	res = it->second->GetNode(aUri);
    }
    return res;
}

TBool GFactory::IsProvided(const MElem* aElem) const
{
    TBool res = EFalse;
    for (TProviders::const_iterator it = iProviders.begin(); it != iProviders.end() && !res; it++) {
	res = it->second->IsProvided(aElem);
    }
    return res;

}

Chromo* GFactory::CreateChromo()
{
    Chromo* res = NULL;
    for (TProviders::iterator it = iProviders.begin(); it != iProviders.end() && res == NULL; it++) {
	res = it->second->CreateChromo();
    }
    return res;
}

TBool GFactory::LoadPlugin(const string& aName)
{
    TBool res = EFalse;
    MProvider* prov = NULL;
    string plgpath = KPluginDir + aName;
    void* handle = dlopen(plgpath.c_str(), RTLD_NOW|RTLD_LOCAL|RTLD_DEEPBIND);
    if (handle != NULL) {
	dlerror();
	plugin_init_func_t* init = (plugin_init_func_t*) dlsym(handle, "init");
	char* str_error = dlerror();
	if (init!= NULL) {
	    prov = init(iEnv);
	    if (prov != NULL) {
		res = ETrue;
		AddProvider(prov);
	    }
	}
	if (!res) {
	    dlclose(handle);
	}
    }
    return res;
}

int GFactory::FilterPlgDirEntries(const struct dirent *aEntry)
{
    string name = aEntry->d_name;
    size_t ppos = name.find_first_of(".");
    string suff = name.substr(ppos + 1);
    int res = suff.compare("so"); 
    return (res == 0) ? 1 : 0;
}

void GFactory::LoadPlugins()
{
    // List plugins directory
    struct dirent **entlist;
    int n = scandir(KPluginDir.c_str(), &entlist, FilterPlgDirEntries, alphasort);
    // Load plugins
    for (int cnt = 0; cnt < n; ++cnt) {
	LoadPlugin(entlist[cnt]->d_name);
    }
}

void GFactory::AddProvider(MProvider* aProv)
{
    // TODO To support name
    TProviders::const_iterator res = iProviders.find(aProv->Name());
    __ASSERT(res == iProviders.end());
    iProviders.insert(TProvidersElem(aProv->Name(), aProv));
}

void GFactory::RemoveProvider(MProvider* aProv)
{
    __ASSERT(iProviders.count(aProv->Name()) > 0);
    iProviders.erase(aProv->Name());
}

void GFactory::AppendNodesInfo(vector<string>& aInfo)
{
    for (TProviders::iterator it = iProviders.begin(); it != iProviders.end(); it++) {
	it->second->AppendNodesInfo(aInfo);
    }
}

const string& GFactory::ModulesPath() const
{
    MProvider* defprov = iProviders.at("ProvDef");
    __ASSERT(defprov != NULL);
    return defprov->ModulesPath();
}

void GFactory::SetEnv(MEnv* aEnv)
{
    assert(iEnv == NULL || aEnv == NULL);
    iEnv = aEnv;
}
