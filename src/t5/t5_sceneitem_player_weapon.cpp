#include "pch.h"
#include "t5.h"

namespace Test5 {

	void PlayerWeapon::Init(Player* owner_) {
		typeId = cTypeId;
		scene = owner_->scene;
		owner = xx::WeakFromThis(owner_);

		pos = CalcPos();
		y = pos.y;

		radius = 16.f;	// unused
		scale = 2.f;
		radians = CalcRadians();

		baseDamage = gg.rnd.Next<int32_t>(1, 10);
	}

	float PlayerWeapon::CalcRadians() {
		return gPI + owner->direction - gPI * 0.35f;
	}

	XY PlayerWeapon::CalcPos() {
		return owner->pos + XY{ 0, -30 };
	}

	void PlayerWeapon::Swing() {
		static constexpr float cSwingBeginDuration{ 0.1f };
		static constexpr float cSwingDuration{ 0.05f };
		static constexpr float cSwingEndDuration{ 0.2f };
		static constexpr float cSwingBeginSteps{ int32_t(cSwingBeginDuration * gg.cFps) };
		static constexpr float cSwingEndSteps{ int32_t(cSwingEndDuration * gg.cFps) };

		static constexpr float cSwingSteps{ int32_t(cSwingDuration * gg.cFps) };
		static constexpr float cSwingAngleSteps{ gPI * 0.7f / int32_t(cSwingDuration * gg.cFps) };

		// 挥刀
		// 1，先把刀角度旋转到出刀前角度
		// 2，把刀改成挥到位后的角度，并创建一个扇形的刀气( 独立的子弹 )
		XX_BEGIN(_1);
		radians = CalcRadians();
		for (_i = 0; _i < cSwingBeginSteps; ++_i) {
			XX_YIELD(_1);
		}
		// todo: 创建刀气 by owner->direction
		for (_i = 0; _i < cSwingSteps; ++_i) {
			radians += cSwingAngleSteps;
			XX_YIELD(_1);
		}
		for (_i = 0; _i < cSwingEndSteps; ++_i) {
			XX_YIELD(_1);
		}
		_1 = 0;	// reset coroutine
		XX_END(_1);
	}

	bool PlayerWeapon::IsSwinging() const {
		return _1 > 0;
	}

	void PlayerWeapon::Update() {
		// 同步 owner 坐标
		pos = CalcPos();
		y = pos.y + 1.f;

		targetRadians = CalcRadians();
		// todo
		// 攻击 cd 计算
		// 攻击目标查找
		// 如果当前没有攻击目标，且 cd 已到，则面朝鼠标挥刀

		// 挥刀
		if (IsSwinging()) {
			// 正在挥刀? 继续
			Swing();
		}
		else {
			// 刀按照某角速度限制 慢慢旋转到出刀前角度
			xx::AngleStep(radians, targetRadians, gPI * 0.02f);
		}

		if (gg.mouse[GLFW_MOUSE_BUTTON_1](0.2f)) {
			if (!IsSwinging()) {
				Swing();
			}
		}
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
