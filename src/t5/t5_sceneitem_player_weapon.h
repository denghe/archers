#pragma once
#include "t5_scene.h"

namespace Test5 {

	struct Player;

	// 悬浮在玩家身上的近战武器：刀
	// 1. 由玩家持有，跟随玩家移动
	// 2. 挥刀时，刀会旋转一定角度，形成一个扇形的刀气( 大概由 3 张动画组成，不一定移动, 每张持续 5 帧 )
	// 3. 挥完刀后，刀角度会慢慢( 角速度限制 )旋转到出刀前角度. 出刀前角度 = 玩家当前朝向 +/- ?? 度
	// 4. 如果 cd 时间已到达, 则可以再次挥刀, 即便刀角度还没旋转到出刀前角度, 也可以再次挥刀
	struct PlayerWeapon : SceneItem {
		static constexpr int32_t cTypeId{ 4 };

		// 指向拥有者
		xx::Weak<Player> owner;

		// 目标朝向
		float targetRadians{}, radiansStep{};

		// 基础攻击力
		float baseDamage{ 1.f };

		// 挥刀相关
		int32_t _1{}, _i{};
		// ...
		void Swing();
		bool IsSwinging() const;
		float CalcRadians();
		XY CalcPos();

		void Init(Player* owner_);
		void Update() override;
		void Draw() override;
		void DrawLight() override;
		void Dispose() override;
	};

}
