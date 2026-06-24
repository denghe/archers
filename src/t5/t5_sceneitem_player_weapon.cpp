#include "pch.h"
#include "t5.h"

namespace Test5 {

	void PlayerWeapon::Init(Player* owner_) {
		typeId = cTypeId;
		scene = owner_->scene;
		owner = xx::WeakFromThis(owner_);

		pos = owner_->pos;
		y = pos.y;
		radius = 16.f;
		scale = 1.f;
		radians = CalcRadians();

		baseDamage = gg.rnd.Next<int32_t>(1, 10);
	}

	float PlayerWeapon::CalcRadians() {
		return owner->radians - (gPI_2 + gPI_4);	// gPI_4: 刀图默认斜 45
	}

	void PlayerWeapon::AnimSwing() {
		// 

		//XX_BEGIN(_1);
		//assert(swingStepCount == 0);
		//for(swingStepCount = cCount; swingStepCount > 0; --swingStepCount) {
		//	radians += cRotateFrameStep * swingDirection;
		//	FillCircles();
		//	XX_YIELD(_1);
		//}
		//swingDirection = -swingDirection;
		//XX_YIELD(_1);
		//_1 = 0;
		//XX_END(_1);
	}

	void PlayerWeapon::Update() {
		// 同步 owner 坐标
		pos = owner->pos;
		y = pos.y + 1.f;

		//// 挥刀
		//if (IsSwinging()) {
		//	// 正在挥刀? 继续
		//	AnimSwing();
		//}
		//else {
		//	//// 不在挥刀? 则保持和 owner 同步
		//	//radians = owner->radians - (cRadiansRange * 0.5f) * swingDirection;
		//}
	}

	void PlayerWeapon::Draw() {
		auto& f = gg.pics.sword1;
		gg.Quad().DrawFrame(f, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians);
	}

	void PlayerWeapon::DrawLight() {
		auto& f = gg.pics.light_sword1;
		gg.Quad().DrawFrame(f, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians);
	}

	void PlayerWeapon::Dispose() {
		assert(scene);
		assert(!disposing);

		// 设置标记
		disposing = true;
	}

}
