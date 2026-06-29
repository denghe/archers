#include "pch.h"
#include "t5.h"

namespace Test5 {

	void SceneItem::CallDisposeCallbacks() {
		for (auto i = disposeCallbacks.len - 1; i >= 0; --i) {
			disposeCallbacks[i]();
		}
	}


	void GridCache::operator=(SceneItem* o) {
		pos = o->pos;
		radius = o->radius;
	}

	//void GridCache::operator=(Player* o) {
	//	pos.x = o->pos.x;
	//	pos.y = o->pos.y - o->radius;
	//	radius = o->radius;
	//}

	void GridCache::operator=(Pot* o) {
		pos.x = o->pos.x;
		pos.y = o->pos.y - o->radius;
		radius = o->radius;
	}

}
