#include "pch.h"
#include "t5.h"

namespace Test5 {

	void SceneItem::CallDisposeCallbacks() {
		for (auto i = disposeCallbacks.len - 1; i >= 0; --i) {
			disposeCallbacks[i]();
		}
	}

}
