#include "pch.h"
#include "t1_.h"

namespace Test1 {

	void Scene::HandleWallsCross(XY& pos_, float radius_) {
		// 遍历 item 邻居建筑处理碰撞( 直接修改 pos_ 位置将其移到建筑范围外 )
		using G = decltype(gridWalls);
		auto& g = gridWalls;
		auto cri = g.PosToCRIndex(pos_);
		g.ForeachBy9(cri.y, cri.x, [&](G::Node& node, float range)->void {
			auto d = pos_ - node.cache.pos;
			auto mag2 = d.x * d.x + d.y * d.y;
			auto r = node.cache.radius + radius_;
			auto rr = r * r;
			// 相交 但没有完全重叠
			if (mag2 < rr && mag2 > 0.0001f) {
				auto mag = std::sqrtf(mag2);
				auto norm = d / mag;
				pos_ = node.cache.pos + norm * r;
			}
		});
	}

	Lava* Scene::TryGetCrossLava(XY const& pos_, float radius_) {
		// 遍历 主门 判断 pos_ 是否离中心点足够近
		using G = decltype(gridLavas);
		auto& g = gridLavas;
		auto cri = g.PosToCRIndex(pos_);
		Lava* rtv{};
		g.ForeachBy9Break(cri.y, cri.x, [&](G::Node& node, float range)->bool {
			auto d = pos_ - node.cache.pos;
			auto mag2 = d.x * d.x + d.y * d.y;
			auto r = node.cache.radius * 0.5f + radius_;
			auto rr = r * r;
			// 如果 pos_ 足够接近圆心
			if (mag2 < rr) {
				rtv = (Lava*)node.value;
				return true;
			}
			return false;
		});
		return rtv;
	}
}
