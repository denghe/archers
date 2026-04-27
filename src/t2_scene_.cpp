#include "pch.h"
#include "t2.h"

namespace Test2 {

	void Scene::GenCreature(int32_t campIndex_, int32_t count_) {
		//auto& ps = archerPixelPosPool;
		//if (count_ > ps.len) count_ = ps.len;
		//for (int32_t i = 0; i < count_; i++) {
		//	auto idx = gg.rnd.Next(ps.len);
		//	archers.Emplace().Emplace()->Init(this, ps[idx]);
		//	ps.SwapRemoveAt(idx);
		//}
	}

	void Scene::HandleBuildingsCross(XY& pos_, float radius_) {
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

}
