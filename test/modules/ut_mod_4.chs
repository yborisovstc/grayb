Mod4_root : Vert
{
  $ + /Mod5_root/Mod5_comp_2;
  Mod_comp_2 : /*/Modules/Mod5_root/Mod5_comp_2
  {
    $ # " TODO TMP remove to avoid names dups, to return
	<rm node=./Mod5_comp_2_1/Mod5_comp_2_1_1/> ";
    Vert1 : Vert;
    $ # " TODO TMP remove to avoid names dups, to return
	<node node=./Mod5_comp_2_1 id=Mod5_comp_2_1_1 parent=./Vert1/> ";
    ./Mod5_comp_2_1/Mod5_comp_2_e1/P1 - $ = ./../../Mod5_comp_2_1_1;
  }
}
