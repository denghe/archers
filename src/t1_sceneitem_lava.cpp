#include "pch.h"
#include "t1_.h"

namespace Test1 {

	void Lava::Init(Scene* scene_, XY pos_) {
		typeId = cTypeId;
		scene = scene_;
		pos = pos_;
		y = pos.y;
		radius = cWallRadius;
		scale = radius * 2.f / gg.pics.c128_wall.uvRect.w;
		radians = {};

		indexAtContainer = scene->lavas.len - 1;
		assert(scene->lavas[indexAtContainer].pointer == this);

		scene->gridLavas.Add(indexAtGrid, this);
	}

	void Lava::Draw() {
		gg.Quad().DrawFrame(gg.pics.c128_lava, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians, 1);
	}

	void Lava::Dispose() {
		assert(scene);
		assert(!disposing);
		assert(indexAtContainer != -1);
		auto& container = scene->lavas;
		assert(container[indexAtContainer].pointer == this);

		// 设置标记
		disposing = true;

		// 进一步释放资源
		if (indexAtGrid > -1) {
			scene->gridLavas.Remove(indexAtGrid, this);
		}

		// 从容器中移除对象( 释放内存 )
		auto i = indexAtContainer;
		container.Back()->indexAtContainer = i;
		indexAtContainer = -1;
		container.SwapRemoveAt(i);
	}

	void Lava::Visit(SceneItem* tar_) {
		// todo: 特效？折返？
		tar_->Dispose();
	}

}
