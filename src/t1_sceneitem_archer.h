#pragma once
#include "t1_scene.h"

namespace Test1 {

	struct Archer : SceneItem {
		static constexpr int32_t cTypeId{ 7 };
		static constexpr float cSearchInterval{ 0.2f };
		static constexpr float cShootInterval{ 1.f };
		int32_t damage{};
		int32_t _1{};
		float nextShootTime{};

		void Init(Scene* scene_, XY pos_);
		void Update() override;
		void Draw() override;
		void DrawLight() override;
		void Dispose() override;
	};

}
