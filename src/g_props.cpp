#include "pch.h"
#include "g_props.h"

namespace Global {

	void Props12::PropsInit() {
		healthMaxPreset = healthMaxDefault;
		healthMaxRatio = 1.f;
		health = healthMax;
		healthRegenerationPreset = 0.f;
		healthRegenerationRatio = 1.f;
		defensePreset = 0.f;
		defenseRatio = 1.f;
		dodgePreset = 0.f;
		dodgeRatio = 1.f;
		damageBasePreset = 0.f;
		damageRatio = 1.f;
		attackSpeedRatio = 1.f;
		criticalChancePreset = 0.f;
		criticalChanceRatio = 1.f;
		criticalDamagePreset = 2.f;
		criticalDamageRatio = 1.f;
		movementSpeedRatio = 1.f;
	}

	void Props12::Props2Calc() {
		healthMax = std::max(healthMaxPreset * healthMaxRatio, 1.f);
		health = std::min(health, healthMax);
		healthRegeneration = std::min(healthRegenerationPreset * healthRegenerationRatio, 0.f);
		defense = defenseFactor / (defenseFactor + defensePreset * defenseRatio);
		dodge = 1.f - dodgeFactor / (dodgeFactor + dodgePreset * dodgeRatio);
		damageBase = std::max(damageBasePreset, 0.f);
		damage = std::max(damageRatio, 0.f);
		attackSpeed = std::max(std::min(1.f / attackSpeedRatio, attackSpeedMin), attackSpeedMax);
		criticalChance = std::max(std::min(criticalChancePreset * criticalChanceRatio, 1.f), 0.f);
		criticalDamage = std::max(criticalDamagePreset * criticalDamageRatio, 1.f);
		movementSpeed = std::max(std::min(moveSpeedBase * movementSpeedRatio, moveSpeedMax), 0.f);
	}

	float& Props12::PropsAt(int32_t idx_) const {
		assert(idx_ >= 0 && idx_ < numProps);
		return ((float*)this)[idx_];
	}

	std::pair<float, bool> Props12::PropsCalcDamagePoint(xx::Rnd& rnd_, float damageBase_) {
		auto d = (damageBase + damageBase_) * damage;
		if (rnd_.Next<float>() <= criticalChance) {
			return { d * criticalDamage, true };
		}
		return { d, false };
	}

	std::pair<float, int> Props12::PropsDoHurt(xx::Rnd& rnd_, float hurtValue_) {
		// 受伤前角色已死( 容错 )
		if (health <= 0) return { 0.f, 2 };
		// 闪避判断
		if (rnd_.Next<float>() <= dodge) return { 0.f, 1 };
		// 伤害按防御打折
		hurtValue_ *= defense;
		if (health <= hurtValue_) {
			// 算实际伤害值
			auto d = hurtValue_ - health;
			health = 0;
			return { d, 2 };
		}
		else {
			health -= hurtValue_;
			return{ hurtValue_, 0 };
		}
	}

	std::tuple<float, bool> Props12::PropsDoRegeneration(float interval_, float time_) {
		// 未到达回血时间点直接返回
		if (regenerationTime > time_) return {};
		auto bakHealth = health;
		// 最多回满
		health = std::min(health + healthRegeneration, healthMax);
		regenerationTime += interval_;
		// 返回实际回血量
		return { health - bakHealth, true };
	}

	// ... more funcs
};
