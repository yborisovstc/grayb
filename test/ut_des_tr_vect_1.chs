testroot : Elem
{
    Modules : AImports
    {
        $ # " Module containing components of des layer ";
        $ + /DesComps/Des;
    }
    test : ./Modules/DesComps/Des
    {
        About = "Simple test of Vector state and transitions";
        Log.Creation = y;
        State : AStatec;
        State - Value = "VS Item_0 Item_1 Item_2 Item_3";
        State - Debug.Update = y;
        Result : AStatec;
        Result - Value = "SS Nil";
        Result - Debug.Update = y;
        Idx : AStatec;
        Idx - Value = "SI 0";
        Idx - Debug.Update = y;
        Const_0 : AStatec;
        Const_0 - Value = "SI 1";
        Const_1 : AStatec;
        Const_1 - Value = "SI 1";
        Incr : ATrcAddVar;
        Incr/Inp ~ Const_1;
        Incr/Inp ~ Idx;
        At : ATrcAtVar;
        Result/Inp ~ At;
        At/Inp ~ State;
        At/Idx ~ Idx;
        Size : AStatec;
        Size - Value = "SI 1";
        Size - Debug.Update = y;
        GetSize : ATrcSizeVar;
        Size/Inp ~ GetSize;
        GetSize/Inp ~ State;
        Size_min_1 : ATrcAddVar;
        Size_min_1/Inp ~ GetSize;
        Size_min_1/InpN ~ Const_1;
        Min : ATrcMinVar;
        Min/Inp ~ Size_min_1;
        Min/Inp ~ Incr;
        Idx/Inp ~ Min;
    }
}
