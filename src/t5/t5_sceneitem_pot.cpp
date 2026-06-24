#include "pch.h"
#include "t5.h"

namespace Test5 {

	void Pot::Init(Scene* scene_, XYi cr_) {
		typeId = cTypeId;
		scene = scene_;
		radius = cCellPixelSize * 0.5f;
		scale = radius * 2.f / gg.pics.frog_pot_[0].uvRect.w;
		radians = {};
		// 坐标位于格子 下面中间 ( 罐子的 pivot 为下中 )
		auto p = cr_ * cCellPixelSize + XY{ cCellPixelSize * 0.5f, cCellPixelSize - 1 };
		pos = p;
		y = p.y;

		// todo: 注册到空间索引
	}

	void Pot::Update() {
	}

	void Pot::Draw() {
		auto& f = gg.pics.frog_pot_[0];
		gg.Quad().DrawFrame(f, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale);// , radians - gPI_2);
	}

	void Pot::DrawLight() {
		gg.Quad().DrawFrame(gg.pics.c64_light, scene->cam.ToGLPos(pos)
			, (256.f / 64.f) * scene->cam.scale, 0, 0.5f);
	}

	void Pot::DrawShadow() {
		gg.Quad().DrawFrame(gg.pics.c64_shadow, scene->cam.ToGLPos(pos)
			, (radius * (1.f / 40.f)) * scene->cam.scale, 0, 1.f, { 180,180,180,180 });
	}

	void Pot::Dispose() {
		assert(scene);
		assert(!disposing);
		assert(indexAtContainer != -1);

		// 设置标记
		disposing = true;

		// 析构
		scene->pots.Remove(this);

		// todo: 从空间索引中移除
	}

}
