#include "pch.h"
#include "t3.h"

namespace Test3 {

	void PlayerWeapon::Init(Player* owner_) {
		typeId = cTypeId;
		scene = owner_->scene;
		owner = xx::WeakFromThis(owner_);

		pos = owner_->pos;
		y = pos.y + 1.f;
		radius = cCreatureRadius * 0.5f;
		scale = radius * 2.f / gg.pics.c64_bullet.uvRect.h;

		radians = -gPI_2;

		baseDamage = gg.rnd.Next<int32_t>(1, 10);
	}

	void PlayerWeapon::Update() {
		// 同步 owner 坐标
		pos = owner->pos;
		y = pos.y + 1.f;

		// todo: shoot
	}

	void PlayerWeapon::Draw() {
		auto& f = gg.pics.sword1;
		gg.Quad().DrawFrame(f, scene->cam.ToGLPos(pos)
			, cWidth / 7.f * scene->cam.scale, radians);
	}

	void PlayerWeapon::DrawLight() {
		auto& f = gg.pics.light_sword1;
		gg.Quad().DrawFrame(f, scene->cam.ToGLPos(pos)
			, cWidth / 7.f * scene->cam.scale, radians);
	}

	void PlayerWeapon::Dispose() {
		assert(scene);
		assert(!disposing);

		// 设置标记
		disposing = true;

		// 自杀
		owner->weapon.Reset();
	}

}
