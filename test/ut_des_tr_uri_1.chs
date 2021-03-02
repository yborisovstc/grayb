testroot : Elem
{
    Modules : AImports
    {
        $ # " Module containing components of des layer ";
        $ + /DesComps/Des;
    }
    test : ./Modules/DesComps/Des
    {
        About = "Simple test of Uris comparition";
        Log.Creation = y;
        ResEq : AStatec {
            Debug.Update = y;
            Value = "SB false";
        }
        ResEq/Inp ~ U_Eq : ATrcCmpVar @ {
            Inp ~ : ATrcUri @ {
                Inp ~ : AStatec {
		     Value = "SS /Test/Node1";
                 };
            }; 
            Inp2 ~ : ATrcUri @ {
                 Inp ~ : AStatec {
		     Value = "SS /Test/*";
                 };
            }; 
        };
        ResNeq : AStatec {
            Debug.Update = y;
            Value = "SB false";
        }
        ResNeq/Inp ~ U_Neq : ATrcCmpVar @ {
            Inp ~ : ATrcUri @ {
                Inp ~ : AStatec {
		     Value = "SS /Test/Node1";
                 };
            }; 
            Inp2 ~ : ATrcUri @ {
                 Inp ~ : AStatec {
		     Value = "SS /Test/Node2";
                 };
            }; 
        };

    }
}
