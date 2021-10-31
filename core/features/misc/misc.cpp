#include "../features.hpp"

void misc::movement::bunny_hop(c_usercmd* cmd) {
	if (!variables::bunnyhop)
		return;

	const int move_type = csgo::local_player->move_type();

	if (move_type == movetype_ladder || move_type == movetype_noclip || move_type == movetype_observer)
		return;

	static auto wasLastTimeOnGround{ csgo::local_player->flags() & 1 };

	if (!(csgo::local_player->flags() & fl_onground) && !wasLastTimeOnGround)
		cmd->buttons &= ~in_jump;

	wasLastTimeOnGround = csgo::local_player->flags() & 1;
};

void misc::thirdperson() {
	static bool in_thirdperson = false;

	if (GetAsyncKeyState(VK_MBUTTON) & 1)
		in_thirdperson = !in_thirdperson;

	if (interfaces::input->camera_in_third_person = in_thirdperson)
		interfaces::input->camera_offset.z = 100;
}

void misc::nade_prediction() {
	static auto nadeVar{ interfaces::console->get_convar("cl_grenadepreview") };

	nadeVar->callbacks.size = 0;
	nadeVar->set_value(true);
}

bool misc::rcs( ) {
    if (!csgo::local_player || !csgo::local_player->is_alive() || !csgo::local_player->active_weapon())
        return false;
        
    auto clientclass = csgo::local_player->client_class();
    if (!clientclass)
        return false;

    auto classids = clientclass->class_id;
    if (classids == class_ids::CWeaponTaser || classids == class_ids::CKnife || classids == class_ids::CKnifeGG)
        return false;

    vec3_t angle;
    vec3_t m_ViewAngle;
    vec3_t OldAngle;
    int ShotsFired = 0;
    while (true) {
        if (GetAsyncKeyState(0x01)) {
            ShotsFired = csgo::local_player->shots_fired();
            if (ShotsFired > 1) {
                m_ViewAngle = csgo::local_player->eye_angles();

                vec3_t m_PunchAngle = csgo::local_player->punch_angle();

                m_ViewAngle.x = m_ViewAngle.x + OldAngle.x;
                m_ViewAngle.y = m_ViewAngle.y + OldAngle.y;
                // Add the old "viewpunch" so we get the "center" of the screen again

                angle.x = m_ViewAngle.x - m_PunchAngle.x * 2;
                angle.y = m_ViewAngle.y - m_PunchAngle.y * 2;
                // remove the new "viewpunch" from the viewangles
                csgo::local_player->set_angles(angle);

                OldAngle.x = m_PunchAngle.x * 2;
                OldAngle.y = m_PunchAngle.y * 2;
            }
            else {
                OldAngle.x = 0;
                OldAngle.y = 0;
            }
        }
        else {
            OldAngle.x = 0;
            OldAngle.y = 0;
        }
    }
}