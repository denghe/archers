#include "pch.h"
#include "t4.h"

namespace Test4 {

	void Wall::Init1(Scene* scene_, XYi cr_) {
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

	void Wall::Init2() {
		// todo: 考虑面向玩家这一侧的斜坡
		// 根据周围邻居的情况，选择合适的 uv 映射范围

	}

	void Wall::Draw() {
		// todo: 自定义 uv
		gg.Quad().DrawFrame(gg.pics.c128_wall, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians);
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
