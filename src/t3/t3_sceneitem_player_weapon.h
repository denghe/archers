#pragma once
#include "t3_scene.h"

namespace Test3 {

	// 玩家的枪械。能自动发射子弹
	struct PlayerWeapon : SceneItem {
		static constexpr int32_t cTypeId{ 6 };
		static constexpr float cWidth{ 10.f };

		// 指向拥有者
		xx::Weak<Player> owner;

		// 武器基础伤害
		float baseDamage{ 10.f };

		// 每轮射击延迟
		float shootDelay{ 1.f };

		// 下一轮射击时间点
		float nextShootTime{};

		// 每轮射击的子弹颗数
		int32_t shootCountPerRound{ 1 };

		// 当前发射子弹的穿刺总次数
		int32_t pierceCount{ 1 };

		// ...


		void Init(Player* owner_);
		void Update() override;
		void Draw() override;
		void DrawLight() override;
		void Dispose() override;
	};

}
