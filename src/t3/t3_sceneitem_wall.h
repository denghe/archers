#pragma once
#include "t3_scene.h"

namespace Test3 {

	struct Wall : SceneItem {
		static constexpr int32_t cTypeId{ 1 };

		void Init(Scene* scene_, XY pos_);
		void Draw() override;
		void Dispose() override;
	};

}
