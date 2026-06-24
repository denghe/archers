#pragma once
#include "t5_scene.h"

namespace Test5 {

	// 放置在场上可以被玩家击碎的罐子
	struct Pot : SceneItem {
		static constexpr int32_t cTypeId{ 2 };	// todo

		void Init(Scene* scene_, XYi cr_);
		void Update() override;
		void Draw() override;
		void DrawLight() override;
		void DrawShadow() override;
		void Dispose() override;
	};

}
