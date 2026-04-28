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

	// todo:
	// 需要提前计算构成刀的圆形于每个角度的位置，以便于碰撞检测( 精度大概是 1 度，360 个圆形数组 )

	// 近战武器，刀。由一串圆形构成（类似糖葫芦）
	struct Creature;
	struct CreatureWeapon : SceneItem {
		static constexpr int32_t cTypeId{ 3 };

		// 穿刺间隔时长( 针对相同对象 )
		static constexpr float cPierceInterval{ 0.1f };

		// 刀长
		static constexpr float cLength{ 256.f };
		// 刀宽
		static constexpr float cWidth{ 16.f };
		// 刀的圆形数量( 密度 )
		static constexpr int32_t cDensity{ (int32_t)(cLength / cWidth * 2) };
		// 刀的圆形间隔
		static constexpr float cStep{ cLength / cDensity };

		// 刀的圆形位置( 相对于刀柄 ). 由 Init & Update 计算填充
		std::array<XY, cDensity> circlePositions;
		// 填充 circlePositions
		void FillCircles();

		// 指向拥有者
		xx::Weak<Creature> owner;

		// 基础攻击力
		float baseDamage{ 1.f };

		// 穿刺信息表( 黑名单, 避免武器连续多帧判定同一个对象 )
		xx::List<PierceInfo> pierceInfos;


		void Init(Creature* owner_);
		void Update() override;
		void Draw() override;
		void DrawGizmos() override;
		void DrawLight() override;
		void Dispose() override;
	};

}
