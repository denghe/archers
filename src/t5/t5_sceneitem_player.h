#pragma once
#include "t5_scene.h"

namespace Test5 {

	enum class ActionModes {
		Idle, Move, Attack
	};

	// 只能左右移动并挂接有武器的玩家( 武器可能会自动开火，但是和玩家就没有关系了 )
	struct Player : SceneProps12DotItem {
		static constexpr int32_t cTypeId{ 2 };

		// 4 向动画相关
		// 指向一段连续的 Frame 内存 ( 根据当前 动作 + 方向 来算 )
		xx::Frame* frames{};
		// 一段连续的 Frame 内存 有多长
		int32_t numFrames{};
		// 每帧帧下标的步进值( numFrames / 播完需要的帧数.  播完需要的帧数 = fps * 播完需要的时长 )
		float frameIndexStep{};	
		// 当前帧下标
		float frameIndex{};
		// 记录上一个坐标，用来推算角色朝向
		XY lastPos{};
		// 角色朝向( 0 ~ 3 )( 修改后须 FillFrames )
		int32_t frameDirection{ -1 };
		// 动作类型( 修改后须 FillFrames )
		ActionModes actionMode{ -1 };
		// 填充上面部分变量( 根据 actionMode & frameDirection )
		void FillFrames();
		// 步进 frame index
		void StepAnim();
		// 设置坐标, 同步 y 以及调更新 lastMoveDirection 的值，同时同步 4向动画相关 变量
		void SetPos(XY pos_);

		void Init(Scene* scene_, XYi cr_);
		void Update() override;
		void Draw() override;
		void DrawLight() override;
		void DrawShadow() override;
		void Dispose() override;
	};

}
