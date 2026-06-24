#pragma once
#include "game.h"

// 模拟 两组怪 在同一张地图上 互相攻击的情况

namespace Test5 {
	// 这里放置一些常量

	static constexpr float cUIScale{ 0.5f };

	// 格子大小。同时也是 罐子 / 玩家 等物的逻辑大小
	static constexpr float cCellPixelSize{ 64.f };
	// 1 / 格子大小
	static constexpr float c1_CellPixelSize{ 1.f / cCellPixelSize };
	// 格子大小的一半
	static constexpr float cCellPixelHalfSize{ cCellPixelSize / 2.f };
	// 墙壁的半径。墙壁的碰撞体积是一个圆形，半径为格子大小的一半
	static constexpr float cWallRadius{ cCellPixelSize / 2.f };

	// ...
}
