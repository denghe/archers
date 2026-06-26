#pragma once
#include "t5_scene.h"

namespace Test5 {

	struct PlayerWeapon;

	// ASDW 控制移动的玩家
	struct Player : SceneProps12DotItem {
		static constexpr int32_t cTypeId{ 2 };

		// 玩家身上挂接的武器数组
		xx::List<xx::Shared<PlayerWeapon>> weapons;

		// 记录上一个坐标，用来推算角色朝向
		XY lastPos{};

		// 记录角色朝向
		float direction{};
		XY directionCosSin{};

		// 跳着走相关
		int32_t _1{}, _2{}, _i{};
		float bounceRadiansTarget{}, bounceRadiansStep{};
		float bounceHeight{}, bounceInc{};

		// 正在跳, 正在移动
		bool bouncing{}, moving{};

		void AnimBounceRotate();
		void Anim();

		// 配合挥刀的下蹲动作相关
		int32_t _3{}, _j{};
		XY scale2{ 1 };
		void SquatBegin();
		void Squat();
		bool IsSquating() const;

		// 设置坐标, 同步 y
		void SetPos(XY pos_);

		void Init(Scene* scene_, XYi cr_);
		void Update() override;
		void Draw() override;
		void DrawLight() override;
		void DrawShadow() override;
		void DrawGizmos() override;
		void Dispose() override;
	};

}
