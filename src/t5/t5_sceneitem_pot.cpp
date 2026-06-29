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

		indexAtContainer = scene_->pots.len - 1;
		assert(scene_->pots[indexAtContainer].pointer == this);

		scene_->gridPots.Add(indexAtGrid, this);
	}

	void Pot::Update() {
	}

	std::pair<float, int> Pot::Hurt(float attackValue_) {
		//auto r = PropsDoHurt(gg.rnd, attackValue_);
		//if (r.second == 0) {
		//	// 变白
			whiteColorEndTime = scene->time + cWhiteColorDuration;
		//}
		//else if (r.second == 2) {
		//	// 爆炸特效
		//	scene->exploders.Emplace().Emplace()->Init(this);
		//	// 自杀
		//	Dispose();
		//}
		//return r;
			return {};
	}

	void Pot::Draw() {
		auto& f = gg.pics.frog_pot_[0];
		float cp{ 1 };
		if (scene->time < whiteColorEndTime) cp = 10000.f;
		gg.Quad().DrawFrame(f, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians, cp);
	}

	void Pot::DrawLight() {
		gg.Quad().DrawFrame(gg.pics.c64_light, scene->cam.ToGLPos(pos)
			, (256.f / 64.f) * scene->cam.scale, 0, 0.5f);
	}

	void Pot::DrawShadow() {
		gg.Quad().DrawFrame(gg.pics.c64_shadow, scene->cam.ToGLPos(pos)
			, (radius * (1.f / 40.f)) * scene->cam.scale, 0, 1.f, { 180,180,180,180 });
	}

	void Pot::DrawGizmos() {
		auto p = scene->gridPots.NodeAt(indexAtGrid).cache.pos;
		gg.Line().DrawCircle(scene->cam.ToGLPos(p), radius * scene->cam.scale, radians, 16);
	}

	void Pot::Dispose() {
		assert(scene);
		assert(!disposing);
		assert(indexAtContainer != -1);
		auto& container = scene->pots;
		assert(container[indexAtContainer].pointer == this);

		// 设置标记
		disposing = true;

		// 进一步释放资源
		if (indexAtGrid > -1) {
			scene->gridPots.Remove(indexAtGrid, this);
		}

		// 从容器中移除对象( 释放内存 )
		auto i = indexAtContainer;
		container.Back()->indexAtContainer = i;
		indexAtContainer = -1;
		container.SwapRemoveAt(i);
	}

}
