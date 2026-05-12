#pragma once
#include "t1_scene.h"

namespace Test1 {

	struct Archer : SceneProps12DotItem {
		static constexpr int32_t cTypeId{ 7 };

		// 搜索时间间隔( 模拟反应时间 同时省点 cpu )
		static constexpr float cSearchInterval{ 0.5f };

		// 射击时间间隔
		static constexpr float cShootInterval{ 0.2f };

		// 下次射击时间
		float nextShootTime{};

		xx::List<Monster*> searchResultCache;

		void Init(Scene* scene_, XY pos_);
		void Update() override;
		void Draw() override;
		void DrawLight() override;
		void Dispose() override;
	};

}
