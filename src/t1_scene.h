#pragma once
#include "t1_sceneitem.h"
#include <xx_grid2d_circle.h>

namespace Test1 {

	using SceneBase = Global::SceneBase;
	struct Scene : SceneBase {
		// 地图逻辑格子尺寸
		XYi mapSize{};
		// 地图总像素尺寸
		XY mapPixelSize{};
		// 地图布局数据( 值为 类型::cTypeId )
		xx::List<int32_t> mapData;

		// 空间索引 / 物理相关
		xx::Grid2dCircle<SceneItem*, GridCache> gridWalls;
		xx::Grid2dCircle<SceneItem*, GridCache> gridLavas;
		xx::Grid2dCircle<SceneItem*, GridCache> gridOutlets;
		xx::Shared<PhysSystem> physMonsters;
		// 所有墙壁
		xx::List<xx::Shared<Wall>> walls;
		// 所有岩浆
		xx::List<xx::Shared<Lava>> lavas;
		// 所有出口
		xx::List<xx::Shared<Outlet>> outlets;
		// 所有怪
		xx::List<xx::Shared<Monster>> monsters;
		// 所有弓箭手
		xx::List<xx::Shared<Archer>> archers;
		// 所有弓箭手的箭矢
		xx::List<xx::Shared<ArcherArrow>> archerArrows;
		// 所有怪物爆炸特效
		xx::List<xx::Shared<Exploder>> exploders;
		// 所有伤害文字
		xx::EffectTextManager effectTexts;

		// 所有弓箭手可放置的坐标
		xx::List<XYi> archerPoss;
		// 所有门(怪物出生点)的坐标
		xx::List<XYi> doorPoss;

		// for draw floorMaskTex, light ...
		xx::FrameBuffer frameBuffer;
		xx::Shared<xx::GLTexture> floorMaskTex;
		xx::List<FloorMask> floorMasks;

		// 地图生成
		void GenWorld();

		// 处理建筑重叠( 圆形判断，会挪移 pos 令重叠消失 )
		void HandleWallsCross(XY& pos_, float radius_);

		// 尝试获取 pos 附近相交( 圆心足够近 )的 lava. 未找到或不够近返回 空
		Lava* TryGetCrossLava(XY const& pos_, float radius_);

		// 尝试获取 pos 附近相交( 圆心足够近 )的 lava. 未找到或不够近返回 空
		Outlet* TryGetCrossOutlet(XY const& pos_, float radius_);

		void Init() override;
		void Update() override;
		void FixedUpdate() override;
		void Draw() override;
		void OnResize(bool modeChanged_) override;
	};

}
