
#include "base.h"
#include "env.h"
#include "log.h"
#include "factr.h"
#include "elem.h"
#include "prov.h"
#include "chromo.h"
#include <stdlib.h>

const string KLogFileName = "faplog.txt";
const char* KRootName = "Root";

ChromoMgr::ChromoMgr(const string& aName): Base(aName)
{
}

ChromoMgr::~ChromoMgr()
{
}

void *ChromoMgr::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx) 
{
    return (strcmp(aName, Type()) == 0) ? this : NULL;
}

TInt ChromoMgr::GetOrder(const MChromo& aChromo) const
{
}


Env::Env(const string& aName, const string& aSpecFile, const string& aLogFileName): Base(aName), iRoot(NULL), iLogger(NULL)
{
    iLogger = new GLogRec("Logger", aLogFileName.empty() ? KLogFileName : aLogFileName);
    iProvider = new GFactory("Factory", this);
    iProvider->LoadPlugins();
    iSystSpec = aSpecFile;
    srand(time(NULL));
    iChMgr = new ChromoMgr("ChromoMgr");
}

Env::~Env()
{
    delete iRoot;
    delete iLogger;
}

// TODO [YB] To integrate into env creation
void Env::ConstructSystem()
{
    // Create root system
    // TODO [YB] Potentially the root also can be inherited form parent
    Chromo *spec = iProvider->CreateChromo();
    if (iSystSpec.empty()) {
	iRoot = new Elem(KRootName, NULL, this);
    }
    else {
	spec->Set(iSystSpec.c_str());
	const ChromoNode& root = spec->Root();
//	iRoot = new Elem(root.Name(), NULL, this);
	string sparent = root.Attr(ENa_Parent);
	Elem* parent = iProvider->GetNode(sparent);
	iRoot = iProvider->CreateNode(sparent, root.Name(), NULL, this);
	//parent->AppendChild(iRoot);
	if (iRoot != NULL) {
	    iRoot->SetMutation(root);
	    iRoot->Mutate();
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

void *Env::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    return (strcmp(aName, Type()) == 0) ? this : NULL;
}


