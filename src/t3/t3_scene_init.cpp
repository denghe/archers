#include "pch.h"
#include "t3.h"

namespace Test3 {

	void Scene::Init() {
		frameBuffer.Init();
		effectTexts.Init(&cam, 10000);
		ui.Emplace()->InitRoot(gg.scale * cUIScale);
		MakeUI();
		cursor.Emplace()->Init();
		cursor->frame = gg.pics.cursor_cross;

		GenWorld();

		mapPixelSize = mapSize * cCellPixelSize;
		cam.Init(gg.scale, gg.designSize.y / mapPixelSize.y, mapPixelSize / 2);

		gridWalls.Init(cCellPixelSize, mapSize.y, mapSize.x);

		gridCreaturess = std::make_unique<xx::Grid2dCircle<SceneItem*, GridCache>[]>(2);	// 预设两阵营
		gridCreaturess[0].Init(cCellPixelSize, mapSize.y, mapSize.x);
		gridCreaturess[1].Init(cCellPixelSize, mapSize.y, mapSize.x);
		creaturess.Resize(2);	// 预设两阵营

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

		gridSnakes.Init(cCellPixelSize, mapSize.y, mapSize.x);

		sortContainer.Resize<true>((int32_t)physCreatures.pixelSize.y);

		// 逐行扫内容并 各种预生成 / 填充
		for (int32_t y = 0; y < mapSize.y; y++) {
			for (int32_t x = 0; x < mapSize.x; x++) {
				auto i = y * mapSize.x + x;
				switch (mapData[i]) {
				case U'玩': {
					// 计算出左上角坐标
					auto p = XY{ x, y } * cCellPixelSize;
					// todo: 查找玩家左右两边区域，看是否有障碍，从而确定左右移动的最大限制
					player.Emplace<Player>()->Init(this, p + cCellPixelHalfSize);
					break;
				}
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

		// 生成一些蛇看看
		// 初始化 pathways
		auto basePos = mapPixelSize * 0.5f;
#if 0

		/*
 -300        0      300  360
   /--------<o-----------\	     -450
  o                       |		 -400
   \----------------\	  |		 -350
					 o	  |		 -300
   /----------------/	  |		 -250
  o						  |		 -200
   \----------------\	  |		 -150
					 o	  |		 -100
   /----------------/	  |		 -50
  o						  |		 0
   \----------------\	  |		 50
					 o	  |		 100
   /----------------/	  |		 150
  o						  |		 200
   \----------------\	  |		 250
					 o	  |		 300
   /----------------/	  |		 350
  o						  |		 400
   \---------------------/		 450

		*/

		auto bp = basePos + XY{ 0, -70.f };
		XY scale{ 1.6f, 0.7f };
		std::vector<xx::CurvePoint> cps{ xx::CurvePoint
			{ bp + XY{ 0, -450} * scale, 0.05f },
			{ bp + XY{ -300, -450 - 25} * scale, 0.05f },
			{ bp + XY{ -300, -350 - 25} * scale, 0.05f },
			{ bp + XY{ 300, -350 + 25} * scale, 0.05f },
			{ bp + XY{ 300, -250 + 25} * scale, 0.05f },
			{ bp + XY{ -300, -250 - 25} * scale, 0.05f },
			{ bp + XY{ -300, -150 - 25} * scale, 0.05f },
			{ bp + XY{ 300, -150 + 25} * scale, 0.05f },
			{ bp + XY{ 300, -50 + 25} * scale, 0.05f },
			{ bp + XY{ -300, -50 - 25} * scale, 0.05f },
			{ bp + XY{ -300, 50 - 25} * scale, 0.05f },
			{ bp + XY{ 300, 50 + 25} * scale, 0.05f },
			{ bp + XY{ 300, 150 + 25} * scale, 0.05f },
			{ bp + XY{ -300, 150 - 25} * scale, 0.05f },
			{ bp + XY{ -300, 250 - 25} * scale, 0.05f },
			{ bp + XY{ 300, 250 + 25} * scale, 0.05f },
			{ bp + XY{ 300, 350 + 25} * scale, 0.05f },
			{ bp + XY{ -300, 350 - 25} * scale, 0.05f },
			{ bp + XY{ -300, 450 - 25} * scale, 0.05f },

			{ bp + XY{ 300, 450 + 25} * scale, 0.05f },
			{ bp + XY{ 360, 400 + 25} * scale, 0.05f },
			{ bp + XY{ 360, -400} * scale, 0.05f },
			{ bp + XY{ 300, -450} * scale, 0.05f },
		};
		xx::MovePath mp;
		mp.FillCurve(true, cps);
		pathways.Emplace().Emplace()->Init(mp, 1.f);

#else
		auto& c = pathways.Emplace().Emplace();
		c->totalDistance = gg.curves.c1.totalDistance;
		c->stepDistance = gg.curves.c1.stepDistance;
		c->loop = gg.curves.c1.loop;
		XY scale {mapPixelSize.x / gg.curves.designWidth * 0.8f};
		scale.y = -scale.y;
		for (auto& cp : gg.curves.c1.points) {
			c->points.emplace_back(basePos + cp.pos * scale, -cp.radians);
			assert(c->points.back().pos.x >= 0 && c->points.back().pos.x < mapPixelSize.x);
			assert(c->points.back().pos.y >= 0 && c->points.back().pos.y < mapPixelSize.y);
		}
#endif

		GenSnake();
	}

}
