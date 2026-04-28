#include "pch.h"
#include "t2.h"

namespace Test2 {

	void CreatureWeapon::Init(Creature* owner_) {
		typeId = cTypeId;
		scene = owner_->scene;
		owner = xx::WeakFromThis(owner_);

		pos = owner_->pos;
		y = pos.y + 1.f;
		radius = cCreatureRadius * 0.5f;
		scale = radius * 2.f / gg.pics.c64_bullet.uvRect.h;
		radians = owner_->radians - (cRadiansRange * 0.5f);
		FillCircles();
	}

	void CreatureWeapon::FillCircles() {
		// 生成刀的圆形判定数据
		// 先算出每个圆形的位置，间隔 cStep，数量 cDensity，半径 cWidth * 0.5f
		// 使用旋转矩阵算出每个圆形的位置，旋转矩阵由 radians 计算得到
		auto at = xx::AffineTransform::MakePosScaleRadians({}, 1.f, -radians);
		//auto r = cWidth * 0.5f;
		for (int32_t i = 0; i < cDensity; ++i) {
			circlePositions[i] = at.Apply({ i * cStep, 0.f });
		}
	}

	void CreatureWeapon::Swing() {
		// 刀从当前角度 -?? 度旋转到 +?? 度，耗费 ?? 秒
		// todo: 来回砍, 模拟雨刮器

		XX_BEGIN(_1);
		assert(swingStepCount == 0);
		for(swingStepCount = cCount; swingStepCount > 0; --swingStepCount) {
			radians += cRotateFrameStep;
			FillCircles();
			XX_YIELD(_1);
		}
		_1 = 0;
		XX_END(_1);
	}

	bool CreatureWeapon::IsSwinging() const {
		return _1 > 0;
	}


	void CreatureWeapon::Update() {
		// 同步 owner 坐标
		pos = owner->pos;
		y = pos.y + 1.f;

#if 0
		// 这段为了找bug
		radians = owner->radians;// -(cRadiansRange * 0.5f);
		FillCircles();
		return;
#endif

		// 挥刀
		if (IsSwinging()) {
			// 正在挥刀? 继续
			Swing();
		}
		else {
			// 不在挥刀? 则保持和 owner 同步
			radians = owner->radians - (cRadiansRange * 0.5f);
			FillCircles();
		}

		// 移除名单里面已经过期 或 对象已失效 的那部分
		auto currTime = scene->time;
		for (auto i = pierceInfos.len - 1; i >= 0; --i) {
			if (auto& o = pierceInfos[i]; !o.target || currTime >= o.elapsedTime) {
				pierceInfos.SwapRemoveAt(i);
			}
		}

		// 开始查找所有圆形位置的怪
		// 遍历所有圆形位置，查询周围的怪物，进行碰撞判定
		auto g = owner->enemyGrid;
		for (int32_t i = 0; i < cDensity; ++i) {
			auto p = pos + circlePositions[i];
			auto cri = g->PosToCRIndex(p);
			// 因为圆形体积不大，直接判断每个圆形位置所在格子和周围八个格子就行了
			g->ForeachBy9(cri.y, cri.x, [&](auto& o, float range)->bool {
				// 开始碰撞判定
				auto d = o.cache.pos - p;
				auto mag2 = d.x * d.x + d.y * d.y;
				auto r = o.cache.radius + cWidth * 0.5f;
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
					auto w = xx::WeakFromThis((Creature*)o.value);
					// 伤害目标
					// 先算攻击力
					auto [atkVal, isCritical] = owner->PropsCalcAttackValue(gg.rnd, baseDamage);
					// 得到实际造成的伤害
					auto [actualDmg, state] = w->Hurt(atkVal);
					if (state == 0) {
						assert(w);
						// 生成伤害数字特效( 暴击时颜色会不同 )
						scene->effectTexts.Add(p, { 0,-1 }, isCritical ? xx::RGBA8_Red : xx::RGBA8_Yellow
							, 2 * scene->cam.scale, -actualDmg, true);
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
						// 击退效果
						d = o.cache.pos - pos;
						mag2 = d.x * d.x + d.y * d.y;
						if (mag2 > 0.01f) {
							auto v = d / std::sqrtf(mag2) * 5000.f;
							scene->physCreatures.At((Creature*)o.value).accel += v;
						}

						// 记录到名单
						pierceInfos.Emplace(PierceInfo{
							.target = std::move(w),	// w 直接挪进去 后面不能再访问了
							.elapsedTime = currTime + cPierceInterval
						});
					}
				}
				return false;
			});
		}
	}

	void CreatureWeapon::Draw() {
		//gg.Quad().DrawFrame(gg.pics.c64_bullet, scene->cam.ToGLPos(pos)
		//	, scale * scene->cam.scale, radians);

		// todo: 换成单图

		auto& q = gg.Quad();
		for (int32_t i = 0; i < cDensity; ++i) {
			q.DrawFrame(gg.pics.c16, scene->cam.ToGLPos(pos + circlePositions[i])
				, cWidth / 16.f * scene->cam.scale, radians);
		}
	}

	void CreatureWeapon::DrawGizmos() {
		// 绘制刀的圆形判定数据
		// 先算出每个圆形的位置，间隔 cStep，数量 cDensity，半径 cWidth * 0.5f
		// 使用旋转矩阵算出每个圆形的位置，旋转矩阵由 radians 计算得到
		for (int32_t i = 0; i < cDensity; ++i) {
			gg.Line().DrawCircle(scene->cam.ToGLPos(pos + circlePositions[i])
				, cWidth * 0.5f * scene->cam.scale, radians, 8);
		}
	}

	void CreatureWeapon::DrawLight() {
		//gg.Quad().DrawFrame(gg.pics.c64_light, scene->cam.ToGLPos(pos)
		//	, (128.f / 32.f) * scene->cam.scale, 0, 0.5f);

		// todo: 换成单图

		auto& q = gg.Quad();
		for (int32_t i = 0; i < cDensity; ++i) {
			q.DrawFrame(gg.pics.c64_light, scene->cam.ToGLPos(pos + circlePositions[i])
				, cWidth / 64.f * 5.f * scene->cam.scale, radians);
		}
	}

	void CreatureWeapon::Dispose() {
		assert(scene);
		assert(!disposing);

		// 设置标记
		disposing = true;
	}

}
