#pragma once
#include "t2_scene.h"

namespace Test2 {

	struct CreatureWeapon;
	struct Creature : SceneProps12DotItem {
		static constexpr int32_t cTypeId{ 2 };

		// 各类时间间隔( 模拟反应时间 同时省点 cpu )
		static constexpr float cSearchRange{ 512.f };
		static constexpr float cSearchInterval{ 0.5f };
		static constexpr float cMoveInterval{ 0.5f };
		static constexpr float cAttackInterval{ 0.2f };
		static constexpr float cMoveSpeed{ 500.f };

		// 挂接武器
		xx::Shared<CreatureWeapon> weapon;

		// 当前行为目标( 可能为空 )
		xx::Weak<Creature> target;

		// 指向敌对阵营的 gridCreaturess，方便搜索敌人( Init 中根据 campIndex 填充 )
		xx::Grid2dCircle<SceneItem*, GridCache>* enemyGrid{};
		xx::List<xx::Shared<Creature>>* enemyCreatures{};

		// 阵营索引( 同时也是 gridCreaturess 的索引 )
		int32_t campIndex{ -1 };
		// 在阵营空间索引中的位置，方便高速随机删除
		int32_t indexAtCampGrid{ -1 };

		XY velocity{};
		XY moveDirection{};
		float nextActionTime{};
		int32_t _1{}, _2{};

		// 设置坐标( 会同步 grid & y ) 主用于 phys 调用
		void SetPos(XY pos_);

		// 并非直接修改坐标移动，而是设置加速度( Update 调用 )
		void Move();

		Creature* SearchNearestEnemy(float searchRange_);

		void Init(Scene* scene_, XY pos_, int32_t campIndex_);
		void Update() override;
		void Draw() override;
		void DrawLight() override;
		void Dispose() override;
	};

}
