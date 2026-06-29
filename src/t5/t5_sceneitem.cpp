#include "pch.h"
#include "t5.h"

namespace Test5 {

	void SceneItem::CallDisposeCallbacks() {
		for (auto i = disposeCallbacks.len - 1; i >= 0; --i) {
			disposeCallbacks[i]();
		}
	}


	void GridCache::operator=(SceneItem* p) {
		pos = p->pos;
		radius = p->radius;
	}

	void GridCache::operator=(Pot* p) {
		pos.x = p->pos.x;
		pos.y = p->pos.y - p->radius * 0.5f;
		radius = p->radius;
	}

}
