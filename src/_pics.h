#pragma once
#include "pch.h"
#include <xx_frame.h>

struct _pics {
	xx::Frame c16;
	xx::Frame c32;
	xx::Frame c32_bullet;
	xx::Frame c64;
	xx::Frame c64_bullet;
	xx::Frame c64_light;
	xx::Frame c64_shadow;
	xx::Frame c128;
	xx::Frame c128_bucket;
	xx::Frame c128_door;
	xx::Frame c128_floor;
	xx::Frame c128_lava;
	xx::Frame c128_monster;
	xx::Frame c128_monster_portal;
	xx::Frame c128_player;
	xx::Frame c128_wall;
	xx::Frame c256_light;
	xx::Frame cursor_cross;
	xx::Frame cursor_default;
	xx::Frame light_firearrow;
	xx::Frame light_sword1;
	xx::Frame snake_body;
	xx::Frame snake_face;
	xx::Frame snake_tail;
	xx::Frame sword1;
	xx::Frame walls_shadowmask;
	std::array<xx::Frame, 484> creature_1_;
	std::array<xx::Frame, 8> explosion_1_;
	std::array<xx::Frame, 9> firearrow_;
	std::array<xx::Frame, 5> floor_;
	std::array<xx::Frame, 9> ore_;
	std::array<xx::Frame, 40> slime1_attack_;
	std::array<xx::Frame, 24> slime1_idle_;
	std::array<xx::Frame, 32> slime1_move_;
	std::array<xx::Frame, 3> walls_;

	xx::Shared<xx::GLTexture> Load(std::string picFN_, bool generateMipmap_ = true);
};
