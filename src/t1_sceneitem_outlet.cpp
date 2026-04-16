#include "pch.h"
#include "t1_.h"

namespace Test1 {

	void Outlet::Init(Scene* scene_, XY pos_) {
		typeId = cTypeId;
		scene = scene_;
		pos = pos_;
		y = pos.y;
		radius = cWallRadius;
		scale = radius * 2.f / gg.pics.c128_wall.uvRect.w;
		radians = {};

		indexAtContainer = scene->outlets.len - 1;
		assert(scene->outlets[indexAtContainer].pointer == this);

		scene->gridOutlets.Add(indexAtGrid, this);
	}

	void Outlet::Draw() {
		// 在颜色上体现 open 状态
		auto color = xx::RGBA8_White;
		if (isOpened) {
			color = {55,55,55,55};
		}
		gg.Quad().DrawFrame(gg.pics.c128_door, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians, 1, color);
	}

	void Outlet::Dispose() {
		assert(scene);
		assert(!disposing);
		assert(indexAtContainer != -1);
		auto& container = scene->outlets;
		assert(container[indexAtContainer].pointer == this);

		// 设置标记
		disposing = true;

		// 进一步释放资源
		if (indexAtGrid > -1) {
			scene->gridOutlets.Remove(indexAtGrid, this);
		}

		// 从容器中移除对象( 释放内存 )
		auto i = indexAtContainer;
		container.Back()->indexAtContainer = i;
		indexAtContainer = -1;
		container.SwapRemoveAt(i);
	}

	void Outlet::Visit(SceneItem* tar_) {
		// todo
		tar_->Dispose();
	}

}
