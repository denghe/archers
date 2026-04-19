#pragma once
#include "t1_scene.h"

namespace Test1 {

	struct Monster : SceneProps12DotItem {
		static constexpr int32_t cTypeId{ 4 };

		// 怪物动画帧数组下标范围
		xx::FromTo<float> frameIndexRange;
		float frameIndex{};

		// todo: 挨打变白

		void Init(Scene* scene_, XY pos_);
		void Update() override;
		void Draw() override;
		void DrawHPBar() override;
		void DrawLight() override;
		void Dispose() override;

		// 令怪物受伤( 可能会死 ).
		// 返回 实际受伤血量 & 状态: 0 正常 1 被闪避 2 死亡( 对象已 Dispose )
		std::pair<float, int> Hurt(float attackValue_) override;
	};

}
