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
			// 根据上个节点在 pathway 的下标, 两节点半径和的比例，计算出自己在 pathway 上的下标 相对于上个节点的偏移量
			auto cursorOffset = (int32_t)((prev->radius + radius) * cNodeDistanceRatio / owner->pathway->stepDistance);
			assert(cursorOffset > 0);
			pathwayCursor = prev->pathwayCursor + cursorOffset;
			// 如果越界了就绕回去
			auto ps = (int32_t)owner->pathway->points.size();
			while (pathwayCursor >= ps) {
				pathwayCursor -= ps;
			}
		}
		// 头部
		else {
			pathwayCursor = 0;
		}
		// 根据在 pathway 上的下标，计算出自己的位置和朝向
		auto& p = owner->pathway->points[pathwayCursor];
		pos = p.pos;
		y = pos.y;
		radians = -p.radians;
		// 添加到蛇的 grid 索引中
		owner->scene->gridSnakes.Add(indexAtGrid, this);
	}

	void SnakeElement::Remove() {
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
		auto prev = GetPrev();
		assert(prev);
		// 根据上个节点在 pathway 的下标, 两节点半径和的比例，计算出自己在 pathway 上的下标 相对于上个节点的偏移量
		auto cursorOffset = (int32_t)((prev->radius + radius) * cNodeDistanceRatio / owner->pathway->stepDistance);
		assert(cursorOffset > 0);
		// 算出理论下标( 可能会越界 但用于计算偏差没问题 )
		auto cursor = prev->pathwayCursor + cursorOffset;
		auto ps = (int32_t)owner->pathway->points.size();
		// 如果实际下标距离理论下标在 cFollowDistance 对应的下标距离内，直接 follor ( 使用理论下标 )
		if (std::abs((cursor - pathwayCursor)) < int32_t(cFollowDistance / owner->pathway->stepDistance)) {
			pathwayCursor = cursor;
			// 如果越界了就绕回去
			while (pathwayCursor >= ps) {
				pathwayCursor -= ps;
			}
		}
		else {
			// 追赶模式
			pathwayCursor -= int32_t(cFollowSpeed / owner->pathway->stepDistance);
			// 如果越界了就绕回去
			while (pathwayCursor < 0) {
				pathwayCursor += ps;
			}
		}
		auto& p = owner->pathway->points[pathwayCursor];
		pos = p.pos;
		y = pos.y;
		radians = p.radians;
		// 同步蛇的 grid 索引
		owner->scene->gridSnakes.Update(indexAtGrid, this);
	}

	void SnakeElement::DrawLight() {
		gg.Quad().DrawFrame(gg.pics.c64_light, scene->cam.ToGLPos(pos)
			, radius * 8.f / 64.f * scene->cam.scale, 0, 1.f);
	}

	/***********************************************************************************/

	void Snake::Init(Scene* scene_, Pathway* pathway_, int32_t bodyLen_) {
		assert(elements.Empty());
		assert(bodyLen_ > 0);
		scene = scene_;
		pathway = pathway_;

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
				elements[i]->Remove();
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
		assert(f1.tex == f2.tex);
		assert(f1.uvRect.w == f2.uvRect.w);

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

	int32_t SnakeBody::ElementUpdate() {
		//U1_RadiusAnim();
		BaseUpdate();
		return 0;
	}

	void SnakeBody::Draw() {
		auto& f = gg.pics.snake_body;
		auto& c = owner->scene->cam;
		gg.Quad().DrawFrame(f, c.ToGLPos(pos)
			, radius * (2.f * cDrawScale) / f.uvRect.w * c.scale
			, radians);
	}

	/***********************************************************************************/

	void SnakeTail::Init() {
		SnakeElement::Init(cTailSize);
	}

	int32_t SnakeTail::ElementUpdate() {
		// todo: pause move support
		pathwayCursor += int32_t(cMoveSpeed / owner->pathway->stepDistance);
		auto ps = (int32_t)owner->pathway->points.size();
		while (pathwayCursor >= ps) {
			pathwayCursor -= ps;
		}
		auto& p = owner->pathway->points[pathwayCursor];
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
