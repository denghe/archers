#include "pch.h"
#include "t4.h"

namespace Test4 {

	void Player::SetPos(XY pos_) {
		pos = pos_;
		y = pos.y;
	}

	void Player::Init(Scene* scene_, XYi cr_) {
		typeId = cTypeId;
		scene = scene_;
		// 坐标位于格子 下面中间 ( 怪的 pivot 为下中 )
		pos = cr_ * cCellPixelSize + XY{ cCellPixelSize*0.5f, cCellPixelSize-1 };
		y = pos.y;
		radius = cCellPixelSize;// cCreatureRadius;
		scale = radius * 2.f / gg.pics.slime1_idle_[0].uvRect.w;
		radians = {};

		// 初始化数据面板
		healthMaxDefault = 100.f;
		PropsInit();
		PropsCalc();
	}

	void Player::Update() {
		// 简单的让坐标等同于鼠标位置，方便看效果
		auto mp = scene->cam.ToLogicPos(gg.mousePos);
		if (mp.x < cCellPixelSize) mp.x = cCellPixelSize;
		else if (mp.x >= scene->mapPixelSize.x - cCellPixelSize) mp.x = scene->mapPixelSize.x - cCellPixelSize - 0.001f;
		if (mp.y < cCellPixelSize) mp.y = cCellPixelSize;
		else if (mp.y >= scene->mapPixelSize.y - cCellPixelSize) mp.y = scene->mapPixelSize.y - cCellPixelSize - 0.001f;
		SetPos(mp);

		// 怪的身高不可以高过 墙正面根 到 shadow mask 边缘的距离, 否则会导致 mask 覆盖到怪身上
		if (gg.mouse[GLFW_MOUSE_BUTTON_LAST + 1](0.2f)) {
			scale *= 1.05f;
		}
		if (gg.mouse[GLFW_MOUSE_BUTTON_LAST + 2](0.2f)) {
			scale *= 0.95f;
		}

		// todo: 4向移动，并设置相应的动画段
		
		// 步进动画
		frameIndex += cFrameIndexStep;
		if (frameIndex >= cFrameIndexMax) {
			frameIndex -= cFrameIndexMax;
		}
	}

	void Player::Draw() {
		auto& f = gg.pics.slime1_idle_[(int32_t)frameIndex];
		gg.Quad().DrawFrame(f, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians);
	}

	void Player::DrawLight() {
		gg.Quad().DrawFrame(gg.pics.c64_light, scene->cam.ToGLPos(pos)
			, (256.f / 64.f) * scene->cam.scale, 0, 0.5f);
	}

	void Player::DrawShadow() {
		// todo
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
