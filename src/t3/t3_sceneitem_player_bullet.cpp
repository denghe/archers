#include "pch.h"
#include "t3.h"

namespace Test3 {

	void PlayerBullet::Init(Player* owner_, PlayerWeapon* weapon_, XY pos_, float radius_) {
		typeId = cTypeId;
		scene = owner_->scene;
		owner = xx::WeakFromThis(owner_);

		indexAtContainer = scene->playerBullets.len - 1;
		assert(scene->playerBullets[indexAtContainer].pointer == this);

		// 算出每帧的步进
		flySpeed = weapon_->bulletFlySpeed;
		inc = { 0.f, -flySpeed * gg.cDelta };
		deathTime = scene->time + cMaxLifetime;

		pos = pos_;
		y = pos.y;
		radius = radius_;
		scale = radius * 2.f / gg.pics.firearrow_[0].uvRect.h;
		radians = -gPI_2;

		// 复制玩家当前数值面板值以便于算伤害
		*(Props2*)this = *(Props2*)owner;
		baseDamage = weapon_->baseDamage;

		// 初始化穿刺数
		leftPierceCount = weapon_->pierceCount;
	}

	void PlayerBullet::Update() {
		// 超时自杀
		if (scene->time >= deathTime) {
			Dispose();
			return;
		}

		// 移动
		pos += inc;
		y = pos.y;

		// 越界自杀
		if (pos.x < 0 || pos.x >= scene->mapPixelSize.x
			|| pos.y < 0 || pos.y >= scene->mapPixelSize.y) {
			Dispose();
			return;
		}

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

		// 查找子弹位置的蛇
		auto cri = scene->gridSnakes.PosToCRIndex(pos);
		scene->gridSnakes.ForeachBy9Break(cri.y, cri.x, [&](decltype(scene->gridSnakes)::Node& o, float range)->bool {
			// 类型判断: 当前仅对蛇身体造成伤害
			if (o.cache.elementType != SnakeElementTypes::Body) return false;

			// 开始碰撞判定
			auto d = o.cache.pos - pos;
			d.y *= 2.0f;	// 椭圆效果
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
				auto w = xx::WeakFromThis((SnakeBody*)o.value);
				// 伤害目标
				// 先算攻击力
				auto [atkVal, isCritical] = PropsCalcAttackValue(gg.rnd, baseDamage);
				// 得到实际造成的伤害
				auto [actualDmg, state] = w->Hurt(atkVal);
				if (state == 0) {
					assert(w);
					// 生成伤害数字特效( 暴击时颜色会不同 )
					scene->effectTexts.Add(pos, { 0, -1.f * (120.f / gg.cFps) }, isCritical ? xx::RGBA8_Red : xx::RGBA8_Yellow
						, 2, -actualDmg, true);
				}
				else if (state == 1) {
					assert(w);
					// todo: miss 的特效表达
				}
				else {
					assert(!w);
				}
				// 如果目标没死( 没被打死 或 miss )
				if (w) {
					// 记录到名单
					pierceInfos.Emplace(PierceInfo{
						.target = std::move(w),	// w 直接挪进去 后面不能再访问了
						.elapsedTime = currTime + cPierceInterval
						});
				}
				// 如果被 miss 则不消耗穿刺
				if (state != 1) {
					// 穿刺次数 -1
					--leftPierceCount;
					// 没有次数就终止整个查询
					if (leftPierceCount <= 0) return true;
				}
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

	void PlayerBullet::Draw() {
		gg.Quad().DrawFrame(gg.pics.firearrow_[frameNumber], scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians);
	}

	void PlayerBullet::DrawLight() {
		gg.Quad().DrawFrame(gg.pics.c64_light, scene->cam.ToGLPos(pos)
			, radians * (10.f / 64.f) * scene->cam.scale, 0, 0.8f);
	}

	void PlayerBullet::Dispose() {
		assert(scene);
		assert(!disposing);
		assert(indexAtContainer != -1);
		auto& container = scene->playerBullets;
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
