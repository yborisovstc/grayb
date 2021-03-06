
#include "profiler_events.h"

using namespace PEvents;

/** Profile events register */
const PindClock::TEvents KPindClock_Events = {
    {Env_Start_Constr, "Env start constr"},
    {Env_Root_Created, "Env Root Created"},
    {Env_End_Constr, "Env End Constr"},
};

/** Clock indicator data */
const PindClock::Idata KPindClockIdata = {
    "Clock indicator",
    1024,
    &KPindClock_Events
};

/** Init data for duration indicator */
const PindDur::Idata KPindDurIdata = {
    "Duration indicator",
    1024,
    {
	{Dur_Env_Constr_Start, "Env constructor start", PEvent::KUndefEid},
	{Dur_Env_Constr, "Env constructor duration", Dur_Env_Constr_Start},
	{Dur_Elem_Constr_Start, "Elem constructor start", PEvent::KUndefEid},
	{Dur_Elem_Constr, "Elem constructor duration", Dur_Elem_Constr_Start},
	{Dur_Profiler_Dur_Start, "Profiler duration measure time. Start", PEvent::KUndefEid},
	{Dur_Profiler_Dur, "Profiler duration measure time", Dur_Profiler_Dur_Start}
    }
};


/** Init data for duration statistical indicator */
const PindDurStat::Idata KPindDurStatIdata = {
    "Duration statistic indicator",
    {
	{DurStat_Elem_Constr, "Elem constructor duration"},
	{DurStat_Elem_Constr_Chromo, "Elem constructor: chromo creation duration"},
	{DurStat_OnCompAdd, "Elem OnCompAdding handling"},
	{DurStat_OnCompChanged, "Elem OnCompChanged handling"},
	{DurStat_GetNode, "Elem GetNode"},
	{DurStat_GetIfi, "Elem GetIfi"},
	{DurStat_NotifNodeMutated, "Elem NotifyNodeMutated"},
	{DurStat_CreateHeir, "Elem CreateHeir"},
	{DurStat_CreateNativeAgt, "Creating native agent"},
	{DurStat_MutCont, "Content changing mutation"},
	{DurStat_MutChange, "Attr changing mutation"},
	{DurStat_TransfOsm, "Transform mut to OSM"},
	{DurStat_MutImportToNode, "Import mgr ImportToNode, excl Mutate"},
	{DurStat_DoImport, "Import mgr DoImport, excl ImportToNode"},
    }
};


/** Init data for profiler */
const GProfiler::TIdata KPInitData = {
    { EPiid_Clock, &KPindClockIdata },
    { EPiid_Dur, &KPindDurIdata },
    { EPiid_DurStat, &KPindDurStatIdata }
};
