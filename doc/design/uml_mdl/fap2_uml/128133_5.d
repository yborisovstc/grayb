format 74

classcanvas 128005 class_ref 128005 // _daa_rmt_agent
  draw_all_relations default hide_attributes default hide_operations default hide_getset_operations default show_members_full_definition default show_members_visibility default show_members_stereotype default show_members_context default show_members_multiplicity default show_members_initialization default show_attribute_modifiers default member_max_width 0 show_parameter_dir default show_parameter_name default package_name_in_tab default class_drawing_mode default drawing_language default show_context_mode default auto_label_position default show_relation_modifiers default show_relation_visibility default show_infonote default shadow default show_stereotype_properties default
  xyz 97 163 2000
end
classcanvas 128133 class_ref 128517 // _daa_agent_proxy
  draw_all_relations default hide_attributes default hide_operations default hide_getset_operations default show_members_full_definition default show_members_visibility default show_members_stereotype default show_members_context default show_members_multiplicity default show_members_initialization default show_attribute_modifiers default member_max_width 0 show_parameter_dir default show_parameter_name default package_name_in_tab default class_drawing_mode default drawing_language default show_context_mode default auto_label_position default show_relation_modifiers default show_relation_visibility default show_infonote default shadow default show_stereotype_properties default
  xyz 91 238 2000
end
classcanvas 128261 class_ref 128389 // _daa_rmt_agent_proxy
  draw_all_relations default hide_attributes default hide_operations default hide_getset_operations default show_members_full_definition default show_members_visibility default show_members_stereotype default show_members_context default show_members_multiplicity default show_members_initialization default show_attribute_modifiers default member_max_width 0 show_parameter_dir default show_parameter_name default package_name_in_tab default class_drawing_mode default drawing_language default show_context_mode default auto_label_position default show_relation_modifiers default show_relation_visibility default show_infonote default shadow default show_stereotype_properties default
  xyz 81 325 2000
end
classcanvas 128517 class_ref 128645 // _daa_env_client
  draw_all_relations default hide_attributes default hide_operations default hide_getset_operations default show_members_full_definition default show_members_visibility default show_members_stereotype default show_members_context default show_members_multiplicity default show_members_initialization default show_attribute_modifiers default member_max_width 0 show_parameter_dir default show_parameter_name default package_name_in_tab default class_drawing_mode default drawing_language default show_context_mode default auto_label_position default show_relation_modifiers default show_relation_visibility default show_infonote default shadow default show_stereotype_properties default
  xyz 463 296 2000
end
classcanvas 128645 class_ref 128133 // _daa_env
  draw_all_relations default hide_attributes default hide_operations default hide_getset_operations default show_members_full_definition default show_members_visibility default show_members_stereotype default show_members_context default show_members_multiplicity default show_members_initialization default show_attribute_modifiers default member_max_width 0 show_parameter_dir default show_parameter_name default package_name_in_tab default class_drawing_mode default drawing_language default show_context_mode default auto_label_position default show_relation_modifiers default show_relation_visibility default show_infonote default shadow default show_stereotype_properties default
  xyz 292 139 2000
end
note 129029 " Consideration of approach where special type of native agent is used for root agent in remove environment, ref ds_daa_proxy_crh_stx_agt. Remote_agent is a native agent that is top-level agent in remote env. Actually it is not single agent but pair: agent itself and agent proxy. Agent proxy is created first in the original environment (the env that derives the agent), then the proxy manages creation of the agent itself."
  xyzwh 13 5 2000 779 67
relationcanvas 128389 relation_ref 128005 // <generalisation>
  from ref 128261 z 2001 to ref 128133
  no_role_a no_role_b
  no_multiplicity_a no_multiplicity_b
end
relationcanvas 128901 relation_ref 128261 // <directional composition>
  from ref 128645 z 2001 to ref 128517
  no_role_a no_role_b
  no_multiplicity_a no_multiplicity_b
end
end
