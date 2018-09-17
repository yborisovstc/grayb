
#include "profiler_events.h"


/** Profile events register */
/*
const Pind::TEventsIdata KPindClock_Events = {
    {Env_Start_Constr, {"Env start constr"}},
    {Env_Root_Created, {"Env Root Created"}},
    {Env_Root_Created_From_Start, {"Env Root Created From Start"}},
    {Env_End_Constr, {"Env End Constr"}},
    {Elem_Constr_Start, {"Elem constructor start"}},
    {Elem_Constr_End, {"Elem constructor end"}},
    {Elem_Constr_Dur, {"Elem constructor duration"}}
};
*/

/** Clock indicator data */
const PindClock::Idata KPindClockIdata = {
    EPiid_Clock,
    {
	{Env_Start_Constr, {"Env start constr"}},
	{Env_Root_Created, {"Env Root Created"}},
	{Env_Root_Created_From_Start, {"Env Root Created From Start"}},
	{Env_End_Constr, {"Env End Constr"}},
	{Elem_Constr_Start, {"Elem constructor start"}},
	{Elem_Constr_End, {"Elem constructor end"}},
	{Elem_Constr_Dur, {"Elem constructor duration"}}
    }
};

const MProfiler::TIdata PInitData = {
    KPindClockIdata
};

;
