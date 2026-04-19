#pragma once
#include "game.h"

// 模拟 Zero Stress King: Idle Defense 这个游戏的 弓箭手 和 怪潮

namespace Test1 {
	// 这里放置一些常量

	static constexpr float cUIScale{ 0.5f };

	static constexpr float cCellPixelSize{ 128 };
	static constexpr float c1_CellPixelSize{ 1.f / 128 };
	static constexpr float cCellPixelHalfSize{ cCellPixelSize / 2.f };
	static constexpr float cWallRadius{ cCellPixelSize / 2.f };
	static constexpr float cMonsterRadius{ cWallRadius * 0.25f };
	static constexpr float cPlayerRadius{ cWallRadius * 0.25f };
	static constexpr float cBossRadius{ cMonsterRadius * 2.5f };

	// ...
}
