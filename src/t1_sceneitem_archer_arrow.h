#pragma once
#include "t1_scene.h"

namespace Test1 {

	struct Archer;
	struct ArcherArrow : SceneItem {
		static constexpr int32_t cTypeId{ 6 };

		// todo: 抛物线效果
		
		// 移动速度
		static constexpr XY cSpeed{ 1000.f };
		// 最大存活时长
		static constexpr float cMaxLifetime{ 5.f };
		// 帧动画步进值
		static constexpr float cFrameNumberInc{ 15.f / gg.cFps };

		// 指向拥有者
		xx::Weak<Archer> owner;
		xx::Weak<Monster> target;
		
		// 每帧移动步进值
		XY inc{};
		// 死亡时间点 = 创建时时间 + 最大存活时长
		float deathTime{};


		float frameNumber{};

		// 伤害值( 创建时从 owner 身上复制 )
		int32_t damage{};

		void Init(Archer* owner_, Monster* tar_);
		void Update() override;
		void Draw() override;
		void DrawLight() override;
		void Dispose() override;
	};

}
