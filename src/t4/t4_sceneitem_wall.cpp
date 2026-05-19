#include "pch.h"
#include "t4.h"

namespace Test4 {

	void Wall::Init(Scene* scene_, XYi cr_, int32_t wallsIndex_) {
		typeId = cTypeId;
		scene = scene_;

		// pos 的坐标可以是 左上角，y 值可以是 底边, 参与显示排序
		cr = cr_;
		pos = cr_ * cCellPixelSize;
		y = pos.y;
		radius = cWallRadius;
		scale = radius / 16.f;	// 贴图是 32*32
		radians = {};

		indexAtContainer = scene_->walls.len - 1;
		assert(scene_->walls[indexAtContainer].pointer == this);

		scene_->gridWalls.Add(indexAtGrid, this);

		wallsIndex = wallsIndex_;
	}

	void Wall::FillTilesIndex() {
		// 根据邻居情况来选 tile frame
		auto s = scene->mapSize;
		auto& d = scene->mapData;
		uint32_t i{ 0b11111111u };
		// .2.
		// .o.
		// ...
		if (cr.y == 0 || d[(cr.y - 1) * s.x + cr.x] == U'墙') i &= 0b11111101u;
		// ...
		// 8o.
		// ...
		if (cr.x == 0 || d[cr.y * s.x + cr.x - 1] == U'墙') i &= 0b01111111u;
		// ...
		// .o4
		// ...
		if (cr.x == s.x - 1 || d[cr.y * s.x + cr.x + 1] == U'墙') i &= 0b11110111u;
		// ...
		// .o.
		// .6.
		if (cr.y == s.y - 1 || d[(cr.y + 1) * s.x + cr.x] == U'墙') i &= 0b11011111u;
		//// 12.
		//// 8o.
		//// ...
		if ((~i & 0b10000010u) == 0b10000010u && (cr.x == 0 || cr.y == 0 || d[(cr.y - 1) * s.x + cr.x - 1] == U'墙'))  i &= 0b11111110u;
		//// .23
		//// .o4
		//// ...
		if ((~i & 0b00001010u) == 0b00001010u && (cr.x == s.x - 1 || cr.y == 0 || d[(cr.y - 1) * s.x + cr.x + 1] == U'墙'))  i &= 0b11111011u;
		//// ...
		//// 8o.
		//// 76.
		if ((~i & 0b10100000u) == 0b10100000u && (cr.x == 0 || cr.y == s.y - 1 || d[(cr.y + 1) * s.x + cr.x - 1] == U'墙'))  i &= 0b10111111u;
		//// ...
		//// .o4
		//// .65
		if ((~i & 0b00101000u) == 0b00101000u && (cr.x == s.x - 1 || cr.y == s.y - 1 || d[(cr.y + 1) * s.x + cr.x + 1] == U'墙'))  i &= 0b11101111u;

		tilesIndex = i;
	}

	void Wall::Draw() {
		auto& f = gg.wallsTiles[wallsIndex][tilesIndex];
		assert(f.tex);
		gg.Quad().DrawTinyFrame(f, scene->cam.ToGLPos(pos), { 0,1 }, scale * scene->cam.scale, radians);
	}

	void Wall::DrawLightMask() {
		auto& f = gg.wallsShadowMaskTiles[tilesIndex];
		assert(f.tex);
		gg.Quad().DrawTinyFrame(f, scene->cam.ToGLPos(pos), { 0,1 }, scale * scene->cam.scale, radians);
	}

	void Wall::Dispose() {
		assert(scene);
		assert(!disposing);
		assert(indexAtContainer != -1);
		auto& container = scene->walls;
		assert(container[indexAtContainer].pointer == this);

		// 设置标记
		disposing = true;

		// 进一步释放资源
		if (indexAtGrid > -1) {
			scene->gridWalls.Remove(indexAtGrid, this);
		}

		// 从容器中移除对象( 释放内存 )
		auto i = indexAtContainer;
		container.Back()->indexAtContainer = i;
		indexAtContainer = -1;
		container.SwapRemoveAt(i);
	}

}
