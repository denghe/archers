#pragma once
#include "t2_scene.h"

namespace Test2 {

	struct CreatureWeapon;
	struct Creature : SceneProps12DotItem {
		static constexpr int32_t cTypeId{ 7 };

		// 搜索时间间隔( 模拟反应时间 同时省点 cpu )
		static constexpr float cSearchInterval{ 0.2f };

		// 挂接武器
		xx::Shared<CreatureWeapon> weapon;

		// 阵营索引( 同时也是 gridCreaturess 的索引 )
		int32_t campIndex{ -1 }, indexAtCampGrid{ -1 };

		void Init(Scene* scene_, XY pos_, int32_t campIndex_);
		void Update() override;
		void Draw() override;
		void DrawLight() override;
		void Dispose() override;
	};

}
