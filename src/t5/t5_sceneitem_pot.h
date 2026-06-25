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
		void Dispose() override;

		// todo: 血条绘制

		// 受伤( attackValue_ 来自攻击方的 PropsCalcAttackValue 计算结果 )
		std::pair<float, int> Hurt(float attackValue_) { return {}; }
	};

}
