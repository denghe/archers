#pragma once
#include "t5_scene.h"

namespace Test5 {

	// ASDW 控制移动的玩家
	struct Player : SceneProps12DotItem {
		static constexpr int32_t cTypeId{ 2 };

		// 记录上一个坐标，用来推算角色朝向
		XY lastPos{};
		float direction{};
		float yOffset{};		// for Draw: pos.y = y + yOffset
		float pcDiff{};			// fill by InitYOffset()
		void InitYOffset();
		void UpdateYOffset();
		int32_t _1{}, _2{}, _i{};
		float bounceHeight{}, bounceInc{}, radiansStep{}, radiansTarget{};
		float bounceHeightMax{}, bounceHalfDuration{};	// call AnimBounceRotate() before: need fill
		bool bouncing{}, moving{};
		void AnimBounceRotate();
		void AnimInit();		// init args
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
