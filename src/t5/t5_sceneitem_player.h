#pragma once
#include "t5_scene.h"

namespace Test5 {

	// ASDW 控制移动的玩家
	struct Player : SceneProps12DotItem {
		static constexpr int32_t cTypeId{ 2 };

		// 记录上一个坐标，用来推算角色朝向
		XY lastPos{};

		// 记录角色朝向
		float direction{};	// todo: directionSin Cos

		// for Draw: pos.y = y + yOffset
		float yOffset{};

		// 记录 pivot 和 center point 的 y 坐标差。在 InitYOffset() 中计算并填充
		float pcDiff{};

		int32_t _1{}, _2{}, _i{};
		float bounceHeight{}, bounceInc{}, radiansStep{}, radiansTarget{};
		float bounceHeightMax{}, bounceHalfDuration{};
		bool bouncing{}, moving{};

		void AnimInit();
		void AnimBounceRotate();
		void Anim();

		// 设置坐标, 同步 y
		void SetPos(XY pos_);

		void Init(Scene* scene_, XYi cr_);
		void Update() override;
		void Draw() override;
		void DrawLight() override;
		void DrawShadow() override;
		void Dispose() override;
	};

}
