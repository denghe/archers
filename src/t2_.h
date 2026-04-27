#pragma once
#include "game.h"

// 模拟 两组怪 在同一张地图上 互相攻击的情况

namespace Test2 {
	// 这里放置一些常量

	static constexpr float cUIScale{ 0.5f };

	static constexpr float cCellPixelSize{ 128 };
	static constexpr float c1_CellPixelSize{ 1.f / cCellPixelSize };
	static constexpr float cCellPixelHalfSize{ cCellPixelSize / 2.f };
	static constexpr float cWallRadius{ cCellPixelSize / 2.f };
	static constexpr float cCreatureRadius{ cWallRadius };

	// ...
}
