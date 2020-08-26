testroot : Elem
{
    About = "Variant using DMut data";
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
            NewCompNamebase : AStatec;
            NewCompNamebase < Value = "SS NewComp_";
            MutParent : AStatec;
            MutParent < Value = "SS Unit";
            TConc : ATrcApndVar;
            TMutNode : ATrcMutNode;
            ElemAdp/InpMut ~ TMutNode;
            TMutNode/Name ~ TConc;
            TMutNode/Parent ~ MutParent;
            Ntos : ATrcNtosVar;
            TConc/Inp ~ NewCompNamebase;
            TConc/Inp2 ~ Ntos;
            Ntos/Inp ~ CompIdxCount;
        }
    }
}
