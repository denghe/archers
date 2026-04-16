#include "pch.h"
#include "t1_.h"

namespace Test1 {

	void ArcherArrow::Init(xx::Weak<Archer> owner_) {
		typeId = cTypeId;
		scene = owner_->scene;
		owner = std::move(owner_);

		// todo: 找最近怪作为目标

		auto pp = owner->pos;
		auto cosSin = XY{};// todo
		auto pr = owner->radius;
		// 起始坐标：从玩家中心点出发，前进玩家半径的距离
		pos = pp + cosSin * pr;
		y = pos.y;
		radius = 16.f;
		scale = radius * 2.f / gg.pics.c32_bullet.uvRect.w;
		radians = {};

		indexAtContainer = scene->archerArrows.len - 1;
		assert(scene->archerArrows[indexAtContainer].pointer == this);

		// todo: 模拟抛物线
		// 算出每帧的步进
		inc = cosSin * cSpeed * gg.cDelta;
		deathTime = scene->time + cMaxLifetime;

		damage = owner->damage;
	}

	void ArcherArrow::Update() {
		if (scene->time >= deathTime) {
			Dispose();
			return;
		}

		pos += inc;
		y = pos.y;

		// todo: 因为是锁定怪物的模式，所以只有当 arrow 落地后才判定，并且判定只是看指针是否未失效
	}

	void ArcherArrow::Draw() {
		gg.Quad().DrawFrame(gg.pics.c32_bullet, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians);
	}

	void ArcherArrow::DrawLight() {
		gg.Quad().DrawFrame(gg.pics.c64_light, scene->cam.ToGLPos(pos)
			, (256.f / 64.f) * scene->cam.scale);
	}

	void ArcherArrow::Dispose() {
		assert(scene);
		assert(!disposing);
		assert(indexAtContainer != -1);
		auto& container = scene->archerArrows;
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
