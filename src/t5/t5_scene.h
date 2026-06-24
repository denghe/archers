#pragma once
#include "t5_sceneitem.h"
#include <xx_grid2d_circle.h>

namespace Test5 {

	using SceneBase = Global::SceneBase;
	struct Scene : SceneBase {

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

		// 罐子空间索引
		xx::Grid2dCircle<Pot*> gridPots;
		// 所有罐子
		xx::List<xx::Shared<Pot>> pots;

		// 追逐鼠标的玩家小人
		xx::Shared<Player> player;

		// for draw floorMaskTex, light ...
		xx::FrameBuffer frameBuffer;
		xx::Shared<xx::GLTexture> floorMaskTex;
		xx::List<FloorMask> floorMasks;

		// 地图生成
		void GenWorld();


		void MakeUI();
		void Init() override;
		void Update() override;
		void FixedUpdate() override;
		void Draw() override;
		void OnResize(bool modeChanged_) override;
	};

}
