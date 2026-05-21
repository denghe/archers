#include "pch.h"
#include "t4.h"

namespace Test4 {

	void Player::StepAnim() {
		// 步进动画
		frameIndex += frameIndexStep;
		if (frameIndex >= numFrames) {
			frameIndex -= numFrames;
		}
	}

	void Player::FillFrames() {
		static constexpr int32_t cNumIdleFrames{ gg.pics.slime1_idle_.size() / 4 };
		static constexpr int32_t cNumMoveFrames{ gg.pics.slime1_move_.size() / 4 };
		static constexpr int32_t cNumAttackFrames{ gg.pics.slime1_attack_.size() / 4 };

		static constexpr float cIdleFrameStep{ cNumIdleFrames / (gg.cFps * 0.5f) };
		static constexpr float cMoveFrameStep{ cNumMoveFrames / (gg.cFps * 0.5f) };
		static constexpr float cAttackFrameStep{ cNumAttackFrames / (gg.cFps * 0.5f) };

		// 重置动画播放进度
		frameIndex = 0;

		// 根据 md 填充 frames, numFrames
		switch (actionMode) {
		case ActionModes::Idle:
			frames = &gg.pics.slime1_idle_[cNumIdleFrames * frameDirection];
			numFrames = cNumIdleFrames;
			frameIndexStep = cIdleFrameStep;
			break;
		case ActionModes::Move:
			frames = &gg.pics.slime1_move_[cNumMoveFrames * frameDirection];
			numFrames = cNumMoveFrames;
			frameIndexStep = cMoveFrameStep;
			break;
		case ActionModes::Attack:
			frames = &gg.pics.slime1_attack_[cNumAttackFrames * frameDirection];
			numFrames = cNumAttackFrames;
			frameIndexStep = cAttackFrameStep;
			break;
		default:
			assert(false);
		}
	}

	void Player::SetPos(XY pos_) {
		if (pos == pos_) return;
		pos = pos_;
		y = pos.y;
		// 没有移动? 直接返回
		if (pos == lastPos) return;

		// 通过上一帧坐标当前坐标差值矢量得到角度，进而判断角度区间推算方向:
		auto d = lastPos - pos;
		lastPos = pos;
		auto r = std::atan2(d.y, d.x);	// PI ~ -PI
		static constexpr float pi14{ gPI / 4 };
		static constexpr float pi34{ gPI / 4 * 3 };

		// 当前史莱姆资源 4向顺序为 下:0  上:1  右:2  左:3
		int32_t md{ -1 };
		if (r >= 0) {
			if (r < pi14) md = 2;	// 右
			else if (r < pi34) md = 1;	// 上
			else md = 3;	// 左
		}
		else {
			if (r > -pi14) md = 2;	// 右
			else if (r > -pi34) md = 0;	// 下
			else md = 3;	// 左
		}

		// 朝向没变? 直接返回
		if (frameDirection == md) return;

		frameDirection = md;
		FillFrames();
	}

	void Player::Init(Scene* scene_, XYi cr_) {
		typeId = cTypeId;
		scene = scene_;
		radius = cCellPixelSize;// cCreatureRadius;
		scale = radius * 2.f / gg.pics.slime1_idle_[0].uvRect.w;
		radians = {};
		// 坐标位于格子 下面中间 ( 怪的 pivot 为下中 )
		auto p = cr_ * cCellPixelSize + XY{ cCellPixelSize * 0.5f, cCellPixelSize - 1 };
		// 模拟一下从上走下来，正面朝下
		lastPos.x = pos.x;
		lastPos.y = pos.y - 1;
		actionMode = ActionModes::Move;
		SetPos(p);

		// 初始化数据面板
		healthMaxDefault = 100.f;
		PropsInit();
		PropsCalc();
	}

	void Player::Update() {
		// 步进动画
		StepAnim();

		// 简单的让坐标等同于鼠标位置，方便看效果
		auto mp = scene->cam.ToLogicPos(gg.mousePos);
		if (mp.x < cCellPixelSize) mp.x = cCellPixelSize;
		else if (mp.x >= scene->mapPixelSize.x - cCellPixelSize) mp.x = scene->mapPixelSize.x - cCellPixelSize - 0.001f;
		if (mp.y < cCellPixelSize) mp.y = cCellPixelSize;
		else if (mp.y >= scene->mapPixelSize.y - cCellPixelSize) mp.y = scene->mapPixelSize.y - cCellPixelSize - 0.001f;
		SetPos(mp);

		// 鼠标滚轮缩放
		// 怪的身高不可以高过 墙正面根 到 shadow mask 边缘的距离, 否则会导致 mask 覆盖到怪身上
		if (gg.mouse[GLFW_MOUSE_BUTTON_LAST + 1](0.2f)) {
			scale *= 1.02f;
		}
		if (gg.mouse[GLFW_MOUSE_BUTTON_LAST + 2](0.2f)) {
			scale *= 0.98f;
		}
	}

	void Player::Draw() {
		auto& f = frames[(int32_t)frameIndex];
		gg.Quad().DrawFrame(f, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians);
	}

	void Player::DrawLight() {
		gg.Quad().DrawFrame(gg.pics.c64_light, scene->cam.ToGLPos(pos)
			, (256.f / 64.f) * scene->cam.scale, 0, 0.5f);
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
