#include "prov.h"


GProvider::GProvider(const string &aName): NBase(aName)
{
}

/*
void *GProvider::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    return (strcmp(aName, Type()) == 0) ? this : NULL;
}
*/

Elem* GProvider::CreateNode(const string& aType, const string& aName, Elem* aMan, MEnv* aEnv)
{
    return NULL;
}

Chromo* GProvider::CreateChromo()
{
    return NULL;
}
