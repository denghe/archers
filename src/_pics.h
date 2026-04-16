#pragma once
#include "pch.h"
#include <xx_frame.h>

struct _pics {
	xx::Frame c32;
	xx::Frame c32_bullet;
	xx::Frame c64;
	xx::Frame c64_bullet;
	xx::Frame c64_light;
	xx::Frame c128;
	xx::Frame c128_bucket;
	xx::Frame c128_door;
	xx::Frame c128_floor;
	xx::Frame c128_lava;
	xx::Frame c128_monster;
	xx::Frame c128_player;
	xx::Frame c128_wall;
	xx::Frame cursor_cross;
	xx::Frame cursor_default;
	std::array<xx::Frame, 484> creature_1_;
	std::array<xx::Frame, 8> explosion_1_;
	std::array<xx::Frame, 9> firearrow_;

	xx::Shared<xx::GLTexture> Load(std::string picFN_, bool generateMipmap_ = true);
};
