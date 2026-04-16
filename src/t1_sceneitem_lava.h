#pragma once
#include "t1_scene.h"

namespace Test1 {

	struct Lava : SceneItem {
		static constexpr int32_t cTypeId{ 3 };

		void Init(Scene* scene_, XY pos_);
		void Draw() override;
		void Dispose() override;

		virtual void Visit(SceneItem* tar_);
	};

}
