#include "pch.h"
#include "t1.h"

namespace Test1 {

	// 搜索关键字：c语言 2d 炮轰直线飞行小球，计算提前量
	// 计算提前量，返回子弹 飞行时长 & 发射方向向量（未归一化）. 无解则返回 打击 target 当前坐标的数据
	std::pair<float, XY> CalculateLeadPoint(XY shooterPos_, XY targetPos_, XY targetVel_, float bulletSpeed_) {
		auto d = targetPos_ - shooterPos_;
		float A = targetVel_.x * targetVel_.x + targetVel_.y * targetVel_.y - bulletSpeed_ * bulletSpeed_;
		float B = 2.0 * (d.x * targetVel_.x + d.y * targetVel_.y);
		float C = d.x * d.x + d.y * d.y;

		float discriminant = B * B - 4 * A * C;
		if (discriminant < 0 || A == 0) return { d.Mag() / bulletSpeed_, targetPos_ };	// 无实数解或退化情况

		float sqrtDisc = std::sqrtf(discriminant);
		float t1 = (-B + sqrtDisc) / (2 * A);
		float t2 = (-B - sqrtDisc) / (2 * A);

		float t = -1;
		if (t1 >= 0.001f) t = t1;  // 避免除以极小值，设最小时间阈值
		if (t2 >= 0.001f && (t < 0.f || t2 < t)) t = t2;
		if (t < 0.f) return { d.Mag() / bulletSpeed_, targetPos_ }; // 无有效解

		// 计算命中点
		auto hitPos = targetPos_ + targetVel_ * t;

		// 返回炮弹 飞行时长& 发射方向向量（未归一化）
		return { t, hitPos - shooterPos_ };
	}

	void ArcherArrow2::Init(Archer* owner_, Monster* tar_) {
		typeId = cTypeId;
		scene = owner_->scene;
		owner = xx::WeakFromThis(owner_);

		indexAtContainer = scene->archerArrows.len - 1;
		assert(scene->archerArrows[indexAtContainer].pointer == this);

		// 保存目标的 weak ptr, 后续每帧都要检测目标是否存在
		target = xx::WeakFromThis(tar_);

		// 算落点
		auto tarVel = (tar_->pos - tar_->lastPos) * gg.cFps;
		auto p = CalculateLeadPoint(owner_->pos, tar_->pos, tarVel, cSpeed);
		tarPos = owner_->pos + p.second;
		if (tarPos.x < cCellPixelSize) tarPos.x = cCellPixelSize;
		if (tarPos.y < cCellPixelSize) tarPos.y = cCellPixelSize;
		if (tarPos.x >= scene->mapPixelSize.x - cCellPixelSize) tarPos.x = scene->mapPixelSize.x - cCellPixelSize;
		if (tarPos.y >= scene->mapPixelSize.y - cCellPixelSize) tarPos.y = scene->mapPixelSize.y - cCellPixelSize;
		auto d = tarPos - owner_->pos;
		auto mag = d.Mag();
		auto dir = d / mag;
		auto duration = mag / cSpeed;
		
		// 计算总飞行步数
		totalSteps = int32_t(duration * gg.cFps);
		assert(totalSteps > 0);
		// 计算每帧的移动步进值( 不考虑抛物线效果 )
		inc = dir * cSpeed / gg.cFps;
		yOffset = 0.f;
		// 计算飞行高度 todo: 这个系数感觉上应该和飞行距离有关
		maxY = mag * 0.3f;

		pos = owner_->pos;
		y = pos.y;
		radius = cPlayerRadius * 0.5f;
		scale = radius * 2.f / gg.pics.firearrow_[0].uvRect.h;
		radians = xx::Atan2(dir);
		pos2 = lastPos = pos;

		// 复制玩家当前数值面板值以便于算伤害
		*(Props2*)this = *(Props2*)owner;
	}

	bool ArcherArrow2::HitTarget(XY tarPos_, float tarRadius_, Monster* tar_) {
		// 开始碰撞判定
		auto d = tarPos_ - pos;
		d.y *= 2.0f;	// 椭圆效果
		auto mag2 = d.x * d.x + d.y * d.y;
		auto r = tarRadius_ + radius;
		auto rr = r * r;
		// 距离小于圆心和: 相交
		if (mag2 < rr) {
			// 防止怪物释放内存导致指针失效，先拿 weak ptr
			auto w = xx::WeakFromThis(tar_);
			// 伤害目标
			// 先算攻击力
			auto [atkVal, isCritical] = PropsCalcAttackValue(gg.rnd, baseDamage);
			// 得到实际造成的伤害
			auto [actualDmg, state] = w->Hurt(atkVal);
			if (state == 0) {
				assert(w);
				// 生成伤害数字特效( 暴击时颜色会不同 )
				scene->effectTexts.Add(pos, { 0, -1.f * (120.f / gg.cFps) }, isCritical ? xx::RGBA8_Red : xx::RGBA8_Yellow
					, 2, -actualDmg, true);
			}
			else if (state == 1) {
				assert(w);
				// todo: miss 的特效表达
			}
			else {
				assert(!w);
			}
			// 返回碰撞发生
			return true;
		}
		return false;
	}

	void ArcherArrow2::Update() {
		// 步进帧动画
		frameNumber += cFrameNumberInc;
		if (frameNumber >= gg.pics.firearrow_.size()) {
			frameNumber = 0.f;
		}

		XX_BEGIN(_1);
		// 飞行阶段
		for (numSteps = 0; numSteps < totalSteps; ++numSteps) {
			{
				// 抛物线公式: y = 1 - x*x. 阶段 1 抛物线 x 区间为 -1 ~ 1
				auto x = -1.f + 2.f * float(numSteps) / totalSteps;
				// 飞行附加高度 = 飞行最高点 * 抛物线公式值
				yOffset = -maxY * (1.f - x * x);
				// 移动
				pos2 += inc;
				// 排序变量不受 yOffset 影响
				y = pos2.y;
				// 更新显示坐标
				pos.x = pos2.x;
				pos.y = pos2.y + yOffset;
				// 更新角度
				auto d = pos - lastPos;
				lastPos = pos;
				radians = xx::Atan2(d);
			}
			XX_YIELD(_1);
		}
		// 落地
		pos2 = pos = tarPos;
		y = pos.y;
		XX_YIELD(_1);

		// 落地时碰撞检测: 
		// 先判断 target 是否还存在, 是否命中 target. 命中就 Dispose 掉. 没命中就继续往下检测
		if (target && HitTarget(target->pos, target->radius, target.pointer())) {
			Dispose();
			return;
		}
		// 查找子弹位置的怪
		bool hit{};
		auto cri = scene->physMonsters->PosToCRIndex(pos);
		scene->physMonsters->ForeachBy9Break(cri.y, cri.x, [&](PhysSystem::Node& o, float range)->bool {
			hit = HitTarget(o.cache.pos, o.cache.radius, (Monster*)o.value);
			return hit;
		});

		// todo: 弄点子弹命中地面的特效？ if (!hit) ...
		Dispose();
		XX_END(_1);
		return;
	}

	void ArcherArrow2::Draw() {
		gg.Quad().DrawFrame(gg.pics.firearrow_[frameNumber], scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians);
	}

	void ArcherArrow2::DrawLight() {
		gg.Quad().DrawFrame(gg.pics.c64_light, scene->cam.ToGLPos(pos)
			, (128.f / 64.f) * scene->cam.scale, 0, 0.5f);
	}

	void ArcherArrow2::Dispose() {
		assert(scene);
		assert(!disposing);
		assert(indexAtContainer != -1);
		auto& container = scene->archerArrows;
		assert(container[indexAtContainer].pointer == this);

		// 设置标记
		disposing = true;

		// 从容器中移除对象( 释放内存 )
		auto i = indexAtContainer;
		container.Back()->indexAtContainer = i;
		indexAtContainer = -1;
		container.SwapRemoveAt(i);
	}

}
