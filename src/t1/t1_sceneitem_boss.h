#pragma once
#include "t1_sceneitem_monster.h"

namespace Test1 {

	struct Boss : Monster {
		static constexpr int32_t cTypeId{ 2 };
		void Init(Scene* scene_, XY pos_);
	};

}
