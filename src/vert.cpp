#include "vert.h"

Vert::Vert(const string& aName): Base(aName)
{
}

void *Vert::DoGetObj(const char *aName)
{
    return (strcmp(aName, Type()) == 0) ? this : NULL;
}
