#pragma once
#include "t1_sceneitem.h"

namespace Test1 {

	struct ArcherArrow;

	// 此物由 火箭 命中目标后 创建
	struct DotFire : Global::Dot {
		static constexpr int32_t cTypeId{ 1 };

		// 能存活 ?? 秒
		static constexpr float cLifespan{ 10.f };

		// 每 ?? 秒产生 1 次伤害
		static constexpr float cActiveInterval{ 0.1f };

		Scene* scene{};
		int32_t damage{};
		float nextDamageTime{};

		void Init(ArcherArrow* caster_);

		// 针对已存在 dot: 叠加伤害 并 延续时长
		void Refresh(ArcherArrow* caster_);

		// 帧逻辑驱动
		int32_t Update(Global::SceneItemBase* owner_) override;

		// 合并 / 替换 / 新建 dot 到 target( 可能会从 caster 复制一些参数 )
		static void Make(ArcherArrow* caster_, Global::DotContainer* target_);
	};

	// ... more Dot Types here
}
