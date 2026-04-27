#pragma once
#include "t2_scene.h"

namespace Test2 {

	// 穿刺信息
	struct PierceInfo {
		// 目标
		xx::Weak<SceneItem> target;
		// 过期时间点
		float elapsedTime;
	};

	struct Creature;
	struct CreatureWeapon : SceneItem {
		static constexpr int32_t cTypeId{ 3 };

		// 穿刺间隔时长( 针对相同对象 )
		static constexpr float cPierceInterval{ 0.1f };

		// 指向拥有者
		xx::Weak<Creature> owner;

		// 基础攻击力
		float baseDamage{ 1.f };

		// 穿刺信息表( 黑名单, 避免武器连续多帧判定同一个对象 )
		xx::List<PierceInfo> pierceInfos;


		void Init(Creature* owner_);
		void Update() override;
		void Draw() override;
		void DrawLight() override;
		void Dispose() override;
	};

}
