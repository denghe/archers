#include "pch.h"
#include "t2.h"

namespace Test2 {

	void Creature::SetPos(XY pos_) {
		pos = pos_;
		y = pos.y;
		scene->gridCreaturess[campIndex].Update(indexAtCampGrid, this);
	}

	void Creature::Init(Scene* scene_, XY pos_, int32_t campIndex_) {
		typeId = cTypeId;
		scene = scene_;
		pos = pos_;
		y = pos.y;
		radius = cCreatureRadius;
		campIndex = campIndex_;
		scale = radius * 2.f / gg.pics.c128_player.uvRect.w;
		radians = {};

		auto& c = scene->creaturess[campIndex];
		indexAtContainer = c.len - 1;
		assert(c[indexAtContainer].pointer == this);

		scene->physCreatures.Add(this);
		scene->gridCreaturess[campIndex].Add(indexAtCampGrid, this);

		// 初始化数据面板
		healthMaxDefault = 100.f;
		PropsInit();
		// 测试下暴击效果
		criticalDamagePreset = 3.f;
		criticalChancePreset = 0.2f;
		PropsCalc();

		// 初始化武器
		weapon.Emplace<CreatureWeapon>()->Init(this);

		// 指向敌对阵营的 gridCreaturess，方便搜索敌人
		if (campIndex == 1) {
			enemyGrid = &scene->gridCreaturess[0];
			enemyCreatures = &scene->creaturess[0];
		}
		else {
			enemyGrid = &scene->gridCreaturess[1];
			enemyCreatures = &scene->creaturess[1];
		}
	}

	std::pair<float, int> Creature::Hurt(float attackValue_) {
		auto r = PropsDoHurt(gg.rnd, attackValue_);
		if (r.second == 0) {
			// 变白
			whiteColorEndTime = scene->time + cWhiteColorDuration;
		}
		else if (r.second == 2) {
			// 爆炸特效
			scene->exploders.Emplace().Emplace()->Init(this);
			// 自杀
			Dispose();
		}
		return r;
	}

	Creature* Creature::SearchNearestEnemy(float searchRange_) {
		Creature* tar{};
		float minMag2{}, currentBatchRange{};
		auto cri = enemyGrid->PosToCRIndex(pos);
		enemyGrid->ForeachByRange(cri.y, cri.x, searchRange_, gg.sgrdd, [&](auto& node, float range)->bool {
			auto d = pos - node.cache.pos;
			auto mag2 = d.x * d.x + d.y * d.y;
			if (!tar) {
				minMag2 = mag2;
				currentBatchRange = range;
				tar = (Creature*)node.value;
			}
			else {
				if (currentBatchRange != range) return true;
				if (mag2 < minMag2) {
					minMag2 = mag2;
					tar = (Creature*)node.value;
				}
			}
			return false;
		});
		return tar;
	}

	void Creature::Move() {
		scene->physCreatures.At(this).accel += moveDirection * cMoveSpeed;
	}

	void Creature::Update() {
		XX_BEGIN(_1);
		// 出生后发呆一会儿( 方便物理挤开啥的 )
		for (nextActionTime = scene->time + cBornDuration; nextActionTime > scene->time;) {
			XX_YIELD(_1);
		}
	LabSearch:
		{
			// 查找 search 范围内的敌人，找出最近的
			auto tar = SearchNearestEnemy(cSearchRange);
			if (!tar) {
				// 没找到? 从敌人数组随机选一个
				if (!enemyCreatures->Empty()) {
					tar = gg.rnd.NextElement(*enemyCreatures).pointer;
				}
			}
			// 将结果写入 target
			if (tar) {
				target = xx::WeakFromThis(tar);
			}
		}
		if (!target) {
			// 等待 search 冷却时间
			for (nextActionTime = scene->time + cSearchInterval; nextActionTime > scene->time;) {
				// todo: 等待过程中应该也能够随机移动?
				XX_YIELD(_1);
			}
			// 继续 search
			goto LabSearch;
		}
		// 平滑转向目标, 对准之后再下一步
		while (!xx::AngleStep(radians, xx::Atan2(target->pos - pos), cRotationFrameLimit)) {
			XX_YIELD(_1);
			// 转向目标过程中如果目标死了 就继续 search
			if (!target) goto LabSearch;
		}
		// 如果目标还活着, 判断距离
		if (target) {
			auto d = target->pos - pos;
			auto mag2 = d.x * d.x + d.y * d.y;
			auto r = weapon->cLength;
			// 在攻击范围内? 直接攻击. 否则移动
			if (mag2 < r * r) goto LabAttack;
			else if (mag2 > 0.01f) {
				auto mag = std::sqrtf(mag2);
				moveDirection = d / mag;
				goto LabMove;
			}
		}

	LabMove:
		// 朝目标方向移动一段时间
		for (nextActionTime = scene->time + cMoveInterval; nextActionTime > scene->time;) {
			if (target) {
				// 平滑转向目标
				xx::AngleStep(radians, xx::Atan2(target->pos - pos), cRotationFrameLimit);
			}
			Move();
			XX_YIELD(_1);
		}
		// 再次 serach ( 有可能目标已经死了 或者 跑远了, 有更好的选择 )
		goto LabSearch;

	LabAttack:
		// 攻击逻辑. 先转向目标 然后挥刀
		weapon->Swing();
		// 等待挥刀结束( 这期间不旋转 )
		while (weapon->IsSwinging()) {
			XX_YIELD(_1);
		}
		if (target) {
			// 计算移动方向( 并不每帧都计算，让跟随显得滞后笨拙一些 )
			auto d = target->pos - pos;
			auto mag2 = d.x * d.x + d.y * d.y;
			if (mag2 > 0.01f) {
				auto mag = std::sqrtf(mag2);
				moveDirection = d / mag;
			}
		}
		// 等待攻击冷却时间. 等待过程中继续按 moveDirection 移动, 并且转向目标( 怪就算死了也要等待 )
		for (nextActionTime = scene->time + cAttackInterval; nextActionTime > scene->time;) {
			if (target) {
				// 平滑转向目标
				xx::AngleStep(radians, xx::Atan2(target->pos - pos), cRotationFrameLimit);
				Move();
			}
			XX_YIELD(_1);
		}
		// 如果目标没死, 还在攻击范围内 就继续攻击. 否则继续 search
		if (target) {
			auto d = target->pos - pos;
			auto mag2 = d.x * d.x + d.y * d.y;
			auto r = weapon->cLength;
			if (mag2 < r * r) goto LabAttack;
		}
		goto LabSearch;
		XX_END(_1);
	}

	void Creature::Draw() {
		auto c = xx::RGBA8_Blue;
		if (campIndex == 1) c = xx::RGBA8_Red;
		float cp{ 1 };
		if (scene->time < whiteColorEndTime) {
			cp = 10000.f;
			c = xx::RGBA8_White;
		}
		gg.Quad().DrawFrame(gg.pics.c128_monster, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians, cp, c);
		weapon->Draw();
	}

	void Creature::DrawLight() {
		gg.Quad().DrawFrame(gg.pics.c64_light, scene->cam.ToGLPos(pos)
			, (256.f / 64.f) * scene->cam.scale, 0, 0.5f);
		weapon->DrawLight();
	}

	void Creature::DrawShadow() {
		// todo
		weapon->DrawShadow();
	}

	void Creature::DrawHPBar() {
		// 显示触发条件：带属性, 非满血
		if (health == healthMax) return;
		auto percent = (float)health / healthMax;
		auto& f = gg.pics.c128_player;
		XY siz{ 100 * scale, 12 };
		auto p = pos + XY{ -siz.x * 0.5f, (f.uvRect.h * f.anchor.y + 1) * scale };
		siz *= scene->cam.scale;
		if (siz.x < 20) siz.x = 20;
		if (siz.y < 6) siz.y = 6;
		gg.HPBar().Alloc()->Fill(scene->cam.ToGLPos(p), siz, xx::RGBA8_Black, xx::RGBA8_White, {100,0,0,255}, percent);
	}

	void Creature::Dispose() {
		assert(scene);
		assert(!disposing);
		assert(indexAtContainer != -1);
		auto& container = scene->creaturess[campIndex];
		assert(container[indexAtContainer].pointer == this);

		// 设置标记
		disposing = true;

		scene->physCreatures.Remove(this);
		scene->gridCreaturess[campIndex].Remove(indexAtCampGrid, this);

		// 从容器中移除对象( 释放内存 )
		auto i = indexAtContainer;
		container.Back()->indexAtContainer = i;
		indexAtContainer = -1;
		container.SwapRemoveAt(i);
	}

}
