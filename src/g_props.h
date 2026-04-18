#pragma once
#include "xx_prims.h"
#include "xx_rnd.h"

namespace Global {

	// Preset 预设/基数 & Ratio 乘数( 后面备注的数字是 作为数组访问时的 下标 )
	struct Props1 {
		// 血上限
		float healthMaxPreset{};			// 0
		float healthMaxRatio{};				// 1
		// 血再生( 每秒回血量 )
		float healthRegenerationPreset{};	// 2
		float healthRegenerationRatio{};	// 3
		// 防御系数
		float defensePreset{};				// 4
		float defenseRatio{};				// 5
		// 闪避系数
		float dodgePreset{};				// 6
		float dodgeRatio{};					// 7
		// 伤害
		float damageBasePreset{};			// 8
		float damageRatio{};				// 9
		// 攻速倍率( 技能cd 除数 )
		float attackSpeedRatio{};			// 10
		// 暴击率
		float criticalChancePreset{};		// 11
		float criticalChanceRatio{};		// 12
		// 暴击伤害倍率
		float criticalDamagePreset{};		// 13
		float criticalDamageRatio{};		// 14
		// 移动速度倍率
		float movementSpeedRatio{};			// 15
	};

	// 结果 / 运行时
	struct Props2 {
		// 血上限
		float healthMax{};					// 16
		// 当前剩余血量
		float health{};						// 17
		// 血再生( 每秒回血量 )
		float healthRegeneration{};			// 18
		// 防( 伤害折扣百分比 )
		float defense{};					// 19
		// 闪避( 让伤害 miss 的百分比 )
		float dodge{};						// 20
		// 伤害基数
		float damageBase{};					// 21
		// 伤害倍率
		float damage{};						// 22
		// 攻速倍率( 已通过 1 / 转为了乘数 )
		float attackSpeed{};				// 23
		// 暴击率( 已限制到了 0 ~ 1 )
		float criticalChance{};				// 24
		// 暴击伤害( 倍率 )
		float criticalDamage{};				// 25
		// 移动速度( 每秒像素距离 )
		float movementSpeed{};				// 26

		// 计算并返回 攻击力(伤害值) & 是否暴击
		std::pair<float, bool> PropsCalcAttackValue(xx::Rnd& rnd_, float damageBase_);

		// 执行受伤逻辑( 受伤时调用 ). hurt value 来自攻击方的 PropsCalcAttackValue 计算结果
		// 返回 受伤血量 & 状态: 0 正常 1 被闪避 2 死亡
		std::pair<float, int> PropsDoHurt(xx::Rnd& rnd_, float hurtValue_);

		// ... more funcs
	};

	// 合并 Props1 & 2
	// 目标类直接走多重继承带上这个类 从而实现数值支撑
	struct Props12 : Props1, Props2 {
		// 数组模式长度( 成员个数 )
		static constexpr int32_t numProps{ (sizeof(Props1) + sizeof(Props2)) / sizeof(float) };

		// 各种系数 / 配置( 通常在初始化阶段前设置好 后面不会改变 )
		float healthMaxDefault{ 100.f };
		// 防御系数
		float defenseFactor{ 100.f };
		// 闪避系数
		float dodgeFactor{ 100.f };
		// 攻速相关
		float attackSpeedMin{ 1.f / 120.f };
		float attackSpeedMax{ 120.f };
		// 移动相关
		float moveSpeedBase{ 500.f };
		float moveSpeedMax{ 3000.f };
		// ...

		// 各种运行时状态变量
		// 存储下次应该回血的时间点
		float regenerationTime{};
		// ...

		// 填充变量初始值( Props1 ) 理论上讲也可以读配置并覆盖
		void PropsInit();

		// 根据上下文计算并将结果填充到 props2( 初始化后 或 装备变化后 适合调用 )
		void PropsCalc();

		// 以数组下标方式访问成员( 常用于遍历装备累加数值 )
		float& PropsAt(int32_t idx_) const;

		// 执行回血逻辑( 每帧调用 )
		// 返回执行结果: 回血量, true/false( 是否产生了回血行为 )
		std::tuple<float, bool> PropsDoRegeneration(float interval_, float time_);

		// ... more funcs
	};

}
