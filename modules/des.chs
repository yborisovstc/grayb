DesComps : Elem
{
  $ # " DES logging event ";
  DesLogevent : Elem;
  $ # " Output socket ";
  StOutSocket : /*/Modules/SysComps/SocketInp
  {
    $ # " Pin Des observer. Requires DES observer interface to handle activation of the state ";
    PinObs : /*/Modules/SysComps/ConnPoint
    {
      ./Provided - $ = MDesInpObserver;
    }
    $ # " Pin Data observer. Provides data interface ";
    PinData : /*/Modules/SysComps/ConnPoint;
  }
  $ # " State Output socket, multicontent ";
  StOutSocketMc : /*/Modules/SysComps/SocketMc
  {
    About = "State Output socket, multicontent";
    PinObs : ConnPointMc
    {
      About = "Pin Des observer. Requires DES observer interface to handle activation of the state";
      Provided = MDesInpObserver;
    }
    PinData : ConnPointMc
    {
      About = "Pin Data observer. Provides data interface";
    }
  }
  $ # " State Output socket, multicontent, monolitic ";
  StOutSocketMcm : ASocketMcm
  {
    About = "State Output socket, multicontent, monolitic";
    Direction = Out;
    PinObs : ConnPointMc
    {
      About = "Pin Des observer. Requires DES observer interface to handle activation of the state";
      Provided = MDesInpObserver;
    }
    PinData : ConnPointMc
    {
      About = "Pin Data observer. Provides data interface";
    }
  }
  $ # " State Output socket, multicontent, monolitic, unit pins ";
  StOutSocketMcmu : ASocketMcm
  {
    About = "State Output socket, multicontent, monolitic";
    Direction = Out;
    PinObs : ConnPointMc;
    ./PinObs - About = "Pin Des observer. Requires DES observer interface to handle activation of the state";
    ./PinObs - Provided = MDesInpObserver;
    PinData : ConnPointMcu;
    ./PinData - About = "Pin Data observer. Provides data interface";
  }
  StOutSocketMcVar : ./StOutSocketMc
  {
    About = "State Output socket for variable data, multicontent";
    ./PinData - Required = MDVarGet;
  }
  $ # " Input socket ";
  StInpSocket : /*/Modules/SysComps/SocketOut
  {
    $ # " Pin Des observer. Requires DES observer interface to handle activation of the state ";
    PinObs : /*/Modules/SysComps/ConnPoint
    {
      ./Required - $ = MDesInpObserver;
    }
    $ # " Pin Data observer. Provides data interface ";
    PinData : /*/Modules/SysComps/ConnPoint;
  }
  $ # " State Input socket, multicontent ";
  StInpSocketMc : /*/Modules/SysComps/SocketMc
  {
    About = "State Input socket, multicontent";
    PinObs : ConnPointMc
    {
      About = "Pin Des observer. Requires DES observer interface to handle activation of the state";
      Required = MDesInpObserver;
    }
    PinData : ConnPointMc
    {
      About = "Pin Data observer. Provides data interface";
    }
  }
  $ # " State Input socket, multicontent, monolitic ";
  StInpSocketMcm : ASocketMcm
  {
    About = "State Input socket, multicontent, monolitic";
    Direction = Inp;
    PinObs : ConnPointMc
    {
      About = "Pin Des observer. Requires DES observer interface to handle activation of the state";
      Required = MDesInpObserver;
    }
    PinData : ConnPointMc
    {
      About = "Pin Data observer. Provides data interface";
    }
  }
  $ # " State Input socket, multicontent, monolitic, unit pins ";
  StInpSocketMcmu : ASocketMcm
  {
    About = "State Input socket, multicontent, monolitic, unit pins";
    Direction = Inp;
    PinObs : ConnPointMcu;
    ./PinObs - About = "Pin Des observer. Requires DES observer interface to handle activation of the state";
    ./PinObs - Required = MDesInpObserver;
    PinData : ConnPointMcu;
    ./PinData - About = "Pin Data observer. Provides data interface";
  }
  StInpSocketMcVar : ./StInpSocketMc
  {
    About = "State Input socket for variable data, multicontent";
    ./PinData - Provided = MDVarGet;
  }
  $ # " Input of standard Int State ";
  StIntInp : /*/Modules/SysComps/ExtenderInpBase
  {
    Int : ./../StInpSocket
    {
      ./PinData/Provided - $ = MDIntGet;
    }
  }
  $ # " Output of standard Int State ";
  StIntOut : /*/Modules/SysComps/ExtenderOutBase
  {
    Int : ./../StOutSocket
    {
      ./PinData/Required - $ = MDIntGet;
    }
  }
  $ # " Input of standard variable State ";
  StVarInp : Vert
  {
    MAExteder : ExtenderAgentInp;
    Int : ./../StInpSocket
    {
      ./PinData/Provided - $ = MDVarGet;
    }
  }
  StVarInpMc : Vert
  {
    MAExteder : AExtender;
    Int : ./../StInpSocketMc
    {
      ./PinData - Provided = MDVarGet;
    }
  }
  $ # " Output of standard variable State ";
  StVarOut : Vert
  {
    MAExteder : ExtenderAgentOut;
    Int : ./../StOutSocket
    {
      ./PinData/Required - $ = MDVarGet;
    }
  }
  StVarOutMc : Vert
  {
    MAExteder : AExtender;
    Direction = Out;
    Int : ./../StOutSocketMc
    {
      ./PinData - Required = MDVarGet;
    }
  }
  ExtStInpSocketMc : /*/Modules/SysComps/ExtenderInpMc
  {
    Int : ./../StInpSocketMc;
    ./Int < Direction = Out;
  }
  ExtStOutSocketMc : /*/Modules/SysComps/ExtenderOutMc
  {
    Int : ./../StOutSocketMc;
  }
  ExtStOutSocketMcVar : /*/Modules/SysComps/ExtenderOutMc
  {
    About = "Extender of state output socket, data variable, multicontent";
    Int : ./../StOutSocketMcVar;
    ./Int < Direction = Inp;
  }
  ExtStInpSocketMcVar : ./ExtStInpSocketMc
  {
    About = "Extender of state input socket, data variable, multicontent";
    ./Int/PinData < Provided = MDVarGet;
  }
  $ # " Input of int transition ";
  TrIntInp : /*/Modules/SysComps/ConnPointInp
  {
    ./Required - $ = MDIntGet;
  }
  $ # " Input of int transition ";
  TrVarInp : /*/Modules/SysComps/ConnPointInp
  {
    ./Required - $ = MDVarGet;
  }
  $ # " Transition of integer data ";
  TrInt : Syst
  {
    Inp : /*/Modules/SysComps/ConnPointInp
    {
      ./Required - $ = MDIntGet;
    }
    Out : /*/Modules/SysComps/ConnPointOut
    {
      ./Provided - $ = MDIntGet;
    }
  }
  $ # " Transition of integer data : Increment ";
  TrIncInt : ./TrInt
  {
    TrAgt : ATrIncInt;
  }
  $ # " Transition of integer data : Multiplication ";
  TrMplInt : ./TrInt
  {
    TrAgt : ATrMplInt;
  }
  $ # " Transition of integer data : Division ";
  TrDivInt : ./TrInt
  {
    TrAgt : ATrDivInt;
    Inp_DR : ./../TrIntInp;
  }
  $ # " Transition of integer data : Subtraction ";
  TrSubInt : ./TrInt
  {
    TrAgt : ATrSubInt;
    Inp_Sub : ./../TrIntInp;
  }
  $ # " Transition of variable data ";
  TrVar : Syst
  {
    Inp : /*/Modules/SysComps/ConnPointInp
    {
      ./Required - $ = MDVarGet;
    }
    Out : /*/Modules/SysComps/ConnPointOut
    {
      ./Provided - $ = MDVarGet;
    }
    ViewData : Vert
    {
      Res : Prop;
    }
  }
  $ # " Transition of variable data : Multiplication ";
  TrMplVar : ./TrVar
  {
    TrAgt : ATrMplVar;
    ./ViewData/Res - $ = ./../../TrAgt;
  }
  $ # " Transition of variable data : Division ";
  TrDivVar : ./TrVar
  {
    TrAgt : ATrDivVar;
    Inp_Dvs : ./../TrVarInp;
    ./ViewData/Res - $ = ./../../TrAgt;
  }
  $ # " Transition of variable data : Composing vector ";
  TrCpsVectVar : ./TrVar
  {
    TrAgt : ATrCpsVectVar;
    ./Inp | Inp0;
    Inp1 : ./../TrVarInp;
    Inp2 : ./../TrVarInp;
    ./ViewData/Res - $ = ./../../TrAgt;
  }
  $ # " Transition of variable data : Switch ";
  TrSwitchVar : ./TrVar
  {
    TrAgt : ATrSwitchVar;
    ./Inp | Sel;
    Inp1 : ./../TrVarInp;
    Inp2 : ./../TrVarInp;
  }
  $ # " Transition of variable data : Getting coord of vector ";
  TrAtVar : ./TrVar
  {
    TrAgt : ATrAtVar;
    Index : ./../TrIntInp;
  }
  $ # " Transition of variable data : Boolean comparition ";
  TrLTVar : ./TrVar
  {
    AF_Lt : ATrBcmpVar;
    ./Inp | Inp1;
    Inp2 : ./../TrVarInp;
    ./ViewData/Res - $ = ./../../AF_Lt;
  }
  $ # " Transition of variable data : Boolean comparition ";
  TrLEVar : ./TrVar
  {
    AF_Le : ATrBcmpVar;
    ./Inp | Inp1;
    Inp2 : ./../TrVarInp;
    ./ViewData/Res - $ = ./../../AF_Le;
  }
  $ # " Transition of variable data : Boolean comparition ";
  TrEQVar : ./TrVar
  {
    AF_Eq : ATrBcmpVar;
    ./Inp | Inp1;
    Inp2 : ./../TrVarInp;
    ./ViewData/Res - $ = ./../../AF_Eq;
  }
  $ # " Transition of variable data : Boolean comparition ";
  TrGTVar : ./TrVar
  {
    AF_Gt : ATrBcmpVar;
    ./Inp | Inp1;
    Inp2 : ./../TrVarInp;
    ./ViewData/Res - $ = ./../../AF_Gt;
  }
  $ # " Transition of variable data : Boolean comparition ";
  TrGEVar : ./TrVar
  {
    AF_Ge : ATrBcmpVar;
    ./Inp | Inp1;
    Inp2 : ./../TrVarInp;
    ./ViewData/Res - $ = ./../../AF_Ge;
  }
  $ # " Transition supported combined chain approach, ref ds_mae_scc ";
  $ # " Transition of variable data ";
  TrcVar : Syst
  {
    Inp : ConnPointMcu;
    ./Inp - $ = "{Required:'MDVarGet' Provided:'MDesInpObserver'}";
    Out : ConnPointMcu;
    ./Out - $ = "{Required:'MDesInpObserver' Provided:'MDVarGet'}";
  }
  $ # " Transition of variable data : Addition ";
  TrcAddVar : ./TrcVar
  {
    TrAgt : ATrcAddVar;
    ./ViewData/Res - $ = ./../../TrAgt;
  }
  $ # " END. Transition supported combined chain approach, ref ds_mae_scc ";
  $ # " State base ";
  State : /*/Modules/IncapsComps/Incaps
  {
    StateAgt : StateAgent;
    $ # " TODO [YB] Do we need dedicated CP or component has to notify it's mgr? ";
    ./Capsule - Sync : /*/Modules/SysComps/ConnPoint
    {
      ./Provided - $ = MDesSyncable;
    }
    $ # " Input ";
    ./Capsule - Inp : /*/Modules/SysComps/ExtenderInpBase
    {
      Int : ./../../../StInpSocket;
    }
    $ # " Output ";
    ./Capsule - Out : /*/Modules/SysComps/ExtenderOutBase
    {
      Int : ./../../../StOutSocket;
    }
  }
  $ # " State base, multicontent ";
  StateMc : /*/Modules/IncapsComps/Incaps
  {
    About = "DES state multicontent base";
    StateAgt : StateAgent;
    ./Capsule - Sync : ConnPointMc
    {
      Provided = MDesSyncable;
    }
    ./Capsule - Inp : /*/Modules/SysComps/ExtenderInpMc
    {
      Int : ./../../../StInpSocketMc;
    }
    ./Capsule - Out : /*/Modules/SysComps/ExtenderOutMc
    {
      Int : ./../../../StOutSocketMc;
    }
  }
  $ # " State base, multicontent, using monolitic extenders ";
  StateMcm : /*/Modules/IncapsComps/Incaps
  {
    About = "DES state multicontent base";
    StateAgt : StateAgent;
    ./Capsule - Sync : ConnPointMc
    {
      Provided = MDesSyncable;
    }
    ./Capsule - Inp : AExtdInp
    {
      Int : ./../../../StInpSocketMcm;
    }
    ./Capsule - Out : AExtdOut
    {
      Int : ./../../../StOutSocketMcm;
    }
  }
  $ # " State base, multicontent, using monolitic extenders and unit pins in sock ";
  StateMcmu : /*/Modules/IncapsComps/Incaps
  {
    About = "DES state base, multicontent, monolitic, unit pins conns ";
    StateAgt : StateAgent;
    ./Capsule - Sync : ConnPointMcu;
    ./Capsule/Sync - Provided = MDesSyncable;
    ./Capsule - Inp : AExtduInp;
    ./Capsule/Inp - Int : ./../StInpSocketMcmu;
    ./Capsule - Out : AExtduOut;
    ./Capsule/Out - Int : ./../StOutSocketMcmu;
  }
  StateInt : ./State
  {
    Confirmed : /*/Modules/DataComps/DataSCInt;
    Prepared : /*/Modules/DataComps/DataSCInt;
    ./Capsule/Inp/Int/PinData/Provided - $ = MDIntGet;
    ./Capsule/Out/Int/PinData/Required - $ = MDIntGet;
    E_Conf_Out : Edge
    {
      ./P1 - $ = ./../../Confirmed/Capsule/Out;
      ./P2 - $ = ./../../Capsule/Out/Int/PinData;
    }
    E_PrepToConf : Edge
    {
      ./P1 - $ = ./../../Prepared/Capsule/Out;
      ./P2 - $ = ./../../Confirmed/Capsule/Inp;
    }
  }
  StateVar : ./State
  {
    Confirmed : /*/Modules/DataComps/DataSCVar;
    Prepared : /*/Modules/DataComps/DataSCVar;
    ./Capsule/Inp/Int/PinData/Provided - $ = MDVarGet;
    ./Capsule/Out/Int/PinData/Required - $ = MDVarGet;
    E_Conf_Out : Edge
    {
      ./P1 - $ = ./../../Confirmed/Capsule/Out;
      ./P2 - $ = ./../../Capsule/Out/Int/PinData;
    }
    E_PrepToConf : Edge
    {
      ./P1 - $ = ./../../Prepared/Capsule/Out;
      ./P2 - $ = ./../../Confirmed/Capsule/Inp;
    }
  }
  $ # " State with variable type data, multicontent ";
  StateVarMc : ./StateMc
  {
    About = "DES state with variable type data, multicontent";
    Confirmed : /*/Modules/DataComps/DataSCVarMc;
    Prepared : /*/Modules/DataComps/DataSCVarMc;
    ./Capsule/Inp/Int/PinData - Provided = MDVarGet;
    ./Capsule/Out/Int/PinData - Required = MDVarGet;
    E_Conf_Out : Aedge
    {
      P1 = ./../Confirmed/Capsule/Out;
      P2 = ./../Capsule/Out/Int/PinData;
    }
    E_PrepToConf : Aedge
    {
      P1 = ./../Prepared/Capsule/Out;
      P2 = ./../Confirmed/Capsule/Inp;
    }
  }
  $ # " State with variable type data, multicontent, monolitic extenders ";
  StateVarMcm : ./StateMcm
  {
    About = "DES state with variable type data, multicontent";
    Confirmed : /*/Modules/DataComps/DataSCVarMc;
    Prepared : /*/Modules/DataComps/DataSCVarMc;
    ./Capsule/Inp/Int/PinData - Provided = MDVarGet;
    ./Capsule/Out/Int/PinData - Required = MDVarGet;
    E_Conf_Out : Aedge
    {
      P1 = ./../Confirmed/Capsule/Out;
      P2 = ./../Capsule/Out/Int/PinData;
    }
    E_PrepToConf : Aedge
    {
      P1 = ./../Prepared/Capsule/Out;
      P2 = ./../Confirmed/Capsule/Inp;
    }
  }
  $ # " State with variable type data, multicontent, monolitic extenders, unit pins ";
  StateVarMcmu : ./StateMcmu
  {
    About = "DES state with variable type data, multicontent, unit pins";
    Confirmed : /*/Modules/DataComps/DataSCVarMc;
    Prepared : /*/Modules/DataComps/DataSCVarMc;
    ./Capsule/Inp/Int/PinData - Provided = MDVarGet;
    ./Capsule/Out/Int/PinData - Required = MDVarGet;
    E_Conf_Out : Aedge
    {
      P1 = ./../Confirmed/Capsule/Out;
      P2 = ./../Capsule/Out/Int/PinData;
    }
    E_PrepToConf : Aedge
    {
      P1 = ./../Prepared/Capsule/Out;
      P2 = ./../Confirmed/Capsule/Inp;
    }
  }
  $ # " State with variable type data, multicontent, monolitic agent, monolitic extenders, unit pins ";
  StateVarMcmau : /*/Modules/IncapsComps/Incaps
  {
    About = "DES state with variable type data, multicontent, monolitic agent and CPs, unit pins";
    StateAgt : AState;
    ./Capsule - Sync : ConnPointMcu;
    ./Capsule/Sync - Provided = MDesSyncable;
    TransOut : ConnPointMcu;
    ./TransOut - Required = MDVarGet;
    ./Capsule - Inp : AExtduInp;
    ./Capsule/Inp - Int : ./../StInpSocketMcmu;
    ./Capsule - Out : AExtduOut;
    ./Capsule/Out - Int : ./../StOutSocketMcmu;
    ./Capsule/Inp/Int/PinData - Provided = MDVarGet;
    ./Capsule/Out/Int/PinData - Required = MDVarGet;
    E_Conf_Out : Aedge
    {
      P1 = ./../StateAgt;
      P2 = ./../Capsule/Out/Int/PinData;
    }
  }
  $ # " Improved state - combined data is used instead of prepared + confirmed
	 Note: to initiate the state you still need to initiate value of prepared.
	 This is required to define type of data because the current scheme doesn't
	 allow to implicitly get type of confirmed
	 ";
  StateVarC : ./State
  {
    Data : /*/Modules/IncapsComps/Incaps
    {
      Confirmed : /*/Modules/DataComps/DataSCVar;
      Prepared : /*/Modules/DataComps/DataSCVar;
      ./Capsule - Inp : /*/Modules/SysComps/ExtenderInp
      {
        ./Int/Provided - $ = MDVarGet;
      }
      ./Capsule - Out : /*/Modules/SysComps/ExtenderOut
      {
        ./Int/Required - $ = MDVarGet;
      }
      E_Inp_Prep : Edge
      {
        ./P1 - $ = ./../../Capsule/Inp/Int;
        ./P2 - $ = ./../../Prepared/Capsule/Inp;
      }
      E_Prep_Conf : Edge
      {
        ./P1 - $ = ./../../Prepared/Capsule/Out;
        ./P2 - $ = ./../../Confirmed/Capsule/Inp;
      }
      E_Conf_Out : Edge
      {
        ./P1 - $ = ./../../Confirmed/Capsule/Out;
        ./P2 - $ = ./../../Capsule/Out/Int;
      }
      ViewData : Vert
      {
        Val : Prop
        {
          $ = ./../../Confirmed/Value;
        }
      }
    }
    ./Capsule/Inp/Int/PinData/Provided - $ = MDVarGet;
    ./Capsule/Out/Int/PinData/Required - $ = MDVarGet;
    E_Data_Out : Edge
    {
      ./P1 - $ = ./../../Data/Capsule/Out;
      ./P2 - $ = ./../../Capsule/Out/Int/PinData;
    }
    ViewData : Vert
    {
      Val : Prop
      {
        $ = ./../../Data/Confirmed/Value;
      }
    }
  }
  $ # " Multicontent variant ";
  StateVarCMc : ./StateMc
  {
    About = "DES state with variable type data, combined data is used instead of prepared + confirmed, multicontent";
    Data : /*/Modules/IncapsComps/Incaps
    {
      Confirmed : /*/Modules/DataComps/DataSCVarMc;
      Prepared : /*/Modules/DataComps/DataSCVarMc;
      ./Capsule - Inp : /*/Modules/SysComps/ExtenderInpMc
      {
        Int : ConnPointMc;
        ./Int < Provided = MDVarGet;
      }
      ./Capsule - Out : /*/Modules/SysComps/ExtenderOutMc
      {
        Int : ConnPointMc;
        ./Int < Required = MDVarGet;
      }
      E_Inp_Prep : Aedge
      {
        P1 = ./../Capsule/Inp/Int;
        P2 = ./../Prepared/Capsule/Inp;
      }
      E_Prep_Conf : Aedge
      {
        P1 = ./../Prepared/Capsule/Out;
        P2 = ./../Confirmed/Capsule/Inp;
      }
      E_Conf_Out : Aedge
      {
        P1 = ./../Confirmed/Capsule/Out;
        P2 = ./../Capsule/Out/Int;
      }
      ViewData.Val = ./Confirmed/ADVar;
    }
    ./Capsule/Inp/Int/PinData - Provided = MDVarGet;
    ./Capsule/Out/Int/PinData - Required = MDVarGet;
    E_Data_Out : Aedge
    {
      P1 = ./../Data/Capsule/Out;
      P2 = ./../Capsule/Out/Int/PinData;
    }
    ViewData.Val = ./Data/Confirmed/ADVar;
  }
  $ # " StateVarC improved to use data with separate type ";
  StateVarCT : ./State
  {
    Data : /*/Modules/IncapsComps/Incaps
    {
      Confirmed : /*/Modules/DataComps/DataSCTVar;
      Prepared : /*/Modules/DataComps/DataSCTVar;
      ./Capsule - Inp : /*/Modules/SysComps/ExtenderInp
      {
        ./Int/Provided - $ = MDVarGet;
      }
      ./Capsule - Out : /*/Modules/SysComps/ExtenderOut
      {
        ./Int/Required - $ = MDVarGet;
      }
      E_Inp_Prep : Edge
      {
        ./P1 - $ = ./../../Capsule/Inp/Int;
        ./P2 - $ = ./../../Prepared/Capsule/Inp;
      }
      E_Prep_Conf : Edge
      {
        ./P1 - $ = ./../../Prepared/Capsule/Out;
        ./P2 - $ = ./../../Confirmed/Capsule/Inp;
      }
      E_Conf_Out : Edge
      {
        ./P1 - $ = ./../../Confirmed/Capsule/Out;
        ./P2 - $ = ./../../Capsule/Out/Int;
      }
      ViewData : Vert
      {
        Val : Prop
        {
          $ = ./../../Confirmed/Value;
        }
      }
    }
    ./Capsule/Inp/Int/PinData/Provided - $ = MDVarGet;
    ./Capsule/Out/Int/PinData/Required - $ = MDVarGet;
    E_Data_Out : Edge
    {
      ./P1 - $ = ./../../Data/Capsule/Out;
      ./P2 - $ = ./../../Capsule/Out/Int/PinData;
    }
    ViewData : Vert
    {
      Val : Prop
      {
        $ = ./../../Data/Confirmed/Value;
      }
    }
  }
  $ # " DES base ";
  Des : /*/Modules/IncapsComps/Incaps
  {
    DesAgt : ADes;
    ./Capsule - Sync : /*/Modules/SysComps/ConnPoint
    {
      ./Provided - $ = MDesSyncable;
    }
  }
  $ # " DES base, multicontent ";
  DesMc : /*/Modules/IncapsComps/Incaps
  {
    About = "Discrete Events System (DES) multicontent base";
    DesAgt : ADes;
    ./Capsule - Sync : ConnPointMc
    {
      Provided = MDesSyncable;
    }
  }
  $ # " DES ticks source ";
  DesTicker : /*/Modules/IncapsComps/Incaps
  {
    ./Capsule - Out : /*/Modules/SysComps/ConnPointOut
    {
      ./Required - $ = MDesSyncable;
    }
    DesSyncAgent : ADesSync;
  }
  $ # " DES ticks source, multicontent ";
  DesTickerMc : /*/Modules/IncapsComps/Incaps
  {
    About = "DES ticks generator";
    ./Capsule - Out : /*/Modules/SysComps/ConnPointOutMc
    {
      Required = MDesSyncable;
    }
    DesSyncAgent : ADesSync;
  }
  $ # " Statec out extender ";
  StcOutExt : AExtdOut
  {
    Int : CpStatecInp;
  }
  $ # " Statec Inp extender ";
  StcInpExt : AExtdInp
  {
    Int : CpStatecOutp;
  }
}
