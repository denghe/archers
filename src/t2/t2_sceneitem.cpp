#include "pch.h"
#include "t2.h"

namespace Test2 {

	void SceneItem::CallDisposeCallbacks() {
		for (auto i = disposeCallbacks.len - 1; i >= 0; --i) {
			disposeCallbacks[i]();
		}
	}

}
