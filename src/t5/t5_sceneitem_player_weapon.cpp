#include "pch.h"
#include "t5.h"

namespace Test5 {

	void PlayerWeapon::Init(Player* owner_) {
		typeId = cTypeId;
		scene = owner_->scene;
		owner = xx::WeakFromThis(owner_);

		pos = CalcPos();
		y = pos.y;

		radius = 32.f;	// for draw gizmos only
		scale = 2.f;
		radians = CalcRadians();

		baseDamage = gg.rnd.Next<int32_t>(1, 10);
	}

	float PlayerWeapon::CalcRadians() {
		return owner->direction - gPI * 0.35f;
	}

	XY PlayerWeapon::CalcPos() {
		return owner->pos + XY{ 0, -30 };
	}

	void PlayerWeapon::SwingBegin() {
		assert(canBreakSwing);
		_1 = 0;
		Swing();
	}

	void PlayerWeapon::Swing() {
		static constexpr float cSwingBeginDuration{ 0.02f };
		static constexpr float cSwingDuration{ 0.03f };
		static constexpr float cSwingEndDuration{ 0.05f };
		static constexpr int32_t cSwingBeginSteps{ int32_t(cSwingBeginDuration * gg.cFps) };
		static constexpr int32_t cSwingSteps{ int32_t(cSwingDuration * gg.cFps) };
		static constexpr int32_t cSwingEndSteps{ int32_t(cSwingEndDuration * gg.cFps) };
		static constexpr float cSwingAngleSteps{ gPI * 0.7f / int32_t(cSwingDuration * gg.cFps) };

		XX_BEGIN(_1);
		// 保持挥刀前的角度一小段时间
		canBreakSwing = false;
		radians = CalcRadians();
		for (_i = 0; _i < cSwingBeginSteps; ++_i) {
			XX_YIELD(_1);
		}

		// 创建刀气
		scene->playerBullets.Emplace().Emplace<PlayerBullet>()->Init(this);

		// 角色开始下蹲
		owner->SquatBegin();

		// 顺时针挥刀
		for (_i = 0; _i < cSwingSteps; ++_i) {
			radians += cSwingAngleSteps;
			XX_YIELD(_1);
		}

		// 刀停在挥完的角度一阵子( 可被打断 )
		canBreakSwing = true;
		for (_i = 0; _i < cSwingEndSteps; ++_i) {
			XX_YIELD(_1);
		}

		// 重置协程行号
		_1 = 0;
		XX_END(_1);
	}

	int32_t PlayerWeapon::IsSwinging() const {
		return _1 > 0;
	}

	void PlayerWeapon::Update() {
		// 同步 owner 坐标
		pos = CalcPos();
		y = pos.y + 1.f;

		targetRadians = CalcRadians();
		// todo
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

		// 攻击 cd 计算
		if (gg.mouse[GLFW_MOUSE_BUTTON_1] && canBreakSwing) {
			if (nextShootTime <= gg.time) {
				nextShootTime = gg.time + shootDelay;
				SwingBegin();
			}
		}
		// 攻击不计算 cd ( 挥刀有无法打断的部分时长 )
		if (gg.mouse[GLFW_MOUSE_BUTTON_2] && canBreakSwing) {
			SwingBegin();
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

	void PlayerWeapon::DrawGizmos() {
		gg.Line().DrawCircle(scene->cam.ToGLPos(pos), radius * scene->cam.scale, radians, 16);
	}

	void PlayerWeapon::Dispose() {
		assert(scene);
		assert(!disposing);

		// 设置标记
		disposing = true;
	}

}
