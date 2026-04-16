#include "pch.h"
#include "t1_.h"

namespace Test1 {

	void Archer::Init(Scene* scene_, XY pos_) {
		typeId = cTypeId;
		scene = scene_;
		pos = pos_;
		y = pos.y;
		radius = cPlayerRadius;
		scale = radius * 2.f / gg.pics.c128_player.uvRect.w;
		radians = {};

		indexAtContainer = scene->archers.len - 1;
		assert(scene->archers[indexAtContainer].pointer == this);

		damage = 1;
	}

	void Archer::Update() {
		if (scene->time >= nextShootTime) {
			nextShootTime = scene->time + cShootInterval;
			scene->archerArrows.Emplace().Emplace()->Init(xx::WeakFromThis(this));
		}
	}

	void Archer::Draw() {
		gg.Quad().DrawFrame(gg.pics.c128_player, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians);
	}

	void Archer::DrawLight() {
		gg.Quad().DrawFrame(gg.pics.c64_light, scene->cam.ToGLPos(pos)
			, (2048.f / 64.f) * scene->cam.scale);
	}

	void Archer::Dispose() {
		assert(scene);
		assert(!disposing);
		assert(indexAtContainer != -1);
		auto& container = scene->archers;
		assert(container[indexAtContainer].pointer == this);

		// 设置标记
		disposing = true;

		// 从容器中移除对象( 释放内存 )
		auto i = indexAtContainer;
		container.Back()->indexAtContainer = i;
		indexAtContainer = -1;
		container.SwapRemoveAt(i);
	}

}
