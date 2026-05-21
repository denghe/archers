#pragma once
#include "t4_scene.h"

namespace Test4 {

	// 只能左右移动并挂接有武器的玩家( 武器可能会自动开火，但是和玩家就没有关系了 )
	struct Player : SceneProps12DotItem {
		static constexpr int32_t cTypeId{ 2 };
		static constexpr float cFrameIndexMax{ 6.f };
		static constexpr float cFrameIndexStep{ cFrameIndexMax / (gg.cFps * 0.5f) };
		// 帧下标
		float frameIndex{};

		// 设置坐标( 会同步 y )
		void SetPos(XY pos_);

		void Init(Scene* scene_, XYi cr_);
		void Update() override;
		void Draw() override;
		void DrawLight() override;
		void DrawShadow() override;
		void Dispose() override;
	};

}
