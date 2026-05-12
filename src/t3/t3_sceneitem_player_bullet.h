#pragma once
#include "t3_scene.h"

namespace Test3 {

	// 玩家的枪械发射的子弹
	struct PlayerBullet : SceneItem {
		static constexpr int32_t cTypeId{ 6 };
		static constexpr float cWidth{ 10.f };

		// 指向拥有者
		xx::Weak<Player> owner;
		xx::Weak<PlayerWeapon> weapon;

		// 基础攻击力
		float baseDamage{ 1.f };

		// ...

		void Init(PlayerWeapon* weapon_);
		void Update() override;
		void Draw() override;
		void DrawLight() override;
		void Dispose() override;
	};

}
