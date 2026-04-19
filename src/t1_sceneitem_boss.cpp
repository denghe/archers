#include "pch.h"
#include "t1_.h"

namespace Test1 {

	void Boss::Init(Scene* scene_, XY pos_) {
		Monster::Init(scene_, pos_, cBossRadius);
		typeId = cTypeId;
		// boss 怪稍微放大点显示 避免周围太空
		scale *= 1.2f;

		// 再次初始化数据面板( 覆盖一些数据 )
		healthMaxDefault = 2000.f;
		PropsInit();
		PropsCalc();
	}

}
