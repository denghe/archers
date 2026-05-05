#pragma once
#include "t1_scene.h"

namespace Test1 {

	struct Archer;
	struct ArcherArrow2 : SceneProps2Item {
		static constexpr int32_t cTypeId{ 9 };

		// 抛物线箭矢
		// 当箭矢落地( 飞行附加高度为 0 )时判定，先看目标是否存活。优先检测箭矢和目标的碰撞。
		// 如果目标不存在了 或 没有碰撞，则箭矢直接落地自杀

		// todo: 如果和目标的距离过近，就飞直线？？这个判断或许应该放在 archer 的射击逻辑里

		// 移动速度
		static constexpr float cSpeed{ 1000.f };
		// 每帧移动步进值
		static constexpr float cFrameSpeed{ cSpeed / gg.cFps };
		// 帧动画步进值
		static constexpr float cFrameNumberInc{ 15.f / gg.cFps };


		// 指向拥有者
		xx::Weak<Archer> owner;
		// 指向目标
		xx::Weak<Monster> target;
		// 每帧移动步进值
		XY inc{};
		// 攻击目标位置
		XY tarPos{};
		// 上一帧的位置( 用于算角度 )
		XY lastPos{};
		// 逻辑坐标( 不受 yOffset 影响 )
		XY pos2{};
		// 飞行阶段 的帧数计数器
		int32_t numSteps{}, totalSteps{};
		// 飞行最高点的 y 值( 用于抛物线效果 )
		float maxY{};
		// 飞行附加高度( 用于抛物线效果 )
		float yOffset{};
		// 当前帧下标
		float frameNumber{};
		// 箭矢的基础攻击力
		float baseDamage{ 10.f };
		// coroutine 相关
		int32_t _1{};

		// 箭矢落地时的碰撞检测和伤害判定. 返回 true 表示碰撞发生( miss 也算 )
		bool HitTarget(XY tarPos_, float tarRadius_, Monster* tar_);

		void Init(Archer* owner_, Monster* tar_);
		void Update() override;
		void Draw() override;
		void DrawLight() override;
		void Dispose() override;
	};

}
