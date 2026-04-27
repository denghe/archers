#include "pch.h"
#include "t1.h"

namespace Test1 {

	void DotFire::Init(ArcherArrow* caster_) {
		typeId = cTypeId;
		scene = caster_->scene;
		Refresh(caster_);
	}

	void DotFire::Refresh(ArcherArrow* caster_) {
		deathTime = scene->time + cLifespan;
		damage += caster_->damage;
		nextActiveTime = 0;	// update 时立即触发一次
	}

	int32_t DotFire::Update(Global::SceneItemBase* owner_) {
		// 超时: 自杀
		if (scene->time >= deathTime) {
			return true;
		}
		// 拿到宿主类型指针备用
		auto self = (SceneProps12DotItem*)owner_;

		// 到达激活时间点: 产生伤害 并更新时间点
		if (scene->time >= nextActiveTime) {
			// 得到实际造成的伤害( 需要小心：可能令宿主 Dispose 内存失效 )
			auto [actualDmg, state] = self->Hurt(damage);
			// 容器已自杀, 直接返回 -1 通知外部循环退出函数
			if (state == 2) return -1;
			// 正常状态
			if (state == 0) {
				scene->effectTexts.Add(self->pos, { 0,-1 }, xx::RGBA8_Blue, 2 * scene->cam.scale, -damage, true);
			}
			// miss
			else {
				// todo: 特效
			}
			// 更新下次激活时间点
			nextActiveTime = scene->time + cActiveInterval;
		}

		// 下次继续
		return 0;
	}

	void DotFire::Make(ArcherArrow* caster_, Global::DotContainer* target_) {
		// 先检查是否已经有相同 dot 存在，如果存在则 Refresh
		auto& dots = target_->dots;
		for (auto& dot : dots) {
			if (dot->typeId == DotFire::cTypeId) {
				dot.Cast<DotFire>()->Refresh(caster_);
				return;
			}
		}
		// 新建
		dots.Emplace().Emplace<DotFire>()->Init(caster_);
	}

}
