#include "pch.h"
#include "t1_.h"

namespace Test1 {

	void ArcherArrow::Init(Archer* owner_, Monster* tar_) {
		typeId = cTypeId;
		scene = owner_->scene;
		owner = xx::WeakFromThis(owner_);
		target = xx::WeakFromThis(tar_);

		pos = owner_->pos;
		y = pos.y;
		radius = 16.f;
		scale = radius * 2.f / gg.pics.firearrow_[0].uvRect.w;
		radians = {};

		indexAtContainer = scene->archerArrows.len - 1;
		assert(scene->archerArrows[indexAtContainer].pointer == this);

		// 算出每帧的步进
		auto d = tar_->pos - owner_->pos;
		auto mag2 = d.x * d.x + d.y * d.y;
		auto _1mag = 1.f / std::sqrtf(mag2);
		inc = d * _1mag * cSpeed * gg.cDelta;
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

		bool needDispose{};

		// 查找子弹位置的 bucket
		auto cri = scene->physMonsters->PosToCRIndex(pos);
		scene->physMonsters->ForeachBy9Break(cri.y, cri.x, [&](PhysSystem::Node& o, float range)->bool {
			// 开始碰撞判定
			auto d = o.cache.pos - pos;
			auto mag2 = d.x * d.x + d.y * d.y;
			auto r = o.cache.radius + radius;
			auto rr = r * r;
			// 距离小于圆心和: 相交
			if (mag2 < rr) {
				// 伤害目标	// todo
				((Monster*)o.value)->Dispose();
				needDispose = true;
				return true;
			}
			return false;
		});
		if (needDispose) {
			// todo: 弄点子弹命中怪消失的粒子？
			Dispose();
			return;
		}

		// 查找子弹位置的建筑. 如果有相交，子弹自杀
		using G = decltype(scene->gridWalls);
		auto& g = scene->gridWalls;
		cri = g.PosToCRIndex(pos);
		needDispose = g.ForeachBy9Break(cri.y, cri.x, [&](G::Node& node, float range)->bool {
			auto d = pos - node.cache.pos;
			auto mag2 = d.x * d.x + d.y * d.y;
			auto r = node.cache.radius + radius;
			auto rr = r * r;
			return mag2 < rr;
		});
		if (needDispose) {
			// todo: 弄点子弹命中建筑消失的粒子？
			Dispose();
			return;
		}
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
