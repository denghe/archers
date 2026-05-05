#include "pch.h"
#include "t1.h"

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

		// 初始化数据面板
		healthMaxDefault = 100.f;
		PropsInit();
		// 测试下暴击效果
		criticalDamagePreset = 3.f;
		criticalChancePreset = 0.2f;	
		PropsCalc();
	}

	void Archer::Update() {
		if (scene->time >= nextShootTime) {
			// 尝试攻击射程内最近怪
			auto searchRange = scene->mapPixelSize.x * 0.5f;
			// 从找到第一个开始判断后续 range 值如果变化就不是同一批了. 在同一批中选最近
			float minMag2{}, currentBatchRange{};

			Monster* tar{};
#if 0
			// 找最近
			auto g = scene->physMonsters.pointer;
			auto cri = g->PosToCRIndex(pos);
			g->ForeachByRange(cri.y, cri.x, searchRange, gg.sgrdd, [&](PhysSystem::Node& node, float range)->bool {
				auto d = pos - node.cache.pos;
				auto mag2 = d.x * d.x + d.y * d.y;
				// todo: 精确射程检测?
				if (!tar) {
					minMag2 = mag2;
					currentBatchRange = range;
					tar = (Monster*)node.value;
				}
				else {
					if (currentBatchRange != range) return true;
					if (mag2 < minMag2) {
						minMag2 = mag2;
						tar = (Monster*)node.value;
					}
				}
				return false;
			});
#else
			//// 追求输出利益最大化：随机选一个先，看看效果
			//if (scene->monsters.len) {
			//	tar = gg.rnd.NextElement(scene->monsters).pointer;
			//}

			// 在最近的一批怪物中随机选一个( 先找最近，再找最近怪的邻居 )
			// 找最近
			auto g = scene->physMonsters.pointer;
			auto cri = g->PosToCRIndex(pos);
			g->ForeachByRange(cri.y, cri.x, searchRange, gg.sgrdd, [&](PhysSystem::Node& node, float range)->bool {
				auto d = pos - node.cache.pos;
				auto mag2 = d.x * d.x + d.y * d.y;
				// todo: 精确射程检测?
				if (!tar) {
					minMag2 = mag2;
					currentBatchRange = range;
					tar = (Monster*)node.value;
				}
				else {
					if (currentBatchRange != range) return true;
					if (mag2 < minMag2) {
						minMag2 = mag2;
						tar = (Monster*)node.value;
					}
				}
				return false;
			});
			// 如果找到了最近的怪，再找最近怪的邻居，随机选一个
			if (tar) {
				auto p = tar->pos;
				cri = g->PosToCRIndex(p);
				searchRange = cCellPixelSize * 3.f;
				g->ForeachByRange(cri.y, cri.x, searchRange, gg.sgrdd, [&](PhysSystem::Node& node, float range)->bool {
					searchResultCache.Add((Monster*)node.value);
					return searchResultCache.len > 100; // 最多从 ??? 个当中随机
				});
				if (searchResultCache.len) {
					tar = gg.rnd.NextElement(searchResultCache);
					searchResultCache.Clear();
				}
			}
#endif
			if (tar) {
				nextShootTime = scene->time + cShootInterval;
				// todo: 如果距离很近就创建 ArcherArrow

				scene->archerArrows.Emplace().Emplace<ArcherArrow2>()->Init(this, tar);
			}
			else {
				nextShootTime = scene->time + cSearchInterval;
			}
		}
	}

	void Archer::Draw() {
		gg.Quad().DrawFrame(gg.pics.c128_player, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians);
	}

	void Archer::DrawLight() {
		gg.Quad().DrawFrame(gg.pics.c64_light, scene->cam.ToGLPos(pos)
			, (256.f / 64.f) * scene->cam.scale, 0, 0.5f);
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
