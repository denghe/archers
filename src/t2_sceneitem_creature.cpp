#include "pch.h"
#include "t2.h"

namespace Test2 {

	void Creature::SetPos(XY pos_) {
		pos = pos_;
		y = pos.y;
		scene->gridCreaturess[campIndex].Update(indexAtCampGrid, this);
	}

	void Creature::Init(Scene* scene_, XY pos_, int32_t campIndex_) {
		typeId = cTypeId;
		scene = scene_;
		pos = pos_;
		y = pos.y;
		radius = cCreatureRadius;
		campIndex = campIndex_;
		scale = radius * 2.f / gg.pics.c128_player.uvRect.w;
		radians = {};

		indexAtContainer = scene->creatures.len - 1;
		assert(scene->creatures[indexAtContainer].pointer == this);

		scene->physCreatures.Add(this);
		scene->gridCreaturess[campIndex].Add(indexAtCampGrid, this);

		// 初始化数据面板
		healthMaxDefault = 100.f;
		PropsInit();
		// 测试下暴击效果
		criticalDamagePreset = 3.f;
		criticalChancePreset = 0.2f;
		PropsCalc();

		// 初始化武器
		weapon.Emplace<CreatureWeapon>()->Init(this);
	}

	void Creature::Update() {
		// todo: 搜索目标，移动，攻击，受伤，死亡等等
		//scene->gridCreaturess[campIndex].Update(indexAtCampGrid, this);
	}

	void Creature::Draw() {
		auto c = xx::RGBA8_Blue;
		if (campIndex == 1) c = xx::RGBA8_Red;
		gg.Quad().DrawFrame(gg.pics.c128_monster, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians, 1.f, c);
	}

	void Creature::DrawLight() {
		gg.Quad().DrawFrame(gg.pics.c64_light, scene->cam.ToGLPos(pos)
			, (256.f / 64.f) * scene->cam.scale, 0, 0.5f);
	}

	void Creature::Dispose() {
		assert(scene);
		assert(!disposing);
		assert(indexAtContainer != -1);
		auto& container = scene->creatures;
		assert(container[indexAtContainer].pointer == this);

		// 设置标记
		disposing = true;

		scene->physCreatures.Remove(this);
		scene->gridCreaturess[campIndex].Remove(indexAtCampGrid, this);

		// 从容器中移除对象( 释放内存 )
		auto i = indexAtContainer;
		container.Back()->indexAtContainer = i;
		indexAtContainer = -1;
		container.SwapRemoveAt(i);
	}

}
