#include "pch.h"
#include "t3.h"

namespace Test3 {

	void PlayerBullet::Init(PlayerWeapon* weapon_) {
		typeId = cTypeId;
		owner = weapon_->owner;
		scene = owner->scene;
		weapon = xx::WeakFromThis(weapon_);

		pos = weapon->pos;
		y = pos.y + 1.f;
		radius = cCreatureRadius * 0.5f;
		scale = radius * 2.f / gg.pics.c64_bullet.uvRect.h;

		radians = 0;

		baseDamage = gg.rnd.Next<int32_t>(1, 10);
	}

	void PlayerBullet::Update() {
		// 同步 owner 坐标
		pos = owner->pos;
		y = pos.y + 1.f;

		// todo: shoot
	}

	void PlayerBullet::Draw() {
		auto& f = gg.pics.sword1;
		gg.Quad().DrawFrame(f, scene->cam.ToGLPos(pos)
			, cWidth / 7.f * scene->cam.scale, radians);
	}

	void PlayerBullet::DrawLight() {
		auto& f = gg.pics.light_sword1;
		gg.Quad().DrawFrame(f, scene->cam.ToGLPos(pos)
			, cWidth / 7.f * scene->cam.scale, radians);
	}

	void PlayerBullet::Dispose() {
		assert(scene);
		assert(!disposing);

		// 设置标记
		disposing = true;

		// 自杀
		owner->weapon.Reset();
	}

}
