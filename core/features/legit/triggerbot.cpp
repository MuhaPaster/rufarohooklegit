#include "../features.hpp"

static bool keyPressed;

constexpr bool isArmored(int hitGroup, bool helmet) noexcept
{
    switch (hitGroup) {
    case hitgroup_head:
        return helmet;

    case hitgroup_chest:
    case hitgroup_stomach:
        return true;
    default:
        return false;
    }
}

constexpr float getDamageMultiplier(int hitGroup) noexcept
{
    switch (hitGroup) {
    case hitgroup_head:
        return 4.0f;
    case hitgroup_stomach:
        return 1.25f;
    case hitgroup_rightleg:
    case hitgroup_leftleg:
        return 0.75f;
    default:
        return 1.0f;
    }
}

// selfcode lmao
void triggerbot::run(c_usercmd* cmd) {

    if (!GetAsyncKeyState(VK_XBUTTON1))
        return;

    vec3_t view = cmd->viewangles;
    view += csgo::local_player->aim_punch_angle();

    vec3_t forward;
    math::angle_vectors(view, &forward);

    vec3_t start = csgo::local_player->get_eye_pos();

    forward *= 8192;
    vec3_t end = start + forward;

    ray_t ray;
    ray.initialize(start, end);

    trace_filter filter;
    filter.skip = csgo::local_player;

    trace_t trace;
    interfaces::trace_ray->trace_ray(ray, MASK_SHOT, &filter, &trace);

    auto entity = reinterpret_cast<entity_t*>(trace.entity);
    if (!entity)
        return;

    auto player = reinterpret_cast<player_t*>(entity);
    auto valid_player = [&]() {
        if (!player || !player->is_alive() || player->dormant() || player->team() == csgo::local_player->team())
            return false;

        return !(player->has_gun_game_immunity());
    };

    if (trace.hitGroup >= 0 && trace.hitGroup <= 7)
        if ((trace.did_hit() && valid_player()) && (interfaces::globals->cur_time + 1.f))
            cmd->buttons |= in_attack;
}

/*void triggerbot::run(c_usercmd* cmd) noexcept
{
    if (!csgo::local_player || !csgo::local_player->is_alive() || csgo::local_player->next_attack() > csgo::local_player->get_tick_base() || csgo::local_player->is_defusing())
        return;

    const auto activeWeapon = csgo::local_player->active_weapon();
    if (!activeWeapon || !activeWeapon->clip1_count() || activeWeapon->next_primary_attack() > csgo::local_player->get_tick_base())
        return;

    if (csgo::local_player->shots_fired() > 0)
        return;

    if (GetAsyncKeyState(VK_XBUTTON1))
        keyPressed = true;

    static auto lastTime = 0.0f;
    static auto lastContact = 0.0f;

    const auto now = interfaces::globals->realtime;

    lastContact = 0.0f;

    if (now - lastTime < 15 / 1000.0f)
        return;

    if (!csgo::local_player->is_flashed())
        return;

    auto startPos = csgo::local_player->get_eye_pos();
    const vec3_t endPos = startPos + vec3_t::fromAngle(cmd->viewangles + csgo::local_player->aim_punch_angle()) * 8192;

    trace_filter filter;
    filter.skip = csgo::local_player;

    trace_t trace;
    ray_t ray;
    ray.initialize(startPos, endPos);

    interfaces::trace_ray->trace_ray(ray, 0x46004009, &filter, &trace);

    lastTime = now;

    if (!keyPressed)
        return;

    if (!trace.entity || !trace.entity->is_player() || csgo::local_player->team() == trace.entity->team())
        return;

    if (trace.entity->has_gun_game_immunity())
        return;

    if (!(trace.hitGroup == hitgroup_head || trace.hitGroup == hitgroup_stomach || trace.hitGroup == hitgroup_chest || (trace.hitGroup == hitgroup_leftarm || trace.hitGroup == hitgroup_rightarm) || (trace.hitGroup == hitgroup_leftleg || trace.hitGroup == hitgroup_rightleg)))
        return;

    float damage = (activeWeapon->item_definition_index() != WeaponId::Taser ? getDamageMultiplier(trace.hitGroup) : 1.0f) * 115 * std::pow(0.99, trace.flFraction * 8192 / 500.0f);

    if (float armorRatio{ 97.50 / 2.0f }; isArmored(trace.hitGroup, trace.entity->has_helmet()))
        damage -= (trace.entity->armor() < damage * armorRatio / 2.0f ? trace.entity->armor() * 4.0f : damage) * (1.0f - armorRatio);
        
    if (damage >= (trace.entity->health() / 2)) {
        cmd->buttons |= in_attack;
        lastTime = 0.0f;
        lastContact = now;
    }
}

/*void triggerbot::run(c_usercmd* usercmd) noexcept {
    vec3_t rem, forward,
        src = csgo::local_player->get_eye_pos();

    trace_t tr;
    ray_t ray;
    trace_filter filter;
    filter.skip = csgo::local_player;

    vec3_t viewangles = usercmd->viewangles;

    viewangles += csgo::local_player->aim_punch_angle() * 2.f;

    math::angle_vectors(viewangles, &forward);

    forward *= csgo::local_player->active_weapon()->get_weapon_data()->range;

    rem = src + forward;

    ray.initialize(src, rem);
    interfaces::trace_ray->trace_ray(ray, 0x46004003, &filter, &tr);

    if (!tr.did_hit())
        return;

    bool dh = false;

    if (tr.hitGroup == hitgroup_head || tr.hitGroup == hitgroup_stomach || tr.hitGroup == hitgroup_chest || (tr.hitGroup == hitgroup_leftarm || tr.hitGroup == hitgroup_rightarm) || (tr.hitGroup == hitgroup_leftleg || tr.hitGroup == hitgroup_rightleg))
        dh = true;

    auto player = reinterpret_cast<entity_t*>(tr.entity);
    if (player && !player->dormant() && player->is_player()) { // might shoot at dead people
        if (player->team() != csgo::local_player->team()) {

            if (false) {
                if (dh && (!(usercmd->buttons & in_attack) && (!GetAsyncKeyState(VK_LBUTTON)))) // if you don't attack currently
                    usercmd->buttons |= in_attack;

                static bool already_shot = false;
                if (csgo::local_player->active_weapon()->is_pistol()) {
                    if (usercmd->buttons & in_attack)
                        if (already_shot)
                            usercmd->buttons &= ~in_attack;

                    already_shot = usercmd->buttons & in_attack ? true : false;
                }
            }
            else {
                if (dh && (!(usercmd->buttons & in_attack) && !GetAsyncKeyState(VK_LBUTTON))) // if you don't attack currently
                    usercmd->buttons |= in_attack;

                static bool already_shot = false;
                if (csgo::local_player->active_weapon()->is_pistol()) {
                    if (usercmd->buttons & in_attack)
                        if (already_shot)
                            usercmd->buttons &= ~in_attack;

                    already_shot = usercmd->buttons & in_attack ? true : false;
                }
            }
        }
    }
}*/