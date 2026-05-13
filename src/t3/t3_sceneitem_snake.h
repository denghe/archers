#pragma once
#include "t3_scene.h"

namespace Test3 {

	struct Snake;
	struct SnakeElement : SceneProps12DotItem {

		// 蛇尾移动速度( 像素/帧 )
		static constexpr float cMoveSpeed{ 100.f * gg.cDelta };
		// 蛇身重连( 断开后的恢复 )速度( 像素/帧 )
		static constexpr float cLinkSpeed{ cMoveSpeed * 3.f };
		// 身体节点之间的距离占两节点半径和的比例( 越大越稀疏 )
		static constexpr float cNodeDistanceRatio{ 0.6f };
		// 绘制缩放( 可以令身体看上去比实际半径更大 或 更小 )
		static constexpr float cDrawScale{ 1.f };
		
		// 身体节点最大半径
		static constexpr float cMaxRadius{ 32.f };
		// 身体节点半径 呼吸效果 范围
		static constexpr xx::FromTo<float> cRadiusRange{ cMaxRadius * 0.9f, cMaxRadius };
		// 身体节点半径 呼吸效果 步长
		static constexpr float cRadiusAnimStep{ (cRadiusRange.to - cRadiusRange.from) / (0.7 * gg.cFps)};
		// 头部半径
		static constexpr float cHeadSize{ (cRadiusRange.from + cRadiusRange.to) * 0.5f * 1.7f };
		// 尾部半径
		static constexpr float cTailSize{ (cRadiusRange.from + cRadiusRange.to) * 0.5f * 0.7f };
		// 蛇身跟随距离( 像素 ) 如果和理论坐标差值在此范围内就直接使用理论坐标
		static constexpr float cFollowDistance{ cMaxRadius * 0.3f };

		// 指向所在蛇的指针( 生命周期通常长于 element )
		Snake* owner{};
		// 类型
		SnakeElementTypes elementType{};
		// 路径点数组下标
		int32_t pathwayCursor{ -1 };

		// 预初始化，填充 typeId, owner, elementType, indexAtContainer
		void PreInit(Snake* owner_, SnakeElementTypes elementType_);
		// 继续初始化
		void Init(float radius_);
		// 从蛇身上移除( 自杀 )
		void Dispose() override;
		// 碰撞检测，返回是否被击中( 头尾不参与碰撞 )
		bool HitCheck(XY p_, float hitRadius_);
		// 获取前一个节点，返回 nullptr 则没有
		SnakeElement* GetPrev();	// -> tail
		// 获取后一个节点，返回 nullptr 则没有
		SnakeElement* GetNext();	// -> head

		// 基于前一个节点的位置更新自己位置，返回 1 则需要被移除
		void BaseUpdate();
		// 替代 Update，返回 1 则需要被移除
		virtual int32_t ElementUpdate() { return 0; }

		void DrawLight() override;
	};

	struct Snake : SceneItem {
		// 存储所有身体节点. 第一个成员是 tail, 倒序, 方便计算身体位置
		xx::List<xx::Shared<SnakeElement>> elements;
		// 指向蛇当前线路
		Pathway* pathway{};
		// 1 / pathway->stepDistance
		float pathway_1_stepDistance{};
		// bodyLen_ 身体节数( 不算头和尾 )
		void Init(Scene* scene_, Pathway* pathway_, int32_t bodyLen_);
		void Update() override;
	};

	struct SnakeHead : SnakeElement {
		xx::Shaker faceShaker;
		void Init();
		int32_t ElementUpdate() override;
		void Draw() override;
	};

	struct SnakeTail : SnakeElement {
		void Init();
		int32_t ElementUpdate() override;
		void Draw() override;
	};

	struct SnakeBody : SnakeElement {
		int32_t U1_n{};
		void U1_RadiusAnim();

		// 每次挨打变白的时长
		static constexpr float cWhiteColorDuration{ 0.05f };
		// 挨打变白结束时间点
		float whiteColorEndTime{};

		// 受伤( attackValue_ 来自攻击方的 PropsCalcAttackValue 计算结果 )
		std::pair<float, int> Hurt(float attackValue_);

		void Init();
		int32_t ElementUpdate() override;
		void Draw() override;
	};

}
