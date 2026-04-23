#pragma once
#include "g_scenebase.h"
#include "g_props.h"
#include "g_dot.h"

namespace Test1 {

	struct Properties {
		int32_t hp{}, hpMax{};
	};

	struct Scene;

	// 场景对象之 基础版
	struct SceneItem : Global::SceneItemBase {

		// 为方便使用，指向所在场景指针( 生命周期通常长于 item )
		Scene* scene{};

		// 用于注册回调函数
		xx::List<std::function<void()>> disposeCallbacks;

		// 用于 Dispose 时调用回调函数
		void CallDisposeCallbacks();
	};

	// 场景对象之 带完整数值 能上DOT 版( 大多数怪继承它 )
	struct SceneProps12DotItem : SceneItem, Global::Props12, Global::DotContainer {
		// 令目标受伤
		virtual std::pair<float, int> Hurt(float attackValue_) { return {}; };

		// 绘制血条的功能函数
		virtual void DrawHPBar() {};
	};

	// 场景对象之 带结果数值版( 常见于子弹啥的. 创建时需复制创建者数据 )
	struct SceneProps2Item : SceneItem, Global::Props2 {};

	struct GridCache {
		XY pos{};
		float radius{};
		void operator=(SceneItem* p) {
			pos = p->pos;
			radius = p->radius;
		}
	};

	struct FloorMask {
		xx::Frame frame;
		XY pos{}, scale{ 1.f };
		float radians{}, colorplus{ 1.f };
		xx::RGBA8 color{ 0,0,0,127 };
	};

	// 一系列预声明就放在这以方便使用
	struct Wall;
	struct Lava;
	struct Outlet;
	struct Monster;
	struct Boss;
	struct Exploder;
	struct Archer;
	struct ArcherArrow;
	struct PhysSystem;
	// ...

}
