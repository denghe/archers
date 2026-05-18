#include "pch.h"
#include "t4.h"

namespace Test4 {

	void Wall::Init(Scene* scene_, XYi cr_) {
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
	}

	void Wall::Draw() {
		// todo: 当 walls 内容变化时 再算 idx

		// 根据邻居情况来选 tile frame
		auto s = scene->mapSize;
		auto& d = scene->mapData;
		uint32_t i{};

		//// x..
		//// .o.
		//// ...
		//if (cr.x == 0 || cr.y == 0 || d[(cr.y - 1) * s.x + cr.x - 1] != U'墙') i |= 0b1;
		//// .x.
		//// .o.
		//// ...
		//if (cr.y == 0 || d[(cr.y - 1) * s.x + cr.x] != U'墙') i |= 0b10;
		//// ..x
		//// .o.
		//// ...
		//if (cr.x == s.x - 1 || cr.y == 0 || d[(cr.y - 1) * s.x + cr.x + 1] != U'墙') i |= 0b100;
		//// ...
		//// xo.
		//// ...
		//if (cr.x == 0 || d[cr.y * s.x + cr.x - 1] != U'墙') i |= 0b10000000;
		//// ...
		//// .ox
		//// ...
		//if (cr.x == s.x - 1 || d[cr.y * s.x + cr.x + 1] != U'墙') i |= 0b1000;
		//// ...
		//// .o.
		//// x..
		//if (cr.x == 0 || cr.y == s.y - 1 || d[(cr.y + 1) * s.x + cr.x - 1] != U'墙') i |= 0b1000000;
		//// ...
		//// .o.
		//// .x.
		//if (cr.y == s.y - 1 || d[(cr.y + 1) * s.x + cr.x] != U'墙') i |= 0b100000;
		//// ...
		//// .o.
		//// ..x
		//if (cr.x == s.x - 1 || cr.y == s.y - 1 || d[(cr.y + 1) * s.x + cr.x + 1] != U'墙') i |= 0b10000;

		// .x.
		// .o.
		// ...
		if (cr.y == 0 || d[(cr.y - 1) * s.x + cr.x] != U'墙') i |= 0b111;
		// ...
		// xo.
		// ...
		if (cr.x == 0 || d[cr.y * s.x + cr.x - 1] != U'墙') i |= 0b11000001;
		// ...
		// .ox
		// ...
		if (cr.x == s.x - 1 || d[cr.y * s.x + cr.x + 1] != U'墙') i |= 0b11100;
		// ...
		// .o.
		// .x.
		if (cr.y == s.y - 1 || d[(cr.y + 1) * s.x + cr.x] != U'墙') i |= 0b1110000;

		// todo: 斜坡

		auto& f = gg.wallsTiles[0][i];
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
