testroot : Elem
{
  Modules : AImports
  {
    $ # " Module containing components of des layer ";
    $ + /DesComps/Des;
  }
  test : ./Modules/DesComps/Des
  {
    About = "Simple test that creates sole state DES Incrementor (var, multicontent) looped to youself";
    Log.Creation = y;
    State : AStatec;
    ./State - Value = "I 0";
    ./State - Debug.Update = y;
    Trans : ATrcAddVar;
    IncrData : AStatec;
    ./IncrData - Value = "I 1";
    IncrData2 : AStatec;
    ./IncrData2 - Value = "I 0";
    Data3 : AStatec;
    ./Data3 - Value = "SI 3";
    E2 : Aedge
    {
      $ = "{P1:'./../Trans/Out' P2:'./../State/Inp'}";
    }
    E3 : Aedge
    {
      $ = "{P1:'./../IncrData' P2:'./../Trans/Inp'}";
    }
    E5 : Aedge
    {
      $ = "{P1:'./../IncrData2' P2:'./../Trans/Inp'}";
    }
    $ # " Loop ";
    E4 : Aedge
    {
      $ = "{P1:'./../State' P2:'./../Trans/Inp'}";
    }
  }
}
