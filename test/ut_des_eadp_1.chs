testroot : Elem
{
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
            MutSuffix : AStatec;
            MutSuffix < Value = "SS  : Unit;";
            Conc1 : ATrcApndVar;
            Conc2 : ATrcApndVar;
            Mut : AStatec;
            Mut < Value = "SS ?";
            Mut < Debug.Update = y;
            ElemAdp/InpMut ~ Mut;
            Mut/Inp ~ Conc2;
            Ntos : ATrcNtosVar;
            Conc2/Inp ~ Conc1;
            Conc2/Inp2 ~ MutSuffix;
            Conc1/Inp ~ NewCompNamebase;
            Conc1/Inp2 ~ Ntos;
            Ntos/Inp ~ CompIdxCount;
        }
    }
}
