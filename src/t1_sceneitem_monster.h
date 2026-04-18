#pragma once
#include "t1_scene.h"

namespace Test1 {

	struct Monster : SceneProps12Item {
		static constexpr int32_t cTypeId{ 4 };

		// 怪物动画帧数组下标范围
		xx::FromTo<float> frameIndexRange;
		float frameIndex{};

		void Init(Scene* scene_, XY pos_);
		void Update() override;
		void Draw() override;
		void DrawHPBar() override;
		void DrawLight() override;
		void Dispose() override;

		// 爆炸( 创建爆炸体并自杀 )
		void Explode();

		// 令怪物受伤 返回实际伤害值( 死亡则对象已 Dispose )
		float Hurt(float attackValue_);
	};

}
