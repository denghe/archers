#pragma once
#include "t5_scene.h"

namespace Test5 {

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

		// 子弹飞行速度
		float bulletFlySpeed{ 100.f };

		// 每轮攻击延迟
		float shootDelay{ 0.2f };

		// 下一轮射击时间点
		float nextShootTime{};

		// 子弹创建时的发射距离( 以武器 pos 算起 )
		float shootDistance{ 32.f };

		// 每轮射击的子弹颗数
		int32_t shootCountPerRound{ 1 };

		// 当前发射子弹的穿刺总次数
		int32_t pierceCount{ 1 };

		// 挥刀相关
		int32_t _1{}, _i{};
		bool canBreakSwing{ true };
		// 开始挥刀( 可打断 挥刀协程 后半段的收刀部分 )
		void SwingBegin();
		// 挥刀协程
		void Swing();
		// 返回 0: 没在挥刀   1: 正在    -1: 正在收刀，可被打断
		int32_t IsSwinging() const;

		float CalcRadians();
		XY CalcPos();

		void Init(Player* owner_);
		void Update() override;
		void Draw() override;
		void DrawLight() override;
		void Dispose() override;
	};

}
