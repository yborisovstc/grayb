#include "prov.h"


GProvider::GProvider(const string &aName): Base(aName)
{
}

void *GProvider::DoGetObj(const char *aName)
{
    return (strcmp(aName, Type()) == 0) ? this : NULL;
}

Vert* GProvider::CreateNode(const string& aType)
{
    return NULL;
}

Chromo* GProvider::CreateChromo()
{
    return NULL;
}
