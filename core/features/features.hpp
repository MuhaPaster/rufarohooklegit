#pragma once
#include "../../dependencies/utilities/csgo.hpp"
#include "../menu/variables.hpp"

#include "lagcompensation/backtrack.hpp"
#include "legit/triggerbot.hpp"
#include "legit/legitbot.hpp"

namespace visuals {
	namespace players {
		void esp_draw();
		void chams_run(i_mat_render_context* ctx, const draw_model_state_t& state, const model_render_info_t& info, matrix_t* matrix);
	}
	namespace entities {
		void grenades_draw();
		void c4_draw();
	}
	namespace misc {
		void recoil_crosshair_draw();
		void spectator_list_draw();
		void modulateworld();
	}
}

namespace misc {
	namespace movement {
		void bunny_hop(c_usercmd* cmd);
	};
	void thirdperson();
	void nade_prediction();
	bool rcs();
}

extern int showtime;
extern std::string prefix;
class CMessage
{
public:
	CMessage(std::string str, float time, color clr, int a)
	{
		this->str = str;
		this->time = time;
		this->clr = clr;
		alpha = a;
	}
	std::string str;
	float time;
	color clr;
	int alpha;
};

namespace events
{
	extern std::vector <CMessage> messages;
	extern 	void log(std::string str, color clr);
	extern void print();
}