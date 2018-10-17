#ifndef __GRAYB_PROFILER_EVENTS_H
#define __GRAYB_PROFILER_EVENTS_H

#include "profiler.h"


namespace PEvents {

enum {
    Env_Start_Constr = 0,
    Env_Root_Created,
    Env_End_Constr,
    Dur_Elem_Constr_Start,
    Dur_Elem_Constr,
    Dur_Env_Constr_Start,
    Dur_Env_Constr,
    Dur_Profiler_Dur_Start,
    Dur_Profiler_Dur,

    DurStat_Elem_Constr,
    DurStat_Elem_Constr_Chromo,
    DurStat_OnCompAdd,
    DurStat_OnCompChanged,
    DurStat_GetNode,
    DurStat_GetIfi,
    DurStat_NotifNodeMutated,
    DurStat_CreateHeir,
    DurStat_CreateNativeAgt,
    DurStat_MutCont,
    DurStat_MutChange,
    DurStat_TransfOsm,
    DurStat_MutImportToNode,
    DurStat_DoImport
};

}

extern const GProfiler::TIdata KPInitData;

#endif


