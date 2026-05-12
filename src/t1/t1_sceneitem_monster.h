#pragma once
#include "t1_scene.h"

namespace Test1 {

	struct MonsterLeader;
	struct Monster : SceneProps12DotItem {
		static constexpr int32_t cTypeId{ 4 };

		// 每次挨打变白的时长
		static constexpr float cWhiteColorDuration{ 0.1f };

		// 队长( 可能没有. 有则找队长要前进目标坐标 )
		xx::Weak<MonsterLeader> leader;

		// 在队内的编号（方便拿坐标）
		int32_t indexAtMembers{ -1 };

		// 动画帧数组下标范围
		xx::FromTo<float> frameIndexRange;

		// 动画帧游标
		float frameIndex{};

		// 挨打变白结束时间点
		float whiteColorEndTime{};

		void Init(Scene* scene_, XY pos_, float radius_ = cMonsterRadius);
		void Init(MonsterLeader* leader_);
		void Update() override;
		void Draw() override;
		void DrawHPBar() override;
		void DrawLight() override;
		void DrawShadow() override;
		void Dispose() override;

		// 令怪物受伤( 可能会死 ).
		// 返回 实际受伤血量 & 状态: 0 正常 1 被闪避 2 死亡( 对象已 Dispose )
		std::pair<float, int> Hurt(float attackValue_) override;
	};

	struct MonsterLeader : Monster {
		static constexpr int32_t cTypeId{ 8 };
		void Init(Scene* scene_, XY pos_, int32_t formationId_);

		// 成员相对坐标( 前进目标 )
		xx::List<XY> memberOffsets;

		// 用于队员找队长拿自己的前进目标坐标
		XY GetTargetPos(Monster* member_) const;
	};
}
