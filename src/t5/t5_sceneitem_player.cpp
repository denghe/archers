#include "pch.h"
#include "t5.h"

namespace Test5 {

	void Player::SetPos(XY pos_) {
		// 没有移动? 直接返回
		if (pos == pos_) {
			moving = false;
			return;
		}
		pos = pos_;
		y = pos.y;
		moving = true;

		// 和上一帧坐标一样? 就不用算角度了
		if (pos == lastPos) return;

		// 通过上一帧坐标当前坐标差值矢量得到角度，进而判断角度区间推算方向:
		auto d = lastPos - pos;
		if (d.x > 0) flipX = false;
		else if (d.x < 0) flipX = true;
		lastPos = pos;
		direction = std::atan2(d.y, d.x);	// PI ~ -PI
	}

	void Player::Init(Scene* scene_, XYi cr_) {
		typeId = cTypeId;
		scene = scene_;
		radius = cCellPixelSize * 0.5f;
		scale = radius * 2.f / gg.pics.frog_char.uvRect.w;
		radians = {};
		// 坐标位于格子 下面中间 ( 怪的 pivot 为下中 )
		auto p = cr_ * cCellPixelSize + XY{ cCellPixelSize * 0.5f, cCellPixelSize - 1 };
		// 模拟一下从上走下来，正面朝下
		lastPos.x = pos.x;
		lastPos.y = pos.y - 1;
		SetPos(p);

		// 初始化数据面板
		healthMaxDefault = 100.f;
		PropsInit();
		PropsCalc();

		// 创建挂接的武器
		weapons.Emplace().Emplace<PlayerWeapon>()->Init(this);
	}

	void Player::AnimBounceRotate() {
		static constexpr float cBounceRadiansTarget{ 30.f / 180.f };
		static constexpr float cBounceHeightMax{ 20.f };
		static constexpr float cBounceHalfDuration{ 1.f / 10.f };
		static constexpr float cBounceStepNums{ gg.cFps * cBounceHalfDuration };
		static constexpr float cBounceStepSpeed{ cBounceHeightMax / cBounceStepNums };
		static constexpr float cBounceStepSpeedMax{ cBounceStepSpeed * 2.f };
		static constexpr float cBounceStepSpeedDecrease{ cBounceStepSpeedMax / cBounceStepNums };

		XX_BEGIN(_2);
		bounceRadiansTarget = cBounceRadiansTarget;
	LabLoop:
		// 跳起并旋转
		bouncing = true;
		bounceInc = cBounceStepSpeedMax;
		bounceHeight = 0;
		radians = bounceRadiansTarget;
		bounceRadiansTarget = -bounceRadiansTarget;	// 反转目标角度
		bounceRadiansStep = (bounceRadiansTarget - radians) / cBounceStepNums;
		do {
			bounceHeight -= bounceInc;
			bounceInc -= cBounceStepSpeedDecrease;
			radians += bounceRadiansStep;
			XX_YIELD(_2);
		} while (bounceInc >= 0);
		bounceHeight = -cBounceHeightMax;
		XX_YIELD(_2);

		// 维持角度落地
		bounceInc = 0;
		radians = bounceRadiansTarget;
		do {
			bounceHeight += bounceInc;
			bounceInc += cBounceStepSpeedDecrease;
			XX_YIELD(_2);
		} while (bounceInc < cBounceStepSpeedMax);
		assert(std::fabs(bounceHeight) < bounceInc);
		bounceHeight = 0;

		// 落地后暂停 1 帧并通知外界，方便停止
		bouncing = false;
		XX_YIELD(_2);

		// 继续
		goto LabLoop;
		XX_END(_2);
	}

	void Player::Anim() {
		static constexpr float cRestoreRadiansDuration{ 0.05f };
		static constexpr int32_t cRestoreRadiansDurationFrames{ int32_t(gg.cFps * cRestoreRadiansDuration) };

		XX_BEGIN(_1);
	LabBegin:
		// 等待移动指令
		while (!moving) {
			XX_YIELD(_1);
		}

	LabBounce:
		// 起跳
		AnimBounceRotate();
		XX_YIELD(_1);

		// 等待下落
		while (bouncing) {
			AnimBounceRotate();
			XX_YIELD(_1);
		}

		// 如果还在移动中，继续下一次跳跃
		if (moving) goto LabBounce;

		// 落地后让角色角度插值旋转到 0
		bounceRadiansStep = radians / cRestoreRadiansDurationFrames;	// 借用一下
		for (_i = 0; _i < cRestoreRadiansDurationFrames; ++_i) {
			radians -= bounceRadiansStep;
			XX_YIELD(_1);
		}
		radians = 0;

		// 回到开头等待指令
		goto LabBegin;
		XX_END(_1);
	}

	void Player::Update() {
		// 简单的让坐标等同于鼠标位置，方便看效果
		auto mp = scene->cam.ToLogicPos(gg.mousePos);
		if (mp.x < cCellPixelSize) mp.x = cCellPixelSize;
		else if (mp.x >= scene->mapPixelSize.x - cCellPixelSize) mp.x = scene->mapPixelSize.x - cCellPixelSize - 0.001f;
		if (mp.y < cCellPixelSize) mp.y = cCellPixelSize;
		else if (mp.y >= scene->mapPixelSize.y - cCellPixelSize) mp.y = scene->mapPixelSize.y - cCellPixelSize - 0.001f;

		SetPos(mp);
		Anim();

		// 更新挂接的武器
		SceneBase::UpdateItems(weapons);
	}

	void Player::Draw() {
		auto& f = gg.pics.frog_char;

		XY p{ pos.x, pos.y + bounceHeight };

		XY s;
		s.y = scale;
		if (flipX) s.x = scale;
		else s.x = -scale;

		gg.Quad().DrawFrame(f, scene->cam.ToGLPos(p), s * scene->cam.scale, radians);

		// 绘制挂接的武器
		for (auto& o : weapons) {
			o->Draw();
		}
	}

	void Player::DrawLight() {
#if 1
		gg.Quad().DrawFrame(gg.pics.c64_light, scene->cam.ToGLPos(pos)
			, (1024.f / cCellPixelSize) * scene->cam.scale, 0, 0.7f);
#endif

		// 绘制挂接的武器
		for (auto& o : weapons) {
			o->DrawLight();
		}
	}

	void Player::DrawShadow() {
		gg.Quad().DrawFrame(gg.pics.c64_shadow, scene->cam.ToGLPos(pos)
			, (radius * (1.f / 40.f)) * scene->cam.scale, 0, 1.f, { 180,180,180,180 });

		// 绘制挂接的武器
		for (auto& o : weapons) {
			o->DrawShadow();
		}
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
