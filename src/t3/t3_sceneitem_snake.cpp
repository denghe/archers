#include "pch.h"
#include "t3.h"

namespace Test3 {

	void SnakeElement::PreInit(Snake* owner_, SnakeElementTypes elementType_) {
		scene = owner_->scene;
		owner = owner_;
		elementType = elementType_;
		indexAtContainer = owner_->elements.len - 1;
	}

	void SnakeElement::Init(float radius_) {
		radius = radius_;
		// 身体或尾巴
		if (indexAtContainer > 0) {
			auto prev = GetPrev();
			assert(prev);
			// 根据两节点半径和的比例，计算出自己相对于上个节点的偏移量
			auto cursorOffset = (prev->radius + radius) * cNodeDistanceRatio * owner->pathway_1_stepDistance;
			assert(cursorOffset > 0);
			pathwayCursor = prev->pathwayCursor + cursorOffset;
			// 如果越界了就绕回去
			auto ps = (float)owner->pathway->points.size();
			if (pathwayCursor >= ps) {
				pathwayCursor -= ps;
				assert(pathwayCursor < ps);
			}
		}
		// 头部
		else {
			pathwayCursor = 0;
		}
		// 根据在 pathway 上的下标，计算出自己的位置和朝向
		auto& p = owner->pathway->points[(int32_t)pathwayCursor];
		pos = p.pos;
		y = pos.y;
		radians = -p.radians;
		// 添加到蛇的 grid 索引中
		owner->scene->gridSnakes.Add(indexAtGrid, this);
	}

	void SnakeElement::Dispose() {
		// 从蛇的 grid 索引中移除
		assert(owner->elements[indexAtContainer].pointer == this);
		owner->scene->gridSnakes.Remove(indexAtGrid, this);	// grid sync
		// 同步填充后面节点的 indexAtContainer
		auto& es = owner->elements;
		for (auto i = indexAtContainer + 1; i < es.len; ++i) {
			es[i]->indexAtContainer = i - 1;
		}
		auto idx = indexAtContainer;
		indexAtContainer = -1;
		// 从父容器移除自身( 会造成对象删除, this 内存失效 )
		es.RemoveAt(idx);
	}

	bool SnakeElement::HitCheck(XY p_, float hitRadius_) {
		// 当前逻辑设定中，头尾不参与 hit
		if (elementType == SnakeElementTypes::Head || elementType == SnakeElementTypes::Tail)
			return false;
		auto d = p_ - pos;
		auto r = radius + hitRadius_;
		return (d.x * d.x + d.y * d.y < r * r);
	}

	SnakeElement* SnakeElement::GetPrev() {
		assert(owner->elements[indexAtContainer].pointer == this);
		if (indexAtContainer == 0) return {};
		return owner->elements[indexAtContainer - 1].pointer;
	}

	SnakeElement* SnakeElement::GetNext() {
		assert(owner->elements[indexAtContainer].pointer == this);
		if (indexAtContainer == owner->elements.len - 1) return {};
		return owner->elements[indexAtContainer + 1].pointer;
	}

	void SnakeElement::BaseUpdate() {
		auto ps = (float)owner->pathway->points.size();
		auto prev = GetPrev();
		assert(prev);
		// 根据两节点半径和的比例，计算出自己相对于上个节点的偏移量
		auto cursorOffset = (prev->radius + radius) * cNodeDistanceRatio * owner->pathway_1_stepDistance;
		assert(cursorOffset > 0);
		// 算出理论下标
		auto cursor = prev->pathwayCursor + cursorOffset;
		// 判断 cursor & pathwayCursor 前后关系
		float d{};
		// 当前节点已卷回最初索引?
		if (prev->pathwayCursor > pathwayCursor) {
			d = pathwayCursor + ps - cursor;
		}
		// 正常计算
		else {
			d = cursor - pathwayCursor;
		}
		// 如果实际下标距离理论下标在 cFollowDistance 对应的下标距离内，直接使用理论下标( 正常模式 )
		if (std::fabsf(d) < cFollowDistance * owner->pathway_1_stepDistance) {
			pathwayCursor = cursor;
			// 如果越界了就绕回去
			if (pathwayCursor >= ps) {
				pathwayCursor -= ps;
				assert(pathwayCursor < ps);
			}
		}
		else {
			// 追赶模式
			pathwayCursor -= cLinkSpeed * owner->pathway_1_stepDistance;
			// 如果越界了就绕回去
			if (pathwayCursor < 0) {
				pathwayCursor += ps;
				assert(pathwayCursor >= 0);
			}
		}
		// 根据 pathway 填充坐标, 角度啥的
		auto& p = owner->pathway->points[(int32_t)pathwayCursor];
		pos = p.pos;
		y = pos.y;
		radians = p.radians;
		// 同步蛇的 grid 索引
		owner->scene->gridSnakes.Update(indexAtGrid, this);
	}

	void SnakeElement::DrawLight() {
		gg.Quad().DrawFrame(gg.pics.c64_light, scene->cam.ToGLPos(pos)
			, radius * 8.f / 64.f * scene->cam.scale, 0, 0.7f);
	}

	/***********************************************************************************/

	void Snake::Init(Scene* scene_, xx::MovePathCache* pathway_, int32_t bodyLen_) {
		assert(elements.Empty());
		assert(bodyLen_ > 0);
		scene = scene_;
		pathway = pathway_;
		pathway_1_stepDistance = 1.f / pathway->stepDistance;

		elements.Emplace().Emplace<SnakeTail>()->PreInit(this, SnakeElementTypes::Tail);
		for (int32_t i = 0; i < bodyLen_; ++i) {
			elements.Emplace().Emplace<SnakeBody>()->PreInit(this, SnakeElementTypes::Body);
		}
		elements.Emplace().Emplace<SnakeHead>()->PreInit(this, SnakeElementTypes::Head);

		((SnakeTail*)elements[0].pointer)->Init();
		for (int32_t i = 1; i <= bodyLen_; ++i) {
			((SnakeBody*)elements[i].pointer)->Init();
		}
		((SnakeHead*)elements[bodyLen_ + 1].pointer)->Init();
	}

	void Snake::Update() {
		assert(elements.len);
		for (auto i = 0; i < elements.len;) {
			if (elements[i]->ElementUpdate()) {
				elements[i]->Dispose();
			}
			else {
				++i;
			}
		}
		if (elements.Empty()) {
			Dispose();
		}
	}

	/***********************************************************************************/

	void SnakeHead::Init() {
		faceShaker.Shake(5, 300.f * gg.cDelta, 99999999);
		SnakeElement::Init(cHeadSize);
	}

	int32_t SnakeHead::ElementUpdate() {
		faceShaker.Update(gg.rnd, gg.time);
		BaseUpdate();
		return 0;
	}

	void SnakeHead::Draw() {
		auto& f1 = gg.pics.snake_body;
		auto& f2 = gg.pics.snake_face;

		auto& c = owner->scene->cam;
		auto p = c.ToGLPos(pos);
		auto s = radius * (2.f * cDrawScale) / f1.uvRect.w * c.scale;
		auto& q = gg.Quad();
		q.DrawFrame(f1, p, s, radians);
		q.DrawFrame(f2, p + faceShaker.offset, s, radians);
	}

	/***********************************************************************************/

	void SnakeBody::Init() {
		auto d = cRadiusRange.to - cRadiusRange.from;
		auto len = owner->elements.len - 2;
		auto step = d / len;
		auto r = cRadiusRange.from + step * (indexAtContainer - 1);
		SnakeElement::Init(r);

		// 初始化数据面板
		healthMaxDefault = 100.f;
		PropsInit();
		PropsCalc();
	}

	void SnakeBody::U1_RadiusAnim() {
		// todo: cNodeDistanceRatio 的范围变化动画
		XX_BEGIN(U1_n);
	LabBig:
		for (; radius < cRadiusRange.to; radius += cRadiusAnimStep) {
			XX_YIELD(U1_n);
		}
		for (; radius > cRadiusRange.from; radius -= cRadiusAnimStep) {
			XX_YIELD(U1_n);
		}
		goto LabBig;
		XX_END(U1_n);
	}

	std::pair<float, int> SnakeBody::Hurt(float attackValue_) {
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

	int32_t SnakeBody::ElementUpdate() {
		//U1_RadiusAnim();
		BaseUpdate();
		return 0;
	}

	void SnakeBody::Draw() {
		auto& f = gg.pics.snake_body;
		auto& cam = owner->scene->cam;
		float cp{ 1 };
		if (scene->time < whiteColorEndTime) {
			cp = 10000.f;
		}
		gg.Quad().DrawFrame(f, cam.ToGLPos(pos)
			, radius * (2.f * cDrawScale) / f.uvRect.w * cam.scale
			, radians, cp);
	}

	void SnakeBody::DrawHPBar() {
		// 显示触发条件：带属性, 非满血
		if (health == healthMax) return;
		auto percent = (float)health / healthMax;
		auto& f = gg.pics.snake_body;
		auto s = scene->cam.scale;
		XY siz{ radius * s, 12 * s };
		if (siz.x < 20) siz.x = 20;
		if (siz.y < 6) siz.y = 6;
		auto p = scene->cam.ToGLPos(pos);
		p += XY{ -siz.x * 0.5f, -radius * s };
		gg.HPBar().Alloc()->Fill(p, siz, xx::RGBA8_Black, xx::RGBA8_White, { 200,0,0,255 }, percent);
	}

	/***********************************************************************************/

	void SnakeTail::Init() {
		SnakeElement::Init(cTailSize);
	}

	int32_t SnakeTail::ElementUpdate() {
		// todo: pause move support
		pathwayCursor += cMoveSpeed * owner->pathway_1_stepDistance;
		auto ps = (float)owner->pathway->points.size();
		while (pathwayCursor >= ps) {
			pathwayCursor -= ps;
		}
		auto& p = owner->pathway->points[(int32_t)pathwayCursor];
		pos = p.pos;
		y = pos.y;
		radians = p.radians;
		owner->scene->gridSnakes.Update(indexAtGrid, this);
		return 0;
	}

	void SnakeTail::Draw() {
		auto& f = gg.pics.snake_tail;
		auto& c = owner->scene->cam;
		gg.Quad().DrawFrame(f, c.ToGLPos(pos)
			, radius * (2.f * cDrawScale) / f.uvRect.w * c.scale
			, radians);
	}

}
