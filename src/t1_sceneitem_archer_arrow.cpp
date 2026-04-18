#include "pch.h"
#include "t1_.h"

namespace Test1 {

	void ArcherArrow::Init(Archer* owner_, Monster* tar_) {
		typeId = cTypeId;
		scene = owner_->scene;
		owner = xx::WeakFromThis(owner_);

		indexAtContainer = scene->archerArrows.len - 1;
		assert(scene->archerArrows[indexAtContainer].pointer == this);

		// 算出每帧的步进
		auto d = tar_->pos - owner_->pos;
		auto mag2 = d.x * d.x + d.y * d.y;
		auto _1mag = 1.f / std::sqrtf(mag2);
		inc = d * _1mag * cSpeed * gg.cDelta;
		deathTime = scene->time + cMaxLifetime;

		pos = owner_->pos;
		y = pos.y;
		radius = 16.f;
		scale = radius * 2.f / gg.pics.firearrow_[0].uvRect.h;
		radians = std::atan2(d.y, d.x);

		// 复制玩家当前数值面板值以便于算伤害
		*(Props2*)this = *(Props2*)owner;
		leftPierceCount = cPierceCount;
	}

	void ArcherArrow::Update() {
		// 超时自杀
		if (scene->time >= deathTime) {
			Dispose();
			return;
		}

		// 移动
		pos += inc;
		y = pos.y;

		// 步进帧动画
		frameNumber += cFrameNumberInc;
		if (frameNumber >= gg.pics.firearrow_.size()) {
			frameNumber = 0.f;
		}

		assert(leftPierceCount > 0);
		// 移除名单里面已经过期 或 对象已失效 的那部分
		auto currTime = scene->time;
		for (auto i = pierceInfos.len - 1; i >= 0; --i) {
			if (auto& o = pierceInfos[i]; !o.target || currTime >= o.elapsedTime) {
				pierceInfos.SwapRemoveAt(i);
			}
		}

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
				// 开始穿刺处理
				// 如果目标在名单里则忽略碰撞
				if (pierceInfos.Exists([&](PierceInfo& pi)->bool {
						// 这里不用 Try版 是因为 Foreach 过程中不会发生 target 删除行为
						return pi.target.GetPointer() == o.value;
					})) {
					// 忽略碰撞 继续下次查询
					return false;
				}
				// 防止怪物释放内存导致指针失效，先拿 weak ptr
				auto w = xx::WeakFromThis((Monster*)o.value);
				// 伤害目标
				// 先算攻击力
				auto [atkVal, isCritical] = PropsCalcAttackValue(gg.rnd, baseDamage);
				// 得到实际造成的伤害
				auto actualDmg = w->Hurt(atkVal);
				// 生成伤害数字特效( 暴击时颜色会不同 )
				scene->effectTexts.Add(pos, { 0,-1 }, isCritical ? xx::RGBA8_Red : xx::RGBA8_Yellow
					, 2 * scene->cam.scale, -actualDmg, true);
				// 如果目标没死
				if (w) {
					// 记录到名单
					pierceInfos.Emplace(PierceInfo{
						.target = std::move(w),	// w 直接挪进去 后面不能再访问了
						.elapsedTime = currTime + cPierceInterval
						});
				}
				// 穿刺次数 -1
				--leftPierceCount;
				// 没有次数就终止整个查询
				if (leftPierceCount <= 0) return true;
			}
			return false;
		});

		bool needDispose{};
		if (leftPierceCount > 0) {
			// 查找子弹位置的建筑. 如果有相交，子弹自杀
			using G = decltype(scene->gridWalls);
			auto& g = scene->gridWalls;
			cri = g.PosToCRIndex(pos);
			needDispose = g.ForeachBy9Break(cri.y, cri.x, [&](G::Node& node, float range)->bool {
				auto d = pos - node.cache.pos;
				auto mag2 = d.x * d.x + d.y * d.y;
				auto r = node.cache.radius;	// +radius; 忽略子弹半径，让碰撞显得更贴墙
				auto rr = r * r;
				return mag2 < rr;
			});
		}
		else {
			needDispose = true;
		}
		if (needDispose) {
			// todo: 弄点子弹命中建筑消失的粒子？
			Dispose();
			return;
		}
	}

	void ArcherArrow::Draw() {
		gg.Quad().DrawFrame(gg.pics.firearrow_[frameNumber], scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians);
	}

	void ArcherArrow::DrawLight() {
		gg.Quad().DrawFrame(gg.pics.c64_light, scene->cam.ToGLPos(pos)
			, (256.f / 64.f) * scene->cam.scale, 0, 0.5f);
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
