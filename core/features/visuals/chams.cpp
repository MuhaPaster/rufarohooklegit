#include "../features.hpp"

extern hooks::draw_model_execute::fn draw_model_execute_original;

void override_material(bool ignorez, bool wireframe, const color& rgba) {
	auto material = interfaces::material_system->find_material("debug/debugambientcube", TEXTURE_GROUP_MODEL);
	material->set_material_var_flag(material_var_ignorez, ignorez);
	material->set_material_var_flag(material_var_wireframe, wireframe);
	material->alpha_modulate(rgba.a / 255.f);
	material->color_modulate(rgba.r / 255.f, rgba.g / 255.f, rgba.b / 255.f);
	interfaces::model_render->override_material(material);
}

void visuals::players::chams_run(i_mat_render_context* ctx, const draw_model_state_t& state, const model_render_info_t& info, matrix_t* matrix) {
	if (!csgo::local_player)
		return;

	if (!variables::chams)
		return;
	
	const auto mdl = info.model;

	if (!mdl)
		return;

	bool is_player = strstr(mdl->name, "models/player") != nullptr;

	if (is_player) {
		player_t* player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(info.entity_index));
		if (!player || !player->is_alive() || player->dormant())
			return;
		if (player->has_gun_game_immunity()) {
			override_material(false, false, color(255, 255, 255, 100));
			draw_model_execute_original(interfaces::model_render, 0, ctx, state, info, matrix);
		}
		else {
			if (player->index() == csgo::local_player->index()) {
				override_material(false, false, color(255, 100, 255, csgo::local_player->is_scoped() ? 30 : 255));
				draw_model_execute_original(interfaces::model_render, 0, ctx, state, info, matrix);
			}
			else {
				if (player->team() != csgo::local_player->team()) {
					if (variables::backtrack_chams && records[player->index()].size() > 0) {
						for (uint32_t i = 0; i < records[player->index()].size(); i++) {
							if (!backtrack.valid_tick(records[player->index()][i].simulation_time, 0.2f) || records[player->index()][i].matrix == nullptr)
								continue;
							override_material(false, false, color(255 - (i * (255 / records[player->index()].size())), i * (255 / records[player->index()].size()), 255, 30));
							draw_model_execute_original(interfaces::model_render, 0, ctx, state, info, records[player->index()][i].matrix);
						}
					}
					switch (variables::chams) {
						case 1:
							override_material(false, false, color(255, 0, 255));
							draw_model_execute_original(interfaces::model_render, 0, ctx, state, info, matrix);
							break;
						case 2:
							override_material(true, false, color(0, 100, 255));
							draw_model_execute_original(interfaces::model_render, 0, ctx, state, info, matrix);
							override_material(false, false, color(255, 0, 255));
							draw_model_execute_original(interfaces::model_render, 0, ctx, state, info, matrix);
							break;
						default: break;
					}
				}
				else {
					override_material(false, false, color(0, 175, 255));
					draw_model_execute_original(interfaces::model_render, 0, ctx, state, info, matrix);
				}
			}
		}
	}
}