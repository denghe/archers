#pragma once
#include "t5_scene.h"

namespace Test5 {

	// 放置在场上可以被玩家击碎的罐子
	struct Pot : SceneItem {
		static constexpr int32_t cTypeId{ 3 };

		void Init(Scene* scene_, XYi cr_);
		void Update() override;
		void Draw() override;
		void DrawLight() override;
		void DrawShadow() override;
		void DrawGizmos() override;
		void Dispose() override;

		// 每次挨打变白的时长
		static constexpr float cWhiteColorDuration{ 0.1f };
		// 挨打变白结束时间点
		float whiteColorEndTime{};

		// todo: 血条绘制

		// 受伤( attackValue_ 来自攻击方的 PropsCalcAttackValue 计算结果 )
		// 返回 实际受伤血量& 状态: 0 正常 1 被闪避 2 死亡
		std::pair<float, int> Hurt(float attackValue_);
	};

}
