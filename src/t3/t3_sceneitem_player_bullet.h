#pragma once
#include "t3_scene.h"

namespace Test3 {

	// 玩家的枪械发射的子弹
	struct PlayerBullet : SceneProps2Item {
		static constexpr int32_t cTypeId{ 6 };

		// 移动速度
		static constexpr float cSpeed{ 1000.f };
		// 最大存活时长
		static constexpr float cMaxLifetime{ 5.f };
		// 帧动画步进值
		static constexpr float cFrameNumberInc{ 15.f / gg.cFps };
		// 穿刺间隔时长( 针对相同对象 )
		static constexpr float cPierceInterval{ 0.1f };

		// 指向拥有者
		xx::Weak<Player> owner;
		// 每帧移动步进值
		XY inc{};
		// 死亡时间点 = 创建时时间 + 最大存活时长
		float deathTime{};
		// 当前帧下标
		float frameNumber{};

		// 箭矢的基础攻击力( Init 时从 weapon 复制 )
		float baseDamage{};
		// 剩余穿刺次数( 命中 1 次，减 1 )( 从 weapon 初始化初始次数 )
		int32_t leftPierceCount{};
		// 穿刺信息表( 黑名单, 避免子弹连续多帧判定同一个对象 )
		xx::List<PierceInfo> pierceInfos;


		void Init(Player* owner_, PlayerWeapon* weapon_, XY pos_, float radius_);
		void Update() override;
		void Draw() override;
		void DrawLight() override;
		void Dispose() override;
	};

}
