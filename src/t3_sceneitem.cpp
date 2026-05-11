#include "pch.h"
#include "t3.h"

namespace Test3 {

	void SceneItem::CallDisposeCallbacks() {
		for (auto i = disposeCallbacks.len - 1; i >= 0; --i) {
			disposeCallbacks[i]();
		}
	}

	void SnakeElementGridCache::operator=(SnakeElement const* o) {
		elementType = o->elementType;
		pos = o->pos;
		radius = o->radius;
	}

}
