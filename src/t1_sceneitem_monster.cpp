#include "pch.h"
#include "t1_.h"

namespace Test1 {

	void Monster::Init(Scene* scene_, XY pos_, float radius_) {
		typeId = cTypeId;
		scene = scene_;
		pos = pos_;
		y = pos.y;
		radius = radius_;
		static constexpr auto n = gg.pics.creature_1_.size() / 4;
		auto i = gg.rnd.Next<int32_t>(n);
		frameIndexRange.from = i * 4;
		frameIndexRange.to = i * 4 + 4;
		frameIndex = frameIndexRange.from;
		scale = radius * 2.f / gg.pics.creature_1_[frameIndexRange.from].uvRect.w;
		radians = {};
		flipX = true;

		indexAtContainer = scene->monsters.len - 1;
		assert(scene->monsters[indexAtContainer].pointer == this);

		scene->physMonsters->Add(this);

		// 初始化数据面板
		healthMaxDefault = 100.f;
		PropsInit();
		dodgePreset = dodgeFactor;	// 测试下闪避效果。 1/2 概率挨打
		PropsCalc();
	}

	void Monster::Update() {
		// 判断是否掉入岩浆. 是：自杀
		if (scene->TryGetCrossLava(pos, radius)) {
			// todo: 特效？
			Dispose();
		}

		// 驱动 dots
		if (DotsUpdate(this)) return;

		// todo: 向前移动( 设置 accel )

		// 切帧动画
		frameIndex += (15.f / gg.cFps);
		if (frameIndex >= frameIndexRange.to) {
			frameIndex = frameIndexRange.from;
		}

	}

	void Monster::Draw() {
		XY s{ scale * scene->cam.scale };
		if (flipX) s.x = -s.x;
		gg.Quad().DrawFrame(gg.pics.creature_1_[frameIndex], scene->cam.ToGLPos(pos)
			, s, radians);
	}

	void Monster::DrawHPBar() {
		// 显示触发条件：带属性, 非满血
		if (health == healthMax) return;
		auto percent = (float)health / healthMax;
		auto& f = gg.pics.creature_1_[frameIndex];
		XY siz{ 15 * scale, 9 };
		auto p = pos + XY{ -siz.x * 0.5f, (f.uvRect.h * f.anchor.y + 1) * scale };
		siz *= scene->cam.scale;
		if (siz.x < 10) siz.x = 10;
		if (siz.y < 4) siz.y = 4;
		gg.HPBar().Alloc()->Fill(scene->cam.ToGLPos(p), siz, xx::RGBA8_Black, xx::RGBA8_White, xx::RGBA8_Red, percent);
	}

	void Monster::DrawLight() {
		gg.Quad().DrawFrame(gg.pics.c64_light, scene->cam.ToGLPos(pos)
			, (radius * (6.f / 64.f)) * scene->cam.scale, 0, 0.5f);
	}

	void Monster::DrawShadow() {
		gg.Quad().DrawFrame(gg.pics.c64_shadow, scene->cam.ToGLPos(pos)
			, (radius * (1.f / 40.f)) * scene->cam.scale, 0, 1.f, { 188,188,188,188 });
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

	std::pair<float, int> Monster::Hurt(float attackValue_) {
		auto r = PropsDoHurt(gg.rnd, attackValue_);
		if (r.second == 2) {
			scene->exploders.Emplace().Emplace()->Init(this);
			Dispose();
		}
		return r;
	}
}
