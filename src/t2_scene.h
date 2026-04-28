#pragma once
#include "t2_sceneitem.h"
#include <xx_grid2d_circle.h>
#include <t2_phys.h>

namespace Test2 {

	using SceneBase = Global::SceneBase;
	struct Scene : SceneBase {
		// 地图逻辑格子尺寸
		XYi mapSize{};
		// 地图总像素尺寸
		XY mapPixelSize{};
		// 地图布局数据( 值为 类型::cTypeId )
		xx::List<int32_t> mapData;

		// 墙壁空间索引
		xx::Grid2dCircle<SceneItem*, GridCache> gridWalls;
		// 所有墙壁
		xx::List<xx::Shared<Wall>> walls;

		// 生物空间索引( 分阵营 )
		std::unique_ptr<xx::Grid2dCircle<SceneItem*, GridCache>[]> gridCreaturess;
		// 所有生物( 分阵营 )
		xx::List<xx::List<xx::Shared<Creature>>> creaturess;
		// 生物物理系统( 不分阵营，统一处理移动和碰撞 )
		PhysSystem physCreatures;

		// 所有伤害文字
		xx::EffectTextManager effectTexts;

		// for draw floorMaskTex, light ...
		xx::FrameBuffer frameBuffer;
		xx::Shared<xx::GLTexture> floorMaskTex;
		xx::List<FloorMask> floorMasks;

		// 地图生成
		void GenWorld();

		// 生物生成
		void GenCreature(int32_t campIndex_, int32_t count_);

		// 处理建筑重叠( 圆形判断，会挪移 pos 令重叠消失 )
		void HandleBuildingsCross(XY& pos_, float radius_);

		void Init() override;
		void Update() override;
		void FixedUpdate() override;
		void Draw() override;
		void OnResize(bool modeChanged_) override;
	};

}
