#pragma once
#include "t4_scene.h"

namespace Test4 {

	struct Wall : SceneItem {
		static constexpr int32_t cTypeId{ 1 };

		// 逻辑格子坐标( 需要换算 pos )
		XYi cr{};
		// 根据相邻关系推算出的 gg.wallsTiles 1级下标
		int32_t wallsIndex{ -1 };
		// 根据相邻关系推算出的 gg.wallsTiles 2级下标
		int32_t tilesIndex{ -1 };
		// 先随机弄点矿叠加显示
		int32_t oreIndex{ -1 };

		// cr: column, row
		void Init(Scene* scene_, XYi cr_, int32_t wallsIndex_ = 0);
		// 根据周围邻居填充 tilesIndex( 发生于地型创建后或改变后 )
		void FillTilesIndex();
		// 随机填矿
		void RndOre();

		void Draw() override;
		void DrawLightMask() override;
		void Dispose() override;
	};

}
