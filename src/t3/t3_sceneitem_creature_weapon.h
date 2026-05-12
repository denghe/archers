#pragma once
#include "t3_scene.h"

namespace Test3 {

	// 近战武器，刀。由一串圆形构成（类似糖葫芦）
	// 刀在一个角度范围内来回挥动
	struct Creature;
	struct CreatureWeapon : SceneItem {
		static constexpr int32_t cTypeId{ 3 };

		static constexpr float cRadiansRange{ 120.f / 180.f * gPI };
		static constexpr float cDuration{ 0.3f };
		static constexpr int32_t cCount{ int32_t(cDuration / gg.cDelta) };
		static constexpr float cRotateFrameStep{ cRadiansRange / cCount };


		// 穿刺间隔时长( 针对相同对象 )
		static constexpr float cPierceInterval{ 0.1f };

		// 刀长
		static constexpr float cLength{ cCreatureRadius * 3.f };
		// 刀宽
		static constexpr float cWidth{ cLength / 6.f };
		// 刀的圆形数量( 密度 )
		static constexpr int32_t cDensity{ (int32_t)(cLength / cWidth * 2) };
		// 刀的圆形间隔
		static constexpr float cStep{ cLength / cDensity };

		// 刀的圆形位置( 相对于刀柄 ). 由 Init & Update 计算填充
		std::array<XY, cDensity> circlePositions;
		// 填充 circlePositions
		void FillCircles();

		// 挥刀
		void Swing();
		bool IsSwinging() const;

		// 指向拥有者
		xx::Weak<Creature> owner;

		int32_t _1{};
		int32_t swingStepCount{};

		// 当前挥刀方向( 1: 正向 或 -1: 反向 )
		int32_t swingDirection{ 1 };

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
