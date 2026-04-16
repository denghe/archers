#pragma once
#include "t1_scene.h"

namespace Test1 {

	struct Outlet : SceneItem {
		static constexpr int32_t cTypeId{ 2 };

		void Init(Scene* scene_, XY pos_);
		void Draw() override;
		void Dispose() override;

		virtual void Visit(SceneItem* tar_);
	};

}
