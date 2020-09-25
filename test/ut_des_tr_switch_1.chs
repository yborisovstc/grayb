testroot : Elem
{
    About = "Unit test of mutation switcher. Using bool switch";
    Modules : AImports
    {
        $ # " Module containing components of des layer ";
        $ + /DesComps/Des;
        $ + /AdpComps/UnitAdp;
        $ + /AdpComps/MelemAdp;
    }
    test : ./Modules/DesComps/Des
    {
        About = "MElem adapter test: adding comp and observation of compoments number";
        Log.Creation = y;
        Target : Elem;
        Target < Cmp_0 : Unit;
        Target < Cmp_1 : Unit;
        Controller : /*/Modules/DesComps/Des
        {
            About = "Controller using adapter for access to target";
            UnitAdp : /*/Modules/AdpComps/UnitAdp
            {
                AdpAgent < AgentUri = ./../../../Target;
                CompUid < Debug.Update = y;
                CompsCount < Debug.Update = y;
            }
            ElemAdp : /*/Modules/AdpComps/MelemAdp
            {
                AdpAgent < AgentUri = ./../../../Target;
            }
            $ # "Comps count index incrementor";
            CompIdxCount : AStatec;
            CompIdxCount < Value = "SI 0";
            CompIdxCount < Debug.Update = y;
            Incr : ATrcAddVar;
            Const_1 : AStatec;
            Const_1 < Value = "SI 1";
            CompIdxCount/Inp ~ Incr;
            Incr/Inp ~ CompIdxCount;
            Incr/Inp ~ Const_1;
            UnitAdp/InpCompIdx ~ CompIdxCount;
            $ # Mutation;
            TCmp_Eq : ATrcCmpVar;
            TCmp_Eq/Inp ~ CompIdxCount;
            TCmp_Eq/Inp2 ~ Const_1;
            TSwitch : ATrcSwitchBool;
            ElemAdp/InpMut ~ TSwitch;
            NewCompName0 : AStatec;
            NewCompName0 < Value = "SS NewComp_0";
            NewCompName1 : AStatec;
            NewCompName1 < Value = "SS NewComp_1";
            MutParent : AStatec;
            MutParent < Value = "SS Unit";
            TMutNode0 : ATrcMutNode;
            TMutNode0/Name ~ NewCompName0;
            TMutNode0/Parent ~ MutParent;
            TSwitch/Inp1 ~ TMutNode0;
            TMutNode1 : ATrcMutNode;
            TMutNode1/Name ~ NewCompName1;
            TMutNode1/Parent ~ MutParent;
            TSwitch/Inp2 ~ TMutNode1;
            TSwitch/Sel ~ TCmp_Eq;
        }
    }
}
