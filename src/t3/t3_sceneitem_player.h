#pragma once
#include "t3_scene.h"

namespace Test3 {

	// 只能左右移动并挂接有武器的玩家( 武器可能会自动开火，但是和玩家就没有关系了 )
	struct Player : SceneProps12DotItem {
		static constexpr int32_t cTypeId{ 2 };

		// 移动速度
		static constexpr float cMoveSpeed{ 1500.f };

		// 挂接武器	// todo: 多种
		xx::Shared<PlayerWeapon> weapon;

		// 设置坐标( 会同步 y )
		void SetPos(XY pos_);

		void Init(Scene* scene_);
		void Update() override;
		void Draw() override;
		void DrawLight() override;
		void DrawShadow() override;
		void Dispose() override;
	};

}
