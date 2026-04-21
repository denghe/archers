#include "pch.h"
#include "t1_.h"

namespace Test1 {

	void Monster::Init(Scene* scene_, XY pos_, float radius_) {
		// 超出地图边界，硬拉?
		auto s = scene_->mapPixelSize - cCellPixelSize;
		if (pos_.x >= s.x) 
			pos_.x = s.x;
		else if (pos_.x < cCellPixelSize)
			pos_.x = cCellPixelSize;
		if (pos_.y >= s.y) 
			pos_.y = s.y;
		else if (pos_.y < cCellPixelSize)
			pos_.y = cCellPixelSize;

		typeId = cTypeId;
		scene = scene_;
		pos = pos_;
		y = pos_.y;
		radius = radius_;
		static constexpr auto n = gg.pics.creature_1_.size() / 4;
		auto i = gg.rnd.Next<int32_t>(n);
		frameIndexRange.from = i * 4;
		frameIndexRange.to = i * 4 + 4;
		frameIndex = frameIndexRange.from;
		scale = radius * 2.f / gg.pics.creature_1_[frameIndexRange.from].uvRect.w;
		radians = {};
		flipX = true;

		indexAtContainer = scene->monsters.len - 1;
		assert(scene->monsters[indexAtContainer].pointer == this);

		scene->physMonsters->Add(this);

		// 初始化数据面板
		healthMaxDefault = 100.f;
		PropsInit();
		dodgePreset = dodgeFactor;	// 测试下闪避效果。 1/2 概率挨打
		PropsCalc();
	}

	void Monster::Update() {
		// todo: 万一卡怪? 整个长超时自杀?

		// 判断是否掉入岩浆. 是：自杀
		if (scene->TryGetCrossLava(pos, radius)) {
			// todo: 特效？
			Dispose();
			return;
		}

		// 驱动怪身上挂的 dots. 如果导致已 Dispose 就直接 return ( 指针已野 )
		if (DotsUpdate(this)) return;

		// todo: 变速
		static constexpr float cMoveSpeed{ 200.f };

		if (leader) {
			// 如果有队长，就用自己队内编号，找队长要目标坐标, 根据坐标来算前进方向
			auto tar = leader->GetTargetPos(this);
			auto d = tar - pos;
			auto mag2 = d.x * d.x + d.y * d.y;
			if (mag2 > 0.001f) {
				auto norm = d / std::sqrtf(mag2);
				// 如果距离过远 就适当加速
				if (mag2 > 5) norm *= 3;
				scene->physMonsters->At(this).accel += norm * cMoveSpeed;
			}
		}
		else {
			// 向前移动( 物理附加 加速度 )
			scene->physMonsters->At(this).accel += XY{ -cMoveSpeed, 0 };
		}


		// 切帧动画
		frameIndex += (15.f / gg.cFps);
		if (frameIndex >= frameIndexRange.to) {
			frameIndex = frameIndexRange.from;
		}

	}

	void Monster::Draw() {
		XY s{ scale * scene->cam.scale };
		if (flipX) s.x = -s.x;
		float cp{ 1 };
		if (scene->time < whiteColorEndTime) cp = 10000.f;
		gg.Quad().DrawFrame(gg.pics.creature_1_[frameIndex], scene->cam.ToGLPos(pos)
			, s, radians, cp);
	}

	void Monster::DrawHPBar() {
		// 显示触发条件：带属性, 非满血
		if (health == healthMax) return;
		auto percent = (float)health / healthMax;
		auto& f = gg.pics.creature_1_[frameIndex];
		XY siz{ 15 * scale, 9 };
		auto p = pos + XY{ -siz.x * 0.5f, (f.uvRect.h * f.anchor.y + 1) * scale };
		siz *= scene->cam.scale;
		if (siz.x < 10) siz.x = 10;
		if (siz.y < 4) siz.y = 4;
		gg.HPBar().Alloc()->Fill(scene->cam.ToGLPos(p), siz, xx::RGBA8_Black, xx::RGBA8_White, xx::RGBA8_Red, percent);
	}

	void Monster::DrawLight() {
		gg.Quad().DrawFrame(gg.pics.c64_light, scene->cam.ToGLPos(pos)
			, (radius * (6.f / 64.f)) * scene->cam.scale, 0, 0.5f);
	}

	void Monster::DrawShadow() {
		gg.Quad().DrawFrame(gg.pics.c64_shadow, scene->cam.ToGLPos(pos)
			, (radius * (1.f / 40.f)) * scene->cam.scale, 0, 1.f, { 188,188,188,188 });
	}

	void Monster::Dispose() {
		assert(scene);
		assert(!disposing);
		assert(indexAtContainer != -1);
		auto& container = scene->monsters;
		assert(container[indexAtContainer].pointer == this);

		// 设置标记
		disposing = true;

		// 进一步释放资源
		scene->physMonsters->Remove(this);

		// 调用回调函数
		CallDisposeCallbacks();

		// 从容器中移除对象( 释放内存 )
		auto i = indexAtContainer;
		container.Back()->indexAtContainer = i;
		indexAtContainer = -1;
		container.SwapRemoveAt(i);
	}

	std::pair<float, int> Monster::Hurt(float attackValue_) {
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

	void Monster::Init(MonsterLeader* leader_) {
		indexAtMembers = leader_->memberOffsets.len - 1;
		leader = xx::WeakFromThis(leader_);
		auto p = leader_->GetTargetPos(this);
		Init(leader_->scene, p);
	}

	void MonsterLeader::Init(Scene* scene_, XY pos_, int32_t formationId_) {
		Monster::Init(scene_, pos_, cBossRadius);
		typeId = cTypeId;
		// 稍微放大点显示 避免周围太空
		scale *= 1.2f;

		// 再次初始化数据面板( 覆盖一些数据 )
		healthMaxDefault = 2000.f;
		PropsInit();
		PropsCalc();

		// todo: 根据 formationId_ 填充 memberOffsets
		// 顺便直接创建队员？？
		switch (formationId_) {
		case 0:
			// 一字长蛇阵? 以怪半径为 margin
			for (int32_t i = 0; i < 5; i++) {
				memberOffsets.Emplace(0, radius + cMonsterRadius + cMonsterRadius * 3 * i);
				scene->monsters.Emplace().Emplace()->Init(this);
				memberOffsets.Emplace(0, -radius - cMonsterRadius - cMonsterRadius * 3 * i);
				scene->monsters.Emplace().Emplace()->Init(this);
			}
			break;
		default:
			assert(false);
			break;
		}
	}

	XY MonsterLeader::GetTargetPos(Monster* member_) const {
		return memberOffsets[member_->indexAtMembers] + pos;
	}

}
