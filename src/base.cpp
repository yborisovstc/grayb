#include <stdlib.h>
#include <stdio.h>
#include <stdexcept> 
#include "base.h"

const char* KFapPanic = "FAP: Error %d";

void Panic(TInt aRes)
{
    _IND_PANIC(KFapPanic, aRes);
}



