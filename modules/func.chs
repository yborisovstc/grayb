FuncComps : Elem
{
  $ # " Input of Func with Int arg/res ";
  Inp_FInt : /*/Modules/SysComps/ConnPointInp
  {
    ./Required - $ = MDIntGet;
    ./Provided - $ = MDataObserver;
  }
  $ # " Extended Input of Func with Int arg/res ";
  Inp_FEInt : /*/Modules/SysComps/Extender
  {
    ./Int/Provided - $ = MDIntGet;
    ./Int/Required - $ = MDataObserver;
  }
  $ # " Extended Input of Func with var arg/res ";
  Inp_FEVar : /*/Modules/SysComps/ExtenderInp
  {
    ./Int/Provided - $ = MDVarGet;
    ./Int/Required - $ = MDataObserver;
  }
  $ # " Output of Func with Int arg/res ";
  Out_FInt : /*/Modules/SysComps/ConnPointOut
  {
    ./Required - $ = MDataObserver;
    ./Provided - $ = MDIntGet;
  }
  $ # " Extended Output of Func with Int arg/res ";
  Out_FEInt : /*/Modules/SysComps/Extender
  {
    ./Int/Provided - $ = MDataObserver;
    ./Int/Required - $ = MDIntGet;
  }
  $ # " Extended Output of Func with Var arg/res ";
  Out_FEVar : /*/Modules/SysComps/ExtenderOut
  {
    ./Int/Provided - $ = MDataObserver;
    ./Int/Required - $ = MDVarGet;
  }
  $ # " Output of Bool Func ";
  Out_FBool : /*/Modules/SysComps/ConnPointOut
  {
    ./Required - $ = MDataObserver;
    ./Provided - $ = MDBoolGet;
  }
  $ # " Input of Bool Func ";
  Inp_FBool : /*/Modules/SysComps/ConnPointInp
  {
    ./Required - $ = MDBoolGet;
    ./Provided - $ = MDataObserver;
  }
  $ # " Output of Func with Vect Int ";
  Out_FVInt : /*/Modules/SysComps/ConnPointOut
  {
    ./Required - $ = MDataObserver;
    ./Provided - $ = MVIntGet;
  }
  $ # " Input of Func with Var arg/res ";
  Inp_FVar : /*/Modules/SysComps/ConnPointInp
  {
    ./Required - $ = MDVarGet;
    ./Provided - $ = MDataObserver;
  }
  $ # " Input of Func with Var arg/res, multicontent ";
  Inp_FVarMc : /*/Modules/SysComps/ConnPointInpMc
  {
    Required = MDVarGet;
    Provided = MDataObserver;
  }
  $ # " Output of Func with Var result ";
  Out_FVar : /*/Modules/SysComps/ConnPointOut
  {
    ./Required - $ = MDataObserver;
    ./Provided - $ = MDVarGet;
  }
  $ # " Output of Func with Var result, multicontent ";
  Out_FVarMc : /*/Modules/SysComps/ConnPointOutMc
  {
    Required = MDataObserver;
    Provided = MDVarGet;
  }
  $ # " Int function wo inputs  ";
  FuncInt : /*/Modules/IncapsComps/Incaps
  {
    ./Capsule - out : ./../../Out_FInt;
  }
  $ # " Var function wo inputs  ";
  FuncVar : /*/Modules/IncapsComps/Incaps
  {
    ./Capsule - out : ./../../Out_FVar;
    ViewData : Vert
    {
      Res : Prop;
    }
  }
  $ # " Var function wo inputs  ";
  FuncVarMc : /*/Modules/IncapsComps/Incaps
  {
    ./Capsule - out : ./../../Out_FVarMc;
    ViewData.Res = ./func_agt;
  }
  $ # " Composed/Reduced Var function wo inputs  ";
  FuncVarC : /*/Modules/IncapsComps/Incaps
  {
    ./Capsule - Out : ./../../Out_FEVar;
    ViewData : Vert
    {
      Res : Prop;
    }
  }
  $ # " Input of Transition Func with Int arg/res ";
  Inp_TFInt : /*/Modules/SysComps/ConnPointInp
  {
    ./Required - $ = MDIntGet;
  }
  $ # " Output of Transition Func with Int arg/res ";
  Out_TFInt : /*/Modules/SysComps/ConnPointOut
  {
    ./Provided - $ = MDIntGet;
  }
  $ # " Extended Input of Transition Func with Int arg/res ";
  Inp_TFEInt : /*/Modules/SysComps/Extender
  {
    ./Int/Provided - $ = MDIntGet;
  }
  $ # " Extended Output of Transition Func with Int arg/res ";
  Out_TFEInt : /*/Modules/SysComps/Extender
  {
    ./Int/Required - $ = MDIntGet;
  }
  $ # " Input of Transition Func with variable arg/res ";
  Inp_TFVar : /*/Modules/SysComps/ConnPointInp
  {
    ./Required - $ = MDVarGet;
  }
  $ # " Input of Transition Func with variable arg/res ";
  Inp_TFVarMc : /*/Modules/SysComps/ConnPointInpMc
  {
    About = "Input of Transition Func with variable arg/res";
    Required = MDVarGet;
  }
  $ # " Output of Transition Func with variable arg/res ";
  Out_TFVar : /*/Modules/SysComps/ConnPointOut
  {
    ./Provided - $ = MDVarGet;
  }
  $ # " Output of Transition Func with variable arg/res, multicontent ";
  Out_TFVarMc : /*/Modules/SysComps/ConnPointOutMc
  {
    Provided = MDVarGet;
  }
  $ # " Extended Input of Transition Func with Var arg/res ";
  Inp_TFEVar : /*/Modules/SysComps/ExtenderInp
  {
    ./Int/Provided - $ = MDVarGet;
  }
  Inp_TFEVarMc : /*/Modules/SysComps/CpExtenderInpMc
  {
    ./Int - Provided = MDVarGet;
  }
  $ # " Extended Output of Transition Func with Var arg/res ";
  Out_TFEVar : /*/Modules/SysComps/ExtenderOut
  {
    ./Int/Required - $ = MDVarGet;
  }
  Out_TFEVarMc : /*/Modules/SysComps/CpExtenderOutMc
  {
    ./Int - Required = MDVarGet;
  }
  $ # " Output of Bool Transition Func ";
  Out_TFBool : /*/Modules/SysComps/ConnPointOut
  {
    ./Provided - $ = MDBoolGet;
  }
  $ # " Input of Bool Transition Func ";
  Inp_TFBool : /*/Modules/SysComps/ConnPointInp
  {
    ./Required - $ = MDBoolGet;
  }
  $ # " Composed/Reduced Var transition function wo inputs  ";
  TFuncVarC : /*/Modules/IncapsComps/Incaps
  {
    ./Capsule - Out : ./../../Out_TFEVar;
  }
  TFuncVarCMc : /*/Modules/IncapsComps/Incaps
  {
    ./Capsule - Out : ./../../Out_TFEVarMc;
  }
  $ # " Int function with single input  ";
  FuncIntSi : /*/Modules/IncapsComps/Incaps
  {
    ./Capsule - inp : /*/Modules/SysComps/ConnPointInp
    {
      ./Required - $ = MDIntGet;
      ./Provided - $ = MDataObserver;
    }
    ./Capsule - out : /*/Modules/SysComps/ConnPointOut
    {
      ./Required - $ = MDataObserver;
      ./Provided - $ = MDIntGet;
    }
  }
  $ # " Bool function with simgle input  ";
  FuncBoolSi : /*/Modules/IncapsComps/Incaps
  {
    ./Capsule - out : ./../../Out_FBool;
  }
  FuncIncInt : /*/Modules/IncapsComps/Incaps
  {
    func_agt : AIncInt;
    ./Capsule - inp : /*/Modules/SysComps/ConnPointInp
    {
      ./Required - $ = MDIntGet;
      ./Provided - $ = MDataObserver;
    }
    ./Capsule - out : /*/Modules/SysComps/ConnPointOut
    {
      ./Required - $ = MDataObserver;
      ./Provided - $ = MDIntGet;
    }
  }
  $ # " Function of integer result ";
  FuncIntRes : /*/Modules/IncapsComps/Incaps
  {
    func_agt : AFunIntRes;
    ./Capsule - inp : /*/Modules/SysComps/ConnPointInp
    {
      ./Required - $ = MDIntGet;
      ./Provided - $ = MDataObserver;
    }
    ./Capsule - out : /*/Modules/SysComps/ConnPointOut
    {
      ./Required - $ = MDIntSet;
    }
  }
  $ # " Function of integer addition ";
  FuncAddInt : ./FuncIntSi
  {
    func_agt : AAddInt;
  }
  $ # " Function of integer addition wo caching ";
  FAddInt : ./FuncIntSi
  {
    func_agt : AFAddInt;
  }
  $ # " Function of var addition wo caching ";
  FAddVar : ./FuncVar
  {
    func_agt : AFAddVar;
    $ # " Input positives ";
    ./Capsule - Inp : ./../../Inp_FVar;
    ./ViewData/Res - $ = ./../../func_agt;
  }
  $ # " Function of var addition wo caching, multicontent ";
  FAddVarMc : ./FuncVarMc
  {
    func_agt : AFAddVar;
    $ # " Input positives ";
    ./Capsule - Inp : ./../../Inp_FVarMc;
    ViewData.Res = ./func_agt;
    SpAgent = ./func_agt;
  }
  $ # " Function of integer addition wo caching ";
  FSubInt : ./FuncInt
  {
    func_agt : AFSubInt;
    $ # " Input positives ";
    ./Capsule - InpP : ./../../Inp_FInt;
    $ # " Input negatives ";
    ./Capsule - InpN : ./../../Inp_FInt;
  }
  $ # " Function of restriction to upper and lower limits ";
  FLimInt : ./FuncInt
  {
    func_agt : AFLimInt;
    $ # " Input  ";
    ./Capsule - Inp : ./../../Inp_FInt;
    $ # " Input positives ";
    ./Capsule - Inp_LimU : ./../../Inp_FInt;
    $ # " Input negatives ";
    ./Capsule - Inp_LimL : ./../../Inp_FInt;
  }
  $ # " Function of division ";
  FDivInt : ./FuncInt
  {
    func_agt : AFDivInt;
    $ # " Divided  ";
    ./Capsule - Inp_DD : ./../../Inp_FInt;
    $ # " Divisor ";
    ./Capsule - Inp_DR : ./../../Inp_FInt;
  }
  $ # " Function of convolution. Working function is external ";
  FConvInt : ./FuncIntSi
  {
    func_agt : AFConvInt;
    $ # " Output for passing argunent to Working function  ";
    ./Capsule - Out_WFarg : ./../../Out_FEInt;
    $ # " Input for getting result from Working function  ";
    ./Capsule - Inp_WFres : ./../../Inp_FInt;
    $ # " Value of Working function argument ";
    WFArg : /*/Modules/DataComps/DataSNInt;
    E1 : Edge
    {
      ./P1 - $ = ./../../WFArg/Capsule/out;
      ./P2 - $ = ./../../Capsule/Out_WFarg/Int;
    }
  }
  $ # " Int  inputs to Int vector conversation ";
  FuncInpToVectInt : /*/Modules/IncapsComps/Incaps
  {
    $ # " Input ";
    ./Capsule - Inp : ./../../Inp_FInt;
    $ # " Output ";
    ./Capsule - Out : ./../../Out_FVInt;
  }
  $ # " Function Inp1 GT Inp2  ";
  FGTInt : ./FuncBoolSi
  {
    func_agt : AFGTInt;
    ./Capsule - Inp1 : ./../../Inp_FInt;
    ./Capsule - Inp2 : ./../../Inp_FInt;
  }
  $ # " Function Bool to Int  ";
  FBoolToInt : ./FuncInt
  {
    func_agt : AFBoolToInt;
    ./Capsule - Inp : ./../../Inp_FBool;
  }
  $ # " Transition function. It doesn's support observation of inputs change ";
  $ # " Int Transition function wo inputs  ";
  TFuncInt : /*/Modules/IncapsComps/Incaps
  {
    ./Capsule - out : ./../../Out_TFInt;
  }
  $ # " Bool Transition function wo inputs  ";
  TFuncBool : /*/Modules/IncapsComps/Incaps
  {
    ./Capsule - out : ./../../Out_TFBool;
  }
  $ # " Var Transition function wo inputs  ";
  TFuncVar : /*/Modules/IncapsComps/Incaps
  {
    ./Capsule - Out : ./../../Out_TFVar;
  }
  $ # " Var Transition function wo inputs  ";
  TFuncVarMc : /*/Modules/IncapsComps/Incaps
  {
    About = "Var Transition function wo inputs";
    ./Capsule - Out : ./../../Out_TFVarMc;
    ViewData.Res = ./func_agt;
  }
  $ # " Function of integer addition wo caching ";
  TFAddInt : ./TFuncInt
  {
    func_agt : AFAddInt;
    ./Capsule - inp : ./../../Inp_TFInt;
  }
  $ # " Function of integer addition wo caching ";
  TFSubInt : ./TFuncInt
  {
    func_agt : AFSubInt;
    $ # " Input positives ";
    ./Capsule - InpP : ./../../Inp_TFInt;
    $ # " Input negatives ";
    ./Capsule - InpN : ./../../Inp_TFInt;
  }
  $ # " Function of restriction to upper and lower limits ";
  TFLimInt : ./TFuncInt
  {
    func_agt : AFLimInt;
    $ # " Input  ";
    ./Capsule - Inp : ./../../Inp_TFInt;
    $ # " Input positives ";
    ./Capsule - Inp_LimU : ./../../Inp_TFInt;
    $ # " Input negatives ";
    ./Capsule - Inp_LimL : ./../../Inp_TFInt;
  }
  $ # " Function of division ";
  TFDivInt : ./TFuncInt
  {
    func_agt : AFDivInt;
    $ # " Divided  ";
    ./Capsule - Inp_DD : ./../../Inp_TFInt;
    $ # " Divisor ";
    ./Capsule - Inp_DR : ./../../Inp_TFInt;
  }
  $ # " Function of convolution. Working function is external ";
  TFConvInt : ./TFuncInt
  {
    func_agt : AFConvInt;
    ./Capsule - inp : ./../../Inp_TFInt;
    $ # " Output for passing argunent to Working function  ";
    ./Capsule - Out_WFarg : ./../../Out_TFEInt;
    $ # " Input for getting result from Working function  ";
    ./Capsule - Inp_WFres : ./../../Inp_TFInt;
    $ # " Value of Working function argument ";
    WFArg : /*/Modules/DataComps/DataSUInt;
    E1 : Edge
    {
      ./P1 - $ = ./../../WFArg/Capsule/out;
      ./P2 - $ = ./../../Capsule/Out_WFarg/Int;
    }
  }
  $ # " Function Inp1 GT Inp2  ";
  TFGTInt : ./TFuncBool
  {
    func_agt : AFGTInt;
    ./Capsule - Inp1 : ./../../Inp_TFInt;
    ./Capsule - Inp2 : ./../../Inp_TFInt;
  }
  $ # " Transition Function EQ  ";
  TFEQVar : ./TFuncVar
  {
    AF_Eq : AFCmpVar;
    ./Capsule - Inp1 : ./../../Inp_TFVar;
    ./Capsule - Inp2 : ./../../Inp_TFVar;
    ViewData.Res = ./AF_Eq;
  }
  $ # " Transition Function GE  ";
  TFGEVar : ./TFuncVar
  {
    AF_Ge : AFCmpVar;
    ./Capsule - Inp1 : ./../../Inp_TFVar;
    ./Capsule - Inp2 : ./../../Inp_TFVar;
    ViewData.Res = ./AF_Ge;
  }
  $ # " Transition Function LT  ";
  TFLTVar : ./TFuncVar
  {
    AF_Lt : AFCmpVar;
    ./Capsule - Inp1 : ./../../Inp_TFVar;
    ./Capsule - Inp2 : ./../../Inp_TFVar;
    ViewData.Res = ./AF_Lt;
  }
  $ # " Transition Function of variable adding ";
  TFAddVar : ./TFuncVar
  {
    func_agt : AFAddVar;
    $ # " Input positives ";
    ./Capsule - Inp : ./../../Inp_TFVar;
    ViewData.Res = ./func_agt;
  }
  $ # " Transition Function of variable adding ";
  TFAddVarMc : ./TFuncVarMc
  {
    About = "Transition Function of variable adding";
    func_agt : AFAddVar;
    ./Capsule - Inp : ./../../Inp_TFVarMc;
    ViewData.Res = ./func_agt;
    SpAgent = ./func_agt;
  }
  $ # " Transition Function of variable adding and substraction ";
  TFAddNVar : ./TFuncVar
  {
    func_agt : AFAddVar;
    $ # " Input positives ";
    ./Capsule - Inp : ./../../Inp_TFVar;
    $ # " Input negative ";
    ./Capsule - InpN : ./../../Inp_TFVar;
    ViewData.Res = ./func_agt;
  }
  $ # " Transition Function of variable multiplication ";
  TFMplVar : ./TFuncVar
  {
    func_agt : AFMplVar;
    $ # " Input positives ";
    ./Capsule - Inp : ./../../Inp_TFVar;
    ViewData.Res = ./func_agt;
  }
  $ # " Transition Function of variable mutltiplication, non commutative ";
  TFMplncVar : ./TFuncVar
  {
    func_agt : AFMplncVar;
    ./Capsule - Inp1 : ./../../Inp_TFVar;
    ./Capsule - Inp2 : ./../../Inp_TFVar;
    ViewData.Res = ./func_agt;
  }
  TFMplncVarMc : ./TFuncVarMc
  {
    func_agt : AFMplncVar;
    ./Capsule - Inp1 : ./../../Inp_TFVarMc;
    ./Capsule - Inp2 : ./../../Inp_TFVarMc;
    ViewData.Res = ./func_agt;
  }
  $ # " Transition Function of variable mutltiplicationi inversion ";
  TFMplinvVar : ./TFuncVar
  {
    func_agt : AFMplinvVar;
    ./Capsule - Inp : ./../../Inp_TFVar;
    ViewData.Res = ./func_agt;
  }
  $ # " Transition Function of variable creation of diag matrix ";
  TFCpsMtrdVar : ./TFuncVar
  {
    func_agt : AFCpsMtrdVar;
    ./Capsule - Inp : ./../../Inp_TFVar;
    ViewData.Res = ./func_agt;
  }
  TFCpsMtrdVarMc : ./TFuncVarMc
  {
    func_agt : AFCpsMtrdVar;
    ./Capsule - Inp : ./../../Inp_TFVarMc;
    ViewData.Res = ./func_agt;
  }
  $ # " Transition Function of composing of vector from comps, variable ";
  TFCpsVectVar : ./TFuncVar
  {
    func_agt : AFCpsVectVar;
    ./Capsule - Inp1 : ./../../Inp_TFVar;
    ./Capsule - Inp2 : ./../../Inp_TFVar;
    ViewData.Res = ./func_agt;
  }
  $ # " Transition Function of variable casting ";
  TFCastVar : ./TFuncVar
  {
    func_agt : AFCastVar;
    ./Capsule - Inp : ./../../Inp_TFVar;
    InpType : Prop;
    ViewData.Res = ./func_agt;
  }
  $ # " Transition Function of getting component of containter  ";
  TFAtVar : ./TFuncVar
  {
    func_agt : AFAtVar;
    ./Capsule - Inp : ./../../Inp_TFVar;
    ./Capsule - Index : ./../../Inp_TFVar;
    ViewData.Res = ./func_agt;
  }
  TFAtVarMc : ./TFuncVarMc
  {
    func_agt : AFAtVar;
    ./Capsule - Inp : ./../../Inp_TFVarMc;
    ./Capsule - Index : ./../../Inp_TFVarMc;
    ViewData.Res = ./func_agt;
  }
  $ # " Transition Function of boolean negation ";
  TFNegVar : ./TFuncVar
  {
    func_agt : AFBoolNegVar;
    ./Capsule - Inp : ./../../Inp_TFVar;
    ViewData.Res = ./func_agt;
  }
  $ # " Transition Function of getting component of containter, reduced by index  ";
  TFAtVarI : ./TFuncVarC
  {
    At : ./../TFAtVar;
    Index : /*/Modules/DataComps/DataSCVar;
    ./Capsule - Inp : ./../../Inp_TFEVar;
    E1 : Edge
    {
      ./P1 - $ = ./../../Capsule/Inp/Int;
      ./P2 - $ = ./../../At/Capsule/Inp;
    }
    E2 : Edge
    {
      ./P1 - $ = ./../../At/Capsule/Out;
      ./P2 - $ = ./../../Capsule/Out/Int;
    }
    E3 : Edge
    {
      ./P1 - $ = ./../../Index/Capsule/Out;
      ./P2 - $ = ./../../At/Capsule/Index;
    }
    ViewData.Res = ./At/func_agt;
  }
  TFAtVarIMc : ./TFuncVarCMc
  {
    At : ./../TFAtVarMc;
    Index : /*/Modules/DataComps/DataSCVarMc;
    ./Capsule - Inp : ./../../Inp_TFEVarMc;
    E1 : Aedge
    {
      P1 = ./../Capsule/Inp/Int;
      P2 = ./../At/Capsule/Inp;
    }
    E2 : Aedge
    {
      P1 = ./../At/Capsule/Out;
      P2 = ./../Capsule/Out/Int;
    }
    E3 : Aedge
    {
      P1 = ./../Index/Capsule/Out;
      P2 = ./../At/Capsule/Index;
    }
    ViewData.Res = ./At/func_agt;
  }
  $ # " Function Inp1 LT Inp2  ";
  FLTVar : ./FuncVar
  {
    AF_Lt : AFBcmpVar;
    ./Capsule - Inp1 : ./../../Inp_FVar;
    ./Capsule - Inp2 : ./../../Inp_FVar;
  }
  $ # " Function Inp1 LE Inp2  ";
  FLEVar : ./FuncVar
  {
    AF_Le : AFBcmpVar;
    ./Capsule - Inp1 : ./../../Inp_FVar;
    ./Capsule - Inp2 : ./../../Inp_FVar;
  }
  $ # " Function Inp1 EQ Inp2  ";
  FEQVar : ./FuncVar
  {
    AF_Eq : AFCmpVar;
    ./Capsule - Inp1 : ./../../Inp_FVar;
    ./Capsule - Inp2 : ./../../Inp_FVar;
    ./ViewData/Res - $ = ./../../AF_Eq;
  }
  $ # " Function Inp1 GT Inp2  ";
  FGTVar : ./FuncVar
  {
    AF_Gt : AFBcmpVar;
    ./Capsule - Inp1 : ./../../Inp_FVar;
    ./Capsule - Inp2 : ./../../Inp_FVar;
  }
  $ # " Function Inp1 GE Inp2  ";
  FGEVar : ./FuncVar
  {
    AF_Ge : AFBcmpVar;
    ./Capsule - Inp1 : ./../../Inp_FVar;
    ./Capsule - Inp2 : ./../../Inp_FVar;
  }
  $ # " Function of getting component of containter  ";
  FAtVar : ./FuncVar
  {
    func_agt : AFAtVar;
    ./Capsule - Inp : ./../../Inp_FVar;
    ./Capsule - Index : ./../../Inp_FVar;
    ./ViewData/Res - $ = ./../../func_agt;
  }
  $ # " Function of getting component of containter, reduced by index  ";
  FAtVarI : ./FuncVarC
  {
    At : ./../FAtVar;
    Index : /*/Modules/DataComps/DataTVar;
    ./Capsule - Inp : ./../../Inp_FEVar;
    E1 : Edge
    {
      ./P1 - $ = ./../../Capsule/Inp/Int;
      ./P2 - $ = ./../../At/Capsule/Inp;
    }
    E2 : Edge
    {
      ./P1 - $ = ./../../At/Capsule/out;
      ./P2 - $ = ./../../Capsule/Out/Int;
    }
    E3 : Edge
    {
      ./P1 - $ = ./../../Index/Capsule/out;
      ./P2 - $ = ./../../At/Capsule/Index;
    }
    ./ViewData/Res - $ = ./../../At/func_agt;
  }
  FAtVar_0 : ./FAtVarI
  {
    ./Index/Value - $ = "SI 0";
  }
  FAtVar_1 : ./FAtVarI
  {
    ./Index/Value - $ = "SI 1";
  }
  $ # " Function of swithching  ";
  FSwitchVar : ./FuncVar
  {
    func_agt : AFSwitchVar;
    ./Capsule - Inp1 : ./../../Inp_FVar;
    ./Capsule - Inp2 : ./../../Inp_FVar;
    ./Capsule - Sel : ./../../Inp_FVar;
  }
  $ # " Transition Function of switching  ";
  TFSwitchVar : ./TFuncVar
  {
    func_agt : AFSwitchVar;
    ./Capsule - Inp1 : ./../../Inp_TFVar;
    ./Capsule - Inp2 : ./../../Inp_TFVar;
    ./Capsule - Sel : ./../../Inp_TFVar;
  }
  $ # " Function Bool to Int  ";
  TFBoolToInt : ./TFuncInt
  {
    func_agt : AFBoolToInt;
    ./Capsule - Inp : ./../../Inp_TFBool;
  }
  $ # " Function of var multiplication wo caching ";
  FMplVar : ./FuncVar
  {
    func_agt : AFMplVar;
    ./Capsule - Inp : ./../../Inp_FVar;
  }
  $ # " Function of var non commutative multiplication wo caching ";
  FMplncVar : ./FuncVar
  {
    func_agt : AFMplncVar;
    ./Capsule - Inp1 : ./../../Inp_FVar;
    ./Capsule - Inp2 : ./../../Inp_FVar;
    ./ViewData/Res - $ = ./../../func_agt;
  }
  $ # " Function of variable mutltiplicationi inversion ";
  FMplinvVar : ./FuncVar
  {
    func_agt : AFMplinvVar;
    ./Capsule - Inp : ./../../Inp_FVar;
    ./ViewData/Res - $ = ./../../func_agt;
  }
  $ # " Function of variable casting ";
  FCastVar : ./FuncVar
  {
    func_agt : AFCastVar;
    ./Capsule - Inp : ./../../Inp_FVar;
    ./ViewData/Res - $ = ./../../func_agt;
  }
  $ # " Function of variable creation of diag matrix ";
  FCpsMtrdVar : ./FuncVar
  {
    func_agt : AFCpsMtrdVar;
    ./Capsule - Inp : ./../../Inp_FVar;
    ./ViewData/Res - $ = ./../../func_agt;
  }
  $ # " Function of composing of vector from comps, variable ";
  FCpsVectVar : ./FuncVar
  {
    func_agt : AFCpsVectVar;
    ./Capsule - Inp1 : ./../../Inp_FVar;
    ./Capsule - Inp2 : ./../../Inp_FVar;
    ./ViewData/Res - $ = ./../../func_agt;
  }
  $ # " Function of var dividing wo caching ";
  FDivVar : ./FuncVar
  {
    func_agt : AFDivVar;
    ./Capsule - Inp : ./../../Inp_FVar;
    ./Capsule - Inp_Dvs : ./../../Inp_FVar;
  }
}
