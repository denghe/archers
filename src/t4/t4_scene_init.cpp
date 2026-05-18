#include "pch.h"
#include "t4.h"

namespace Test4 {

	void Scene::Init() {
		frameBuffer.Init();
		ui.Emplace()->InitRoot(gg.scale * cUIScale);
		MakeUI();
		cursor.Emplace()->Init();
		cursor->frame = gg.pics.cursor_cross;

		GenWorld();

		mapPixelSize = mapSize * cCellPixelSize;
		cam.Init(gg.scale, gg.designSize.y / mapPixelSize.y, mapPixelSize / 2);
		gridWalls.Init(cCellPixelSize, mapSize.y, mapSize.x);

		static constexpr auto cellSize = cCreatureRadius * 2;
		floorMaskTex.Emplace()->Make(mapPixelSize);

		assert(gridWalls.pixelSize.x >= mapPixelSize.x);
		assert(gridWalls.pixelSize.y >= mapPixelSize.y);

		//sortContainer.Resize<true>((int32_t)physCreatures.pixelSize.y);

		// 预生成针对 gg.pics.walls_? 贴图的 uv 布局. 整张图 256 * 348, 一共 8 列, 每列 32
		// 第一排高度是 32 第二排是 26，如此交替 6 次, (32 + 26) * 6. 26 高度的是面向玩家的斜壁, 独立处理
		// 每张 tile 按 外围描边 开口, 分为 8 格
		// 123
		// 8 4
		// 765
		// 中间一格算本体，四个方向是出路( 如果有邻居 ), 四个角用来配合描边
		// 根据排列组合，如果哪一格有描边，就记为1. 没有记为0. 可以转为 "1 byte 数字"
		// 可以建立 256 长度的数组，用 "1 byte 数字" 作为下标来定位 uv
		// 对于 567 部位带描边的情况，可对应 4 种斜坡，可以放到另一个 256 长度的数组
		// 
		// 当前 walls 图例，内容布局如下( 不算斜壁 )：
		// 
		// 123|123|123|1  |  3|123|123|123
		// 8  |   |  4|8  |  4|8  |  4|8 4
		// 7  |   |  5|7 5|7 5|7 5|7 5|7 5
		// ---+---+---+---+---+---+---+---
		// 199|  7| 31|208|235|215| 95|223
		// ---+---+---+---+---+---+---+---
		// 1  |   |  3|1 3|1 3|1 3|1 3|1 3
		// 8  |   |  4|8  |  4|8  |  4|8 4
		// 7  |   |  5|7  |  5|765|765|7 5
		// ---+---+---+---+---+---+---+---
		// 193|  0| 28|197| 29|245|125|221
		// ---+---+---+---+---+---+---+---
		// 1  |   |  3|123|123|1 3|123|1 3
		// 8  |   |  4|8  |   |8  |   |8 4
		// 765|765|765|7 5|7  |7 5|7 5|765
		// ---+---+---+---+---+---+---+---
		// 241|112|120|215| 71|213| 87|253
		// ---+---+---+---+---+---+---+---
		//   3|   |1  |1 3|1  |1 3|1 3|123
		//    |   |   |8  |   |   |  4|8 4
		// 7 5|7 5|7 5|765|765|765|7 5|765
		// ---+---+---+---+---+---+---+---
		// 	84| 80| 81|245|113|117| 93|255
		// ---+---+---+---+---+---+---+---
		//   3|1 3|1  |   |   |123|123|123
		//    |   |   |   |   |8  |   |  4
		//   5|7 5|7  |  5|7  |765|765|765
		// ---+---+---+---+---+---+---+---
		// 	20|	85| 65| 16| 64|247|119|127
		// ---+---+---+---+---+---+---+---
		// 1 3|1 3|1 3|  3|1  |  3|1  |   
		//    |   |   |   |   |   |   |   
		//   5|   |7  |   |   |7  |  5|   
		// ---+---+---+---+---+---+---+---
		// 	21|  5| 69|  4|  1| 68| 17|
		// ---+---+---+---+---+---+---+---
		// 

		// 逐行扫内容并 各种预生成 / 填充
		for (int32_t y = 0; y < mapSize.y; y++) {
			for (int32_t x = 0; x < mapSize.x; x++) {
				auto i = y * mapSize.x + x;
				switch (mapData[i]) {
				case U'墙':
				{
					// 计算出左上角坐标
					auto p = XY{ x, y } * cCellPixelSize;
					// 创建墙壁主体
					walls.Emplace().Emplace()->Init1(this, { x,y });
					break;
				}
				}
			}
		}

		// 再扫
		for (auto& o : walls) o->Init2();

		// todo: more logic here

	}

}
