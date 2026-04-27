#include "pch.h"
#include "t1.h"

namespace Test1 {

	void Scene::Init() {
		frameBuffer.Init();
		effectTexts.Init(&cam, 10000);
		ui.Emplace()->InitRoot(gg.scale * cUIScale);
		cursor.Emplace()->Init();
		cursor->frame = gg.pics.cursor_cross;
		GenWorld();

		// 填充 archerPixelPosPool
		// [o  ]
		// [  o]
		// [o   o  ]
		// [  o   o]
		// [o   o  ]
		// [  o   o]
		// 每个方块地图，像这样交错占位 密度可调? 行 = 列数，偶数
		static constexpr int32_t density{ 16 };
		static constexpr float margin{ cCellPixelSize / density };
		for (auto& p : archerPoss) {
			auto basePos = p * cCellPixelSize + margin * 0.5f;	// 每个大格左上角像素坐标
			for (int32_t y = 0; y < density; ++y) {
				for (int32_t x = 0; x < density; ++x) {
					if ((x & 1) == (y & 1)) {
						archerPixelPosPool.Emplace(XY{ x, y } * margin + basePos);
					}
				}
			}
		}

		// todo: more logic here
	}

	// todo: 
	// 
	// 怪一组一组的出. 
	// 前进角度可以有一点倾斜
	// 出怪的几种方式： 
	// 1 从屏外进入
	// 2 开个传送门再出现
	// 3 大怪召唤 / 复活尸体?
	// 怪的种类：
	// 移动速度快慢, 体积大小, 某伤害格挡?
	// 更多的防御 / 陷阱设施
	//
	// 更多弓箭手攻击手段，特效
	// 天赋树设计 数值成长
	// 

}
