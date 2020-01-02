Root : Elem
{
  Modules : Elem
  {
    $ # " Module containing components of incaps layer ";
    $ + /IncapsComps/Incaps;
    $ # " Module containing components of data ";
    $ + /DataComps/DataTVar;
    $ + /DataComps/DataSVar;
    $ # " Module containing components of func ";
    $ + /FuncComps/FAddVar;
    $ + /FuncComps/FuncIntRes;
  }
  Start : Elem
  {
    Incaps_root : /Root/Modules/IncapsComps/Incaps
    {
      Dt : /Root/Modules/DataComps/DataTVar
      {
        ./Value - $ = "F 0";
      }
      Add : /Root/Modules/FuncComps/FAddVar;
      Inp_data1 : /Root/Modules/DataComps/DataSVar
      {
        ./Value - $ = "I 4";
      }
      e1 : Edge
      {
        ./P2 - $ = /Root/Start/Incaps_root/Dt/Capsule/inp;
        ./P1 - $ = /Root/Start/Incaps_root/Add/Capsule/out;
      }
      e2 : Edge
      {
        ./P2 - $ = /Root/Start/Incaps_root/Add/Capsule/Inp;
        $ # " <cont node="./P1" ref="/Root/Start/Incaps_root/Inp_data1/Capsule/out"/> ";
      }
    }
  }
}
