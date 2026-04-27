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
		//radians = std::atan2(d.y, d.x);	// todo
	}

	void CreatureWeapon::Update() {
		// 移动( 同步 owner )
		pos = owner->pos;
		y = pos.y + 1.f;

		// 移除名单里面已经过期 或 对象已失效 的那部分
		auto currTime = scene->time;
		for (auto i = pierceInfos.len - 1; i >= 0; --i) {
			if (auto& o = pierceInfos[i]; !o.target || currTime >= o.elapsedTime) {
				pierceInfos.SwapRemoveAt(i);
			}
		}

		//// 查找子弹位置的怪
		//auto cri = scene->physMonsters->PosToCRIndex(pos);
		//scene->physMonsters->ForeachBy9Break(cri.y, cri.x, [&](PhysSystem::Node& o, float range)->bool {
		//	// 开始碰撞判定
		//	auto d = o.cache.pos - pos;
		//	d.y *= 2.0f;	// 椭圆效果
		//	auto mag2 = d.x * d.x + d.y * d.y;
		//	auto r = o.cache.radius + radius;
		//	auto rr = r * r;
		//	// 距离小于圆心和: 相交
		//	if (mag2 < rr) {
		//		// 开始穿刺处理
		//		// 如果目标在名单里则忽略碰撞
		//		if (pierceInfos.Exists([&](PierceInfo& pi)->bool {
		//				// 这里不用 Try版 是因为 Foreach 过程中不会发生 target 删除行为
		//				return pi.target.GetPointer() == o.value;
		//			})) {
		//			// 忽略碰撞 继续下次查询
		//			return false;
		//		}
		//		// 防止怪物释放内存导致指针失效，先拿 weak ptr
		//		auto w = xx::WeakFromThis((Monster*)o.value);
		//		// 伤害目标
		//		// 先算攻击力
		//		auto [atkVal, isCritical] = PropsCalcAttackValue(gg.rnd, baseDamage);
		//		// 得到实际造成的伤害
		//		auto [actualDmg, state] = w->Hurt(atkVal);
		//		if (state == 0) {
		//			assert(w);
		//			// 生成伤害数字特效( 暴击时颜色会不同 )
		//			scene->effectTexts.Add(pos, { 0,-1 }, isCritical ? xx::RGBA8_Red : xx::RGBA8_Yellow
		//				, 2 * scene->cam.scale, -actualDmg, true);
		//			// 在目标怪 身上挂 dot Fire
		//			DotFire::Make(this, w.GetPointer());
		//		}
		//		else if(state == 1) {
		//			assert(w);
		//			// todo: miss 的特效表达
		//		}
		//		else {
		//			assert(!w);
		//		}
		//		// 如果目标没死( 没被打死 或 miss )
		//		if (w) {
		//			// 记录到名单
		//			pierceInfos.Emplace(PierceInfo{
		//				.target = std::move(w),	// w 直接挪进去 后面不能再访问了
		//				.elapsedTime = currTime + cPierceInterval
		//				});
		//		}
		//		// 如果被 miss 则不消耗穿刺
		//		if (state != 1) {
		//			// 穿刺次数 -1
		//			--leftPierceCount;
		//			// 没有次数就终止整个查询
		//			if (leftPierceCount <= 0) return true;
		//		}
		//	}
		//	return false;
		//});
	}

	void CreatureWeapon::Draw() {
		gg.Quad().DrawFrame(gg.pics.c64_bullet, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians);
	}

	void CreatureWeapon::DrawLight() {
		gg.Quad().DrawFrame(gg.pics.c64_light, scene->cam.ToGLPos(pos)
			, (128.f / 32.f) * scene->cam.scale, 0, 0.5f);
	}

	void CreatureWeapon::Dispose() {
		assert(scene);
		assert(!disposing);

		// 设置标记
		disposing = true;
	}

}
