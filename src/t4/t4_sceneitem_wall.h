#pragma once
#include "t4_scene.h"

namespace Test4 {

	struct Wall : SceneItem {
		static constexpr int32_t cTypeId{ 1 };

		// 逻辑格子坐标( 需要换算 pos )
		XYi cr{};

		// 先来一发, 记录基本参数, 占位
		void Init1(Scene* scene_, XYi cr_);

		// 所有 wall 建完之后 需要再遍历一次, call 它( 继续初始化显示内容 )
		void Init2();

		void Draw() override;
		void Dispose() override;
	};

}
