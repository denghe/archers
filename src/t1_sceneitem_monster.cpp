#include "pch.h"
#include "t1_.h"

namespace Test1 {

	void Monster::Init(Scene* scene_, XY pos_) {
		typeId = cTypeId;
		scene = scene_;
		pos = pos_;
		y = pos.y;
		radius = cBucketRadius;
		scale = radius * 2.f / gg.pics.c128_bucket.uvRect.w;
		radians = {};

		indexAtContainer = scene->monsters.len - 1;
		assert(scene->monsters[indexAtContainer].pointer == this);

		scene->physMonsters->Add(this);

		// 创建数据面板并初始化
		properties.Emplace();
		properties->hp = 100;
		properties->hpMax = 100;
	}

	void Monster::Update() {
		// todo: 向前移动
	}

	void Monster::Draw() {
		gg.Quad().DrawFrame(gg.pics.c128_bucket, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians);
	}

	void Monster::DrawHPBar() {
		// 显示触发条件：带属性, 非满血
		if (properties->hp == properties->hpMax) return;
		auto percent = (float)properties->hp / properties->hpMax;
		auto p = pos + XY{ -25, -40 };
		auto siz = XY{ 50, 9 } * scene->cam.scale;
		if (siz.x < 10) siz.x = 10;
		if (siz.y < 4) siz.y = 4;
		gg.HPBar().Alloc()->Fill(scene->cam.ToGLPos(p), siz, xx::RGBA8_Black, xx::RGBA8_White, xx::RGBA8_Red, percent);
	}

	void Monster::DrawLight() {
		gg.Quad().DrawFrame(gg.pics.c64_light, scene->cam.ToGLPos(pos)
			, (256.f / 64.f) * scene->cam.scale);
	}

	void Monster::Dispose() {
		assert(scene);
		assert(!disposing);
		assert(indexAtContainer != -1);
		auto& container = scene->monsters;
		assert(container[indexAtContainer].pointer == this);

		// 设置标记
		disposing = true;

		// 进一步释放资源
		scene->physMonsters->Remove(this);

		// 调用回调函数
		CallDisposeCallbacks();

		// 从容器中移除对象( 释放内存 )
		auto i = indexAtContainer;
		container.Back()->indexAtContainer = i;
		indexAtContainer = -1;
		container.SwapRemoveAt(i);
	}

	void Monster::Explode() {
		scene->exploders.Emplace().Emplace()->Init(this);
		Dispose();
	}
}
