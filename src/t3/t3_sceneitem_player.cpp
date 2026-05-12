#include "pch.h"
#include "t3.h"

namespace Test3 {

	void Player::SetPos(XY pos_) {
		pos = pos_;
		y = pos.y;
	}

	void Player::Init(Scene* scene_) {
		typeId = cTypeId;
		scene = scene_;
		pos = {};	// todo
		y = pos.y;
		radius = cCreatureRadius;
		scale = radius * 2.f / gg.pics.c128_player.uvRect.w;
		radians = {};

		// 初始化数据面板
		healthMaxDefault = 100.f;
		PropsInit();
		// 测试下暴击效果
		criticalDamagePreset = 3.f;
		criticalChancePreset = 0.2f;
		PropsCalc();

		// 初始化武器
		weapon.Emplace<PlayerWeapon>()->Init(this);
	}

	void Player::Update() {
		// todo: 读鼠标位置并对齐 x, 横着移动. 但也有范围限制
	}

	void Player::Draw() {
		gg.Quad().DrawFrame(gg.pics.c128_monster, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians);
		weapon->Draw();
	}

	void Player::DrawLight() {
		gg.Quad().DrawFrame(gg.pics.c64_light, scene->cam.ToGLPos(pos)
			, (256.f / 64.f) * scene->cam.scale, 0, 0.5f);
		weapon->DrawLight();
	}

	void Player::DrawShadow() {
		// todo
		weapon->DrawShadow();
	}

	void Player::Dispose() {
		assert(scene);
		assert(!disposing);
		assert(indexAtContainer != -1);
		assert(scene->player.pointer == this);

		// 设置标记
		disposing = true;

		// 析构
		scene->player.Reset();
	}

}
