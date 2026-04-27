#include "pch.h"
#include "t2.h"

namespace Test2 {

	void Scene::Init() {
		frameBuffer.Init();
		effectTexts.Init(&cam, 10000);
		ui.Emplace()->InitRoot(gg.scale * cUIScale);
		cursor.Emplace()->Init();
		cursor->frame = gg.pics.cursor_cross;

		GenWorld();

		mapPixelSize = mapSize * cCellPixelSize;
		cam.Init(gg.scale, 1.f, mapPixelSize / 2);
		gridWalls.Init(cCellPixelSize, mapSize.y, mapSize.x);
		gridCreaturess = std::make_unique<xx::Grid2dCircle<SceneItem*, GridCache>[]>(2);	// 预设两阵营
		gridCreaturess[0].Init(cCellPixelSize, mapSize.y, mapSize.x);
		gridCreaturess[1].Init(cCellPixelSize, mapSize.y, mapSize.x);
		static constexpr auto cellSize = cCreatureRadius * 2;
		physCreatures.Init(this
			, std::ceilf(mapPixelSize.y / cellSize)
			, std::ceilf(mapPixelSize.x / cellSize)
			, cellSize, 5000, 15);
		floorMaskTex.Emplace()->Make(mapPixelSize);

		assert(gridWalls.pixelSize.x >= mapPixelSize.x);
		assert(gridWalls.pixelSize.y >= mapPixelSize.y);
		assert(physCreatures.pixelSize.x >= mapPixelSize.x);
		assert(physCreatures.pixelSize.y >= mapPixelSize.y);
		sortContainer.Resize<true>((int32_t)physCreatures.pixelSize.y);

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
					walls.Emplace().Emplace()->Init(this, p + cCellPixelHalfSize);
					// 判断左边和上边如果也有，在中缝补一个墙增加密度
					if (x > 0 && mapData[i - 1] == U'墙') {
						walls.Emplace().Emplace()->Init(this, p + XY{ 0, cCellPixelHalfSize });
					}
					if (y > 0 && mapData[i - mapSize.x] == U'墙') {
						walls.Emplace().Emplace()->Init(this, p + XY{ cCellPixelHalfSize, 0 });
					}

					break;
				}
				}
			}
		}

		// todo: more logic here
	}

}
