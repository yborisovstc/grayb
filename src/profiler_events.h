#ifndef __GRAYB_PROFILER_EVENTS_H
#define __GRAYB_PROFILER_EVENTS_H

#include "profiler.h"


const MProfiler::TIdata PInitData;

enum {
    Env_Start_Constr = 0,
    Env_Root_Created,
    Env_Root_Created_From_Start,
    Env_End_Constr,
    Elem_Constr_Start,
    Elem_Constr_End,
    Elem_Constr_Dur,
};

#endif


