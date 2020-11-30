testroot : Elem
{
    Modules : AImports
    {
        $ # " Module containing components of des layer ";
        $ + /DesComps/Des;
        $ + /AdpComps/UnitAdp;
    }
    test : ./Modules/DesComps/Des
    {
        About = "MUnit adapter test: observation of compoments number";
        Log.Creation = y;
        $ # Target_1;
        Target : Unit;
        Target < Cmp_0 : Unit;
        Target < Cmp_1 : Unit;
        $ # Target_2;
        Target2 : Unit;
        Target2 < Cmp2_0 : Unit;
        Target2 < Cmp2_1 : Unit;
        Target2 < Cmp2_2 : Unit;
        $ # Target_3;
        Target3 : Unit;
        Target3 < Cmp3_0 : Unit;
        Target3 < Cmp3_1 : Unit;
        Target3 < Cmp3_2 : Unit;
        Target3 < Cmp3_3 : Unit;
        Controller : /*/Modules/DesComps/Des
        {
            About = "Controller using adapter for access to target";
            Adapter : /*/Modules/AdpComps/UnitAdp
            {
                AdpAgent < AgentUri = ./../../../Target;
            }
            $ # Components_count;
            CompCount : AStatec;
            CompCount < Value = "SI 0";
            CompCount < Debug.Update = y;
            CompCount/Inp ~ Adapter/CompsCount;
            $ # Components;
            CompNames : AStatec;
            CompNames < Value = VS;
            CompNames < Debug.Update = y;
            CompNames/Inp ~ Adapter/CompNames;
            $ # Tics_Counter;
            Counter : AStatec;
            Counter < Value = "SI 0";
            Counter < Debug.Update = y;
            Incr : ATrcAddVar;
            Counter/Inp ~ Incr;
            Const_1 : AStatec;
            Const_1 < Value = "SI 1";
            Incr/Inp ~ Const_1;
            Incr/Inp ~ Counter;
            $ # "Managed agent 1 URI";
            MagUri : AStatec;
            MagUri < Value = "SS ./../../../Target2";
            MagUri < Debug.Update = y;
            $ # "Managed agent 2 URI";
            MagUri2 : AStatec;
            MagUri2 < Value = "SS ./../../../Target3";
            MagUri2 < Debug.Update = y;
            $ # "Target switcher";
            Const_3 : AStatec;
            Const_3 < Value = "SI 3";
            Cmp_Ge : ATrcCmpVar;
            Cmp_Ge/Inp ~ Incr;
            Cmp_Ge/Inp2 ~ Const_3;
            Sw : ATrcSwitchBool;
            Sw/Sel ~ Cmp_Ge;
            Sw/Inp1 ~ MagUri;
            Sw/Inp2 ~ MagUri2;
            Adapter/InpMagUri ~ Sw;
        }
    }
}
