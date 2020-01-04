testroot : Elem
{
  Modules : AImports
  {
    $ # " Module containing components of incaps layer ";
    $ + /IncapsComps/Incaps;
    $ # " Module containing components of data ";
    $ + /DataComps/DataLogevent;
    $ # " Module containing components of des layer ";
    $ + /DesComps/StateInt;
    $ + /DesComps/TrIncInt;
  }
  test : ./Modules/IncapsComps/Incaps
  {
    State1 : ./../Modules/DesComps/StateInt
    {
      Logspec : Elem
      {
        Creation : Elem;
      }
      Confirmed - Logspec : Elem
      {
        Update : /testroot/Modules/DataComps/DataLogevent;
      }
      Trans : ./../../Modules/DesComps/(TrInt:)TrIncInt;
      E1 : Edge
      {
        P1 - $ = ./../../(TrIncInt:)Trans/Inp;
        P2 - $ = ./../../Capsule/Inp/(StInpSocket:)Int/PinData;
      }
      E2 : Edge
      {
        P1 - $ = ./../../Trans/Out;
        P2 - $ = ./../../(DataSCInt:)Prepared/Capsule/Inp;
      }
    }
    $ # " Loop ";
    E3 : Edge
    {
      P1 - $ = ./../../State1/Capsule/Out;
      P2 - $ = ./../../State1/Capsule/Inp;
    }
  }
}
