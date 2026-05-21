#pragma once
#include "t4_sceneitem.h"
#include <xx_grid2d_circle.h>

namespace Test4 {

	using SceneBase = Global::SceneBase;
	struct Scene : SceneBase {

		/********************************/
		// UI area

		// 墙壁图切换
		static constexpr xx::FromTo<int32_t> cWallTexIndexRange{ 0, 2 };
		int32_t cWallTexIndex{ cWallTexIndexRange.from };
		xx::Shared<xx::Slider> uiWallTexIndex;

		// 地板图切换
		static constexpr xx::FromTo<int32_t> cFloorTexIndexRange{ 0, 4 };
		int32_t cFloorTexIndex{ cFloorTexIndexRange.from };
		xx::Shared<xx::Slider> uiFloorTexIndex;

		// UI area
		/********************************/


		// 地图逻辑格子尺寸
		XYi mapSize{};
		// 地图总像素尺寸
		XY mapPixelSize{};
		// 地图布局数据( 值为 类型::cTypeId )
		xx::List<int32_t> mapData;

		// 墙壁空间索引
		xx::Grid2dCircle<Wall*> gridWalls;
		// 所有墙壁
		xx::List<xx::Shared<Wall>> walls;

		// 整个能简单移动的 player 来测试一下小人挖矿的效果
		xx::Shared<Player> player;

		// for draw floorMaskTex, light ...
		xx::FrameBuffer frameBuffer;
		xx::Shared<xx::GLTexture> floorMaskTex;
		xx::List<FloorMask> floorMasks;

		// 地图生成
		void GenWorld();

		// 建墙
		void GenWall(XYi cr_);
		// 挖墙
		void DigWall(XYi cr_);
		// 设矿
		void SetWallOre(XYi cr_);

		void MakeUI();
		void Init() override;
		void Update() override;
		void FixedUpdate() override;
		void Draw() override;
		void OnResize(bool modeChanged_) override;
	};

}
