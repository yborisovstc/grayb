DataComps : Elem
{
  $ # " DES logging event ";
  DataLogevent : Elem;
  $ # " Data with separate input for setting and output for getting ";
  DataS : /*/Modules/IncapsComps/Incaps
  {
    D_1 : Description
    {
      $ = Base of Data with separate input for setting and output for getting;
    }
    ./Capsule - inp : /*/Modules/SysComps/ConnPointInp;
    ./Capsule - out : /*/Modules/SysComps/ConnPointOut
    {
      ./Required - $ = MDataObserver;
    }
    Value : Prop;
  }
  $ # " Data with separate input for setting and output for getting ";
  DataSMc : /*/Modules/IncapsComps/Incaps
  {
    About = Base of Data with separate input for setting and output for getting;
    ./Capsule - inp : /*/Modules/SysComps/ConnPointInpMc;
    ./Capsule - out : /*/Modules/SysComps/ConnPointOutMc
    {
      Required = MDataObserver;
    }
  }
  $ # " Data with separate input for setting and output for getting. Props for Type and Value ";
  DataST : ./DataS
  {
    ./D_1 - $ = Base of Data with separate input for setting and output for getting. Separate Type property.;
    Type : Prop;
  }
  $ # " Data with separate input for setting and output for getting. W/o notificaion ";
  DataSU : /*/Modules/IncapsComps/Incaps
  {
    D_1 : Description
    {
      $ = Data with separate input for setting and output for getting, w/o notification;
    }
    ./Capsule - inp : /*/Modules/SysComps/ConnPointInp;
    ./Capsule - out : /*/Modules/SysComps/ConnPointOut;
    Value : Prop;
  }
  $ # " Data with separate input for setting and output for getting. W/o notificaion ";
  DataSUMc : /*/Modules/IncapsComps/Incaps
  {
    About = Data with separate input for setting and output for getting, w/o notification;
    ./Capsule - inp : /*/Modules/SysComps/ConnPointInpMc;
    ./Capsule - out : /*/Modules/SysComps/ConnPointOutMc;
    Value = ;
  }
  $ # " Data with separate input for setting and output for getting. Integer ";
  DataSInt : ./DataS
  {
    ./D_1 - $ = Integer Data with separate input for setting and output for getting;
    ADInt : DInt;
    ./Capsule/inp/Provided - $ = MDIntSet;
    ./Capsule/out/Provided - $ = MDIntGet;
  }
  $ # " Data with separate input for setting and output for getting. Variant ";
  DataSVar : ./DataS
  {
    ./D_1 - $ = Variant Data with separate input for setting and output for getting;
    ADVar : DVar;
    ./Capsule/inp/Provided - $ = MDVarSet;
    ./Capsule/out/Provided - $ = MDVarGet;
  }
  $ # " Data with separate input for setting and output for getting. Variant ";
  DataSVarMc : ./DataSMc
  {
    About = Variant Data with separate input for setting and output for getting;
    ADVar : DVar;
    ./Capsule/inp - Provided = MDVarSet;
    ./Capsule/out - Provided = MDVarGet;
    ViewData.Res = ./ADVar;
  }
  $ # " Data with separate input for setting and output for getting. Variant. Separate type prop ";
  DataSTVar : ./DataST
  {
    ./D_1 - $ = Variant Data with separate input for setting and output for getting. Separate Type property.;
    ADVar : DVar;
    ./Capsule/inp/Provided - $ = MDVarSet;
    ./Capsule/out/Provided - $ = MDVarGet;
  }
  DataSIInt : ./DataSInt
  {
    ./D_1 - $ = Initital data - Integer Data without input.;
    ./Capsule/inp ! ;
  }
  $ # " Observer iface is declared for purpose of compatibility but isn't supported ";
  DataSNInt : ./DataS
  {
    AData : DNInt;
    ./Capsule/inp/Provided - $ = MDIntSet;
    ./Capsule/out/Provided - $ = MDIntGet;
  }
  DataSUInt : ./DataSU
  {
    ADInt : DInt;
    ./Capsule/inp/Provided - $ = MDIntSet;
    ./Capsule/out/Provided - $ = MDIntGet;
  }
  DataSUVar : ./DataSU
  {
    ADVar : DVar;
    ./Capsule/inp/Provided - $ = MDVarSet;
    ./Capsule/out/Provided - $ = MDVarGet;
  }
  DataSUVarMc : ./DataSUMc
  {
    About = Variable data with separate input for setting and output for getting, w/o notification;
    ADVar : DVar;
    ./Capsule/inp - Provided = MDVarSet;
    ./Capsule/out - Provided = MDVarGet;
    ViewData. = ./ADVar;
  }
  $ # " Terminal data, input only, provided data observer, required gettng data ";
  DataTInt : ./DataS
  {
    ./D_1 - $ = Terminal data, input only. Monitoring input and update itself;
    ADInt : DInt;
    ./Capsule/inp/Provided - $ = MDataObserver;
    ./Capsule/inp/Required - $ = MDIntGet;
    ./Capsule/out ! ;
  }
  $ # " Terminal data, input only, provided data observer, required gettng data ";
  DataTVar : ./DataS
  {
    ./D_1 - $ = Terminal data, input only, variable type. Monitoring input and update itself;
    AData : DVar;
    ./Capsule/inp/Provided - $ = MDataObserver;
    ./Capsule/inp/Required - $ = MDVarGet;
    ./Capsule/out/Provided - $ = MDVarGet;
  }
  $ # " Terminal data, input only, provided data observer, required gettng data ";
  DataTVarMc : ./DataSMc
  {
    About = Terminal data, input only, variable type. Monitoring input and update itself;
    AData : DVar;
    ./Capsule/inp - Provided = MDataObserver;
    ./Capsule/inp - Required = MDVarGet;
    ./Capsule/out - Provided = MDVarGet;
    ViewData.Res = ./AData;
  }
  $ # " Terminal data, input only, provided data observer, required gettng data. Separate type prop ";
  DataTTVar : ./DataST
  {
    ./D_1 - $ = Terminal data, input only, variable type. Monitoring input and update itself. Separate type property.;
    AData : DVar;
    ./Capsule/inp/Provided - $ = MDataObserver;
    ./Capsule/inp/Required - $ = MDVarGet;
    ./Capsule/out/Provided - $ = MDVarGet;
  }
  DataInt : /*/Modules/IncapsComps/Incaps
  {
    ADInt : DInt;
    ./Capsule - out : /*/Modules/SysComps/ConnPointOut
    {
      ./Provided - $ = MDInt;
    }
  }
  $ # " START: Data with separate input, output, and dedicated input for initiating of setting data";
  $ # " DES logging event Update ";
  Update : ./DataLogevent;
  $ # " Data base with separate conn point for update ";
  DataSC : /*/Modules/IncapsComps/Incaps
  {
    ./Capsule - Inp : /*/Modules/SysComps/ConnPointInp;
    ./Capsule - Out : /*/Modules/SysComps/ConnPointOut;
    ./Capsule - Upd : /*/Modules/SysComps/ConnPoint;
    Value : Prop;
  }
  $ # " Data base with separate conn point for update, multicontent ";
  DataSCMc : /*/Modules/IncapsComps/Incaps
  {
    About = Data base with separate conn point for update;
    ./Capsule - Inp : /*/Modules/SysComps/ConnPointInpMc;
    ./Capsule - Out : /*/Modules/SysComps/ConnPointOutMc;
    ./Capsule - Upd : ConnPointMc;
    Value = ;
  }
  $ # " Data base with separate conn point for update and separate type ";
  DataSCT : ./DataSC
  {
    Type : Prop;
  }
  $ # " Data Int ";
  DataSCInt : ./DataSC
  {
    ADInt : DInt;
    ./Capsule/Inp/Required - $ = MDIntGet;
    ./Capsule/Out/Provided - $ = MDIntGet;
    ./Capsule/Upd/Provided - $ = MUpdatable;
  }
  $ # " Data Var ";
  DataSCVar : ./DataSC
  {
    ADVar : DVar;
    ./Capsule/Inp/Required - $ = MDVarGet;
    ./Capsule/Out/Provided - $ = MDVarGet;
    ./Capsule/Upd/Provided - $ = MUpdatable;
  }
  $ # " Data Var, multicontent ";
  DataSCVarMc : ./DataSCMc
  {
    About = Data with separate conn point for update;
    ADVar : DVar;
    ./Capsule/Inp - Required = MDVarGet;
    ./Capsule/Out - Provided = MDVarGet;
    ./Capsule/Upd - Provided = MUpdatable;
    ViewData. = ./ADVar;
  }
  $ # " Data Var with separate type ";
  DataSCTVar : ./DataSCT
  {
    ADVar : DVar;
    ./Capsule/Inp/Required - $ = MDVarGet;
    ./Capsule/Out/Provided - $ = MDVarGet;
    ./Capsule/Upd/Provided - $ = MUpdatable;
  }
  $ # " END: Data with separate input, output, and dedicated input for initiating of setting data";
}
