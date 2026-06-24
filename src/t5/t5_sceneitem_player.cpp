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

		InitYOffset();
		AnimInit();
		bounceHeightMax = 20.f;
		bounceHalfDuration = 1.f / 10.f;

		// 初始化数据面板
		healthMaxDefault = 100.f;
		PropsInit();
		PropsCalc();
	}


	void Player::InitYOffset() {
		auto& frame = gg.pics.frog_char;
		auto frameHeight = (float)frame.uvRect.h;
		auto centerHeight = frameHeight * 0.5f;
		auto pivotHeight = frameHeight * frame.anchor.y;
		pcDiff = centerHeight - pivotHeight;
	}

	void Player::UpdateYOffset() {
		yOffset = pcDiff + bounceHeight;
	}

	void Player::AnimInit() {
		radians = 0;
		radiansTarget = 0;
		bounceHeight = 0;
		bouncing = {};
		UpdateYOffset();
	}

	// jump + rotate anim
	void Player::AnimBounceRotate() {
		static constexpr float bounceRadiansTarget1{ 25.f / 180.f };
		static constexpr float bounceRadiansTarget2{ -35.f / 180.f };
		assert(bounceHeightMax > 0);
		float bounceStepNums{ gg.cFps * bounceHalfDuration };
		float bounceStepSpeed{ bounceHeightMax / bounceStepNums };
		float bounceStepSpeedMax{ bounceStepSpeed * 2.f };
		float bounceStepSpeedDecrease{ bounceStepSpeedMax / bounceStepNums };

		XX_BEGIN(_2);
		AnimInit();
	LabLoop:
		// jump up
		bouncing = true;
		bounceInc = bounceStepSpeedMax;
		radians = radiansTarget;
		radiansTarget = bounceRadiansTarget1;
		radiansStep = (radiansTarget - radians) / bounceStepNums;
		do {
			bounceHeight -= bounceInc;
			bounceInc -= bounceStepSpeedDecrease;
			UpdateYOffset();
			radians += radiansStep;
			XX_YIELD(_2);
		} while (bounceInc >= 0);
		bounceHeight = -bounceHeightMax;
		UpdateYOffset();
		XX_YIELD(_2);

		// falling
		bounceInc = 0;
		radians = radiansTarget;
		do {
			bounceHeight += bounceInc;
			bounceInc += bounceStepSpeedDecrease;
			UpdateYOffset();
			XX_YIELD(_2);
		} while (bounceInc < bounceStepSpeedMax);
		assert(std::fabs(bounceHeight) < bounceInc);

		// for anim end notice
		bouncing = false;
		XX_YIELD(_2);

		// jump up
		bouncing = true;
		bounceInc = bounceStepSpeedMax;
		radians = radiansTarget;
		radiansTarget = bounceRadiansTarget2;
		radiansStep = (radiansTarget - radians) / bounceStepNums;
		do {
			bounceHeight -= bounceInc;
			bounceInc -= bounceStepSpeedDecrease;
			UpdateYOffset();
			radians += radiansStep;
			XX_YIELD(_2);
		} while (bounceInc >= 0);
		bounceHeight = -bounceHeightMax;
		UpdateYOffset();
		XX_YIELD(_2);

		// falling
		bounceInc = 0;
		radians = radiansTarget;
		do {
			bounceHeight += bounceInc;
			bounceInc += bounceStepSpeedDecrease;
			UpdateYOffset();
			XX_YIELD(_2);
		} while (bounceInc < bounceStepSpeedMax);
		assert(std::fabs(bounceHeight) < bounceInc);
		bounceHeight = 0;
		UpdateYOffset();
		XX_YIELD(_2);

		// for anim end notice
		bouncing = false;
		XX_YIELD(_2);

		// loop
		goto LabLoop;
		XX_END(_2);
	}

	void Player::Anim() {
#if 0
		XX_BEGIN(_1);
		while (true) {
			// bounce + rotate
			if (moving || bouncing) {
				AnimBounceRotate();
				XX_YIELD(_1);
			}

			AnimInit();	// reset anim
			XX_YIELD(_1);
		}
		XX_END(_1);
#else
		AnimBounceRotate();
#endif
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
	}

	void Player::Draw() {
		auto& f = gg.pics.frog_char;

		XY p{ pos.x, pos.y + yOffset };
		XY anchor{ f.anchor.x, 0.5f };
		XY s;
		s.y = scale;
		if (flipX) s.x = scale;
		else s.x = -scale;
		gg.Quad().DrawFrame(f, scene->cam.ToGLPos(p), s * scene->cam.scale, radians);
	}

	void Player::DrawLight() {
		gg.Quad().DrawFrame(gg.pics.c64_light, scene->cam.ToGLPos(pos)
			, (1024.f / cCellPixelSize) * scene->cam.scale, 0, 1.f);
	}

	void Player::DrawShadow() {
		gg.Quad().DrawFrame(gg.pics.c64_shadow, scene->cam.ToGLPos(pos)
			, (radius * (1.f / 40.f)) * scene->cam.scale, 0, 1.f, { 180,180,180,180 });
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
