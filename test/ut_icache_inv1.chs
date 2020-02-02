Root : Elem
{
    Modules : Elem
    {
        $ + /IncapsComps/Incaps;
        $ + /DataComps/DataSUInt;
        $ + /DataComps/DataSUVar;
        $ + /FuncComps/TFAddVar;
        $ + /DesComps/Des;
        $ + /DesComps/StateVar;
    }
    IncapsRoot : /Root/Modules/IncapsComps/Incaps
    {
        DesRoot : /Root/Modules/DesComps/Des
        {
            st : /Root/Modules/DesComps/StateVar
            {
                Add : /Root/Modules/FuncComps/TFAddVar;
                /Root/IncapsRoot/DesRoot/st/Capsule/Inp/Int/PinData ~ /Root/IncapsRoot/DesRoot/st/Add/Capsule/Inp;
                /Root/IncapsRoot/DesRoot/st/Add/Capsule/Out ~ /Root/IncapsRoot/DesRoot/st/(DataSCVar:)Prepared/Capsule/Inp;
                Const1 : /Root/Modules/DataComps/DataSUVar
                {
                    ./Value - $ = "F 0.1";
                }
                /Root/IncapsRoot/DesRoot/st/Const1/Capsule/out ~ /Root/IncapsRoot/DesRoot/st/Add/Capsule/Inp;
                ./(DataSCVar:)Confirmed/Value - $ = "F 0";
                ./(DataSCVar:)Prepared/Value - $ = "F 0";
            }
            E_back : Edge
            {
                ./P2 - $ = /Root/IncapsRoot/DesRoot/st/Capsule/Out;
            }
            $ # " <cont node=\"./E_back/P1\" ref=\"/Root/IncapsRoot/DesRoot/st/Capsule/Inp\"/> ";
        }
    }
}
