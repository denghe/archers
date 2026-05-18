#include "pch.h"
#include "t4.h"

namespace Test4 {

	void Wall::Init(Scene* scene_, XYi cr_) {
		typeId = cTypeId;
		scene = scene_;

		// todo: cr to pos
		// pos 的坐标可以是 左上角，y 值可以是 底边, 参与显示排序
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
		// todo: 根据邻居情况来选 tile frame
		auto& f = gg.wallsTiles[0][0];
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
