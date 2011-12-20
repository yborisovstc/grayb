
#include "base.h"
#include "env.h"
#include "log.h"
#include "factr.h"
#include "graph.h"
#include "chromo.h"

const string KLogFileName = "faplog.txt";
const char* KRootName = "Root";

Env::Env(const string& aName, const string& aSpecFile, const string& aLogFileName): Base(aName), iRoot(NULL), iLogger(NULL)
{
    iLogger = new GLogRec("Logger", aLogFileName.empty() ? KLogFileName : aLogFileName);
    iProvider = new GFactory("Factory");
    iProvider->LoadPlugins();
    iSystSpec = aSpecFile;
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
	iRoot = new Graph(KRootName);
    }
    else {
	spec->Set(iSystSpec.c_str());
	const ChromoNode& root = spec->Root();
	iRoot = new Graph(root.Name());
	iRoot->SetMutation(root);
	iRoot->Mutate();
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

void Env::AddProvider(GProvider* aProv)
{
    iProvider->AddProvider(aProv);
}

void *Env::DoGetObj(const char *aName)
{
    return (strcmp(aName, Type()) == 0) ? this : NULL;
}

