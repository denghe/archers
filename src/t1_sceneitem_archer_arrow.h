#pragma once
#include "t1_scene.h"

namespace Test1 {

	// 穿刺信息
	struct PierceInfo {
		// 目标
		xx::Weak<SceneItem> target;
		// 过期时间点
		float elapsedTime;
	};

	struct Archer;
	struct ArcherArrow : SceneProps2Item {
		static constexpr int32_t cTypeId{ 6 };

		// todo: 抛物线效果( 以及更多子弹攻击类型？
		// todo: 因为是锁定怪物的模式，所以只有当 arrow 落地后才判定，并且判定只是看指针是否未失效

		// 移动速度
		static constexpr XY cSpeed{ 1000.f };
		// 最大存活时长
		static constexpr float cMaxLifetime{ 5.f };
		// 帧动画步进值
		static constexpr float cFrameNumberInc{ 15.f / gg.cFps };
		// 初始穿刺次数
		static constexpr int32_t cPierceCount{ 5 };
		// 穿刺间隔时长( 针对相同对象 )
		static constexpr float cPierceInterval{ 0.1f };


		// 指向拥有者
		xx::Weak<Archer> owner;
		// 每帧移动步进值
		XY inc{};
		// 死亡时间点 = 创建时时间 + 最大存活时长
		float deathTime{};
		// 当前帧下标
		float frameNumber{};

		// 箭矢的基础攻击力
		float baseDamage{ 1.f };
		// 剩余穿刺次数( 命中 1 次，减 1 )
		int32_t leftPierceCount{};
		// 穿刺信息表( 黑名单, 避免子弹连续多帧判定同一个对象 )
		xx::List<PierceInfo> pierceInfos;


		void Init(Archer* owner_, Monster* tar_);
		void Update() override;
		void Draw() override;
		void DrawLight() override;
		void Dispose() override;
	};

}
