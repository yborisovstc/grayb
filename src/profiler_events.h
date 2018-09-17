#ifndef __GRAYB_PROFILER_EVENTS_H
#define __GRAYB_PROFILER_EVENTS_H

#include "profiler.h"


namespace PEvents {

enum {
    Env_Start_Constr = 0,
    Env_Root_Created,
    Env_End_Constr,
    Elem_Constr_Start,
    Elem_Constr_End,
    Elem_Constr_Dur,
};

}

extern const GProfiler::TIdata KPInitData;

#endif


