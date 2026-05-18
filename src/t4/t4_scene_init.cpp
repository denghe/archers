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

		// 逐行扫内容并 各种预生成 / 填充
		for (int32_t y = 0; y < mapSize.y; y++) {
			for (int32_t x = 0; x < mapSize.x; x++) {
				auto i = y * mapSize.x + x;
				switch (mapData[i]) {
				case U'墙':
				{
					walls.Emplace().Emplace()->Init(this, { x,y });
					break;
				}
				}
			}
		}

		// todo: more logic here

	}

}
