#pragma once
#include "g_scenebase.h"

namespace Global {

	struct DotContainer;

	// 基类. 需派生
	struct Dot {
		//static constexpr int32_t cTypeId{/* 派生类需要带这行，值不重复 */};

		// 需要在派生类中 typeId = cTypeId;
		int32_t typeId{};

		// 存储 dot 过期 / 死亡时间点
		float deathTime{};

		// 帧逻辑. 
		// 返回 0 表示没有异常，下次继续
		// 返回 1 表示 dot 自杀, 返回 -1 表示需要直接退出 DotsUpdate 函数
		virtual int32_t Update(SceneItemBase* owner_) { return 0; }

		virtual ~Dot() = default;
	};

	/**********************************************************************/
	/**********************************************************************/

	// 所有 dot 的容器. 和 SceneItem 一起 多重继承 以方便使用
	struct DotContainer {
		xx::List<xx::Shared<Dot>> dots;

		// 倒序执行所有 dots( 参数通常传 this. 也就是 dots 所在类 )
		// 返回 true 表示容器已自杀, 外部调用函数需继续 return
		bool DotsUpdate(SceneItemBase* owner_) {
			for (auto i = dots.len - 1; i >= 0; --i) {
				auto r = dots[i]->Update(owner_);
				if (r == -1) return true;
				else if (r == 1) {
					dots.SwapRemoveAt(i);
				}
			}
			return false;
		}
	};

}
