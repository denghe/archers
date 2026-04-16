#pragma once
#include "t1_scene.h"

namespace Test1 {

	struct Archer;
	struct ArcherArrow : SceneItem {
		static constexpr int32_t cTypeId{ 6 };

		// 移动速度
		static constexpr XY cSpeed{ 1000.f };
		// 最大存活时长
		static constexpr float cMaxLifetime{ 5.f };
		// 初始穿刺次数
		static constexpr int32_t cPierceCount{ 3 };
		// 穿刺间隔时长( 针对相同对象 )
		static constexpr float cPierceInterval{ 0.1f };

		// 指向拥有者
		xx::Weak<Archer> owner;
		// todo: 抛物线与锁定
		// 每帧移动步进值
		XY inc{};
		// 死亡时间点 = 创建时时间 + 最大存活时长
		float deathTime{};

		// 伤害值( 创建时从 owner 身上复制 )
		int32_t damage{};

		void Init(xx::Weak<Archer> owner_);
		void Update() override;
		void Draw() override;
		void DrawLight() override;
		void Dispose() override;
	};

}
