#include <xx_curvemovepath.h>

struct curves {

	xx::MovePathCache c1; // 0: c1

	uint32_t designWidth{1920}, designHeight{1080}, safeLength{128};

	// fill contents
	void Init(float stepDistance_ = 1.f);

	// array access
	xx::MovePathCache& operator[](size_t index_);
};
