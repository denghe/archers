#include "pch.h"
#include "t1_.h"

namespace Test1 {

	void SceneItem::CallDisposeCallbacks() {
		for (auto i = disposeCallbacks.len - 1; i >= 0; --i) {
			disposeCallbacks[i]();
		}
	}

}
