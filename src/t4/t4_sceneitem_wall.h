#pragma once
#include "t4_scene.h"

namespace Test4 {

	struct Wall : SceneItem {
		static constexpr int32_t cTypeId{ 1 };

		// 逻辑格子坐标( 需要换算 pos )
		XYi cr{};

		// cr: column, row
		void Init(Scene* scene_, XYi cr_);

		void Draw() override;
		void Dispose() override;
	};

}
