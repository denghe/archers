#include "pch.h"
#include "t3.h"

namespace Test3 {

	void SnakeElement::PreInit(Snake* owner_, SnakeElementTypes elementType_) {
		owner = owner_;
		elementType = elementType_;
		indexAtContainer = owner_->elements.len - 1;
	}

	void SnakeElement::Init(float radius_) {
		radius = radius_;
		if (indexAtContainer > 0) {
			auto prev = GetPrev();
			assert(prev);
			auto cursorOffset = (int32_t)((prev->radius + radius) * cNodeDistanceRatio / owner->pathway->stepDistance);
			assert(cursorOffset > 0);
			pathwayCursor = prev->pathwayCursor + cursorOffset;
			auto ps = (int32_t)owner->pathway->points.size();
			while (pathwayCursor >= ps) {
				pathwayCursor -= ps;
			}
		}
		else {
			pathwayCursor = 0;
		}
		auto& p = owner->pathway->points[pathwayCursor];
		pos = p.pos;
		radians = -p.radians;
		owner->scene->gridSnakes.Add(indexAtGrid, this);	// grid sync
	}

	void SnakeElement::Remove() {
		assert(owner->elements[indexAtContainer].pointer == this);
		owner->scene->gridSnakes.Remove(indexAtGrid, this);	// grid sync
		auto& es = owner->elements;
		for (auto i = indexAtContainer + 1; i < es.len; ++i) {
			es[i]->indexAtContainer = i - 1;
		}
		auto idx = indexAtContainer;
		indexAtContainer = -1;
		es.RemoveAt(idx);	// unsafe
	}

	bool SnakeElement::HitCheck(XY p_, float hitRadius_) {
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
		auto cursorOffset = (int32_t)((prev->radius + radius) * cNodeDistanceRatio / owner->pathway->stepDistance);
		assert(cursorOffset > 0);
		auto cursor = pathwayCursor;
		if (prev->pathwayCursor > pathwayCursor) {
			cursor += (int32_t)owner->pathway->points.size();
		}
		if (std::abs(cursorOffset - (cursor - prev->pathwayCursor))
			< int32_t((cSpeed * cStickRatio) / owner->pathway->stepDistance)) {
			// follow
			pathwayCursor = prev->pathwayCursor + cursorOffset;
		}
		else {
			// backward
			pathwayCursor -= int32_t(cSpeed / owner->pathway->stepDistance);
			while (pathwayCursor < 0) {
				pathwayCursor += (int32_t)owner->pathway->points.size();
			}
		}
		auto ps = (int32_t)owner->pathway->points.size();
		while (pathwayCursor >= ps) {
			pathwayCursor -= ps;
		}
		auto& p = owner->pathway->points[pathwayCursor];
		pos = p.pos;
		radians = p.radians;

		owner->scene->gridSnakes.Update(indexAtGrid, this);	// grid sync
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

	void Snake::Draw() {
		for (int32_t i = 0, e = elements.len; i < e; ++i) {
			elements[i]->Draw();
		}
	}

	void Snake::DrawLight() {
		for (int32_t i = 0, e = elements.len; i < e; ++i) {
			elements[i]->DrawLight();
		}
	}

	/***********************************************************************************/

	void SnakeHead::Init() {
		faceShaker.Shake(5, 300.f * gg.cDelta, 99999999);
		SnakeElement::Init(16.f);
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

	void SnakeHead::DrawLight() {
		// todo
	}

	/***********************************************************************************/

	void SnakeBody::Init() {
		auto d = cRadiusRange.to - cRadiusRange.from;
		auto len = owner->elements.len - 2;
		auto step = d / len;
		auto r = cRadiusRange.from + step * (indexAtContainer - 1);
		// auto r = gLooper.rnd.Next<float>(cRadius.from, cRadius.to);
		SnakeElement::Init(r);
	}

	void SnakeBody::U1_RadiusAnim() {
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
		U1_RadiusAnim();
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

	void SnakeBody::DrawLight() {
		// todo
	}

	/***********************************************************************************/

	void SnakeTail::Init() {
		SnakeElement::Init(16.f);
	}

	int32_t SnakeTail::ElementUpdate() {
		// todo: do not move?
		pathwayCursor += int32_t(cSpeed / owner->pathway->stepDistance);
		auto ps = (int32_t)owner->pathway->points.size();
		while (pathwayCursor >= ps) {
			pathwayCursor -= ps;
		}
		auto& p = owner->pathway->points[pathwayCursor];
		pos = p.pos;
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

	void SnakeTail::DrawLight() {
		// todo
	}


}
