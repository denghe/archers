#pragma once
#include "t3_sceneitem.h"
#include <xx_grid2d_circle.h>
#include <t3_phys.h>

namespace Test3 {

	using SceneBase = Global::SceneBase;
	struct Scene : SceneBase {
		// 地图逻辑格子尺寸
		XYi mapSize{};
		// 地图总像素尺寸
		XY mapPixelSize{};

		// 为了公平 翻转以切换 update creaturess 的顺序
		bool flag{};

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

		// 蛇( 所有子节点 )空间索引
		xx::Grid2dCircle<SnakeElement*, SnakeElementGridCache> gridSnakes;
		// 所有蛇
		xx::List<xx::Shared<Snake>> snakes;
		// 蛇的路径
		xx::List<xx::Shared<Pathway>> pathways;

		// 在场景下方发射子弹攻击蛇的玩家本体
		xx::Shared<Player> player;
		// 所有玩家子弹
		xx::List<xx::Shared<PlayerBullet>> playerBullets;

		// 所有怪物爆炸特效
		xx::List<xx::Shared<Exploder>> exploders;
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
