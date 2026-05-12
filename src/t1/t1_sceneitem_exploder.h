#pragma once
#include "t1_scene.h"

namespace Test1 {

	struct Monster;
	// 爆炸者( 由炸药桶爆炸时创建，负责爆炸效果和伤害逻辑 )
	struct Exploder : SceneItem {
		static constexpr int32_t cTypeId{ 5 };

		static constexpr float cExplodeNumFrames{ gg.pics.explosion_1_.size() };
		static constexpr float cExplodeRadiusRatio{ 1.5f };
		static constexpr float cExplodeDuration{ 0.7f };

		int32_t _1{};
		float frameIndex{}, inc{};

		void Init(Monster* tar_);
		void Update() override;
		void Draw() override;
		void DrawLight() override;
		void Dispose() override;
	};

}
