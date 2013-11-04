
#include "base.h"
#include "env.h"
#include "log.h"
#include "factr.h"
#include "elem.h"
#include "chromo.h"
#include <stdlib.h>

const string KLogFileName = "faplog.txt";
const char* KRootName = "Root";

Env::Env(const string& aName, const string& aSpecFile, const string& aLogFileName): Base(aName), iRoot(NULL), iLogger(NULL)
{
    iLogger = new GLogRec("Logger", aLogFileName.empty() ? KLogFileName : aLogFileName);
    iProvider = new GFactory("Factory");
    iProvider->LoadPlugins();
    iSystSpec = aSpecFile;
    srand(time(NULL));
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
	iRoot = iProvider->CreateNode(sparent, root.Name(), NULL, this);
	if (iRoot != NULL) {
	    iRoot->SetMutation(root);
	    iRoot->Mutate();
	}
	else {
	    Logger()->WriteFormat("Env: cannot create elem [%s] of type [%s]", root.Name().c_str(), sparent.c_str());
	}
    }
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
}

void *Env::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    return (strcmp(aName, Type()) == 0) ? this : NULL;
}


