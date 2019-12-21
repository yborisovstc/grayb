Mod_root_3 : Vert
{
  $ + /Mod2_root_3/Mod2_comp_2;
  Mod3_comp_int2 : Vert;
  Mod3_comp_int : Vert
  {
    Mod3_comp_int_c1 : Vert;
  }
  Mod_comp_2 : /*/Modules/Mod2_root_3/Mod2_comp_2;
  Mod3_comp_3 : ./Mod3_comp_int;
  $ # " Test on handling of cases when system has to resolve model hier uri that
	 doesn't directly correspond chromo uri becasue of nodes destination path ";
  Mod3_c4 : ./Mod3_comp_int
  {
    ./Mod3_comp_int_c1 - Mod3_c4_c1 : Vert
    {
      Mod3_c4_c1_c1 : ./../../../Mod3_comp_int2;
    }
  }
}
