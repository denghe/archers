#include "pch.h"
#include "curves.h"

void curves::Init(float stepDistance_) {
	std::vector<xx::CurvePoint> tmpCps;
	xx::MovePath tmpMp;

	// c1;
	tmpCps.clear();
	tmpCps.push_back({ {-1132, 484}, 0.05, 100 });
	tmpCps.push_back({ {807, 444}, 0.05, 100 });
	tmpCps.push_back({ {812, 339}, 0.05, 100 });
	tmpCps.push_back({ {-826, 316}, 0.05, 100 });
	tmpCps.push_back({ {-824, 206}, 0.05, 100 });
	tmpCps.push_back({ {813, 198}, 0.05, 100 });
	tmpCps.push_back({ {815, 75}, 0.05, 100 });
	tmpCps.push_back({ {-824, 55}, 0.05, 100 });
	tmpCps.push_back({ {-824, -56}, 0.05, 100 });
	tmpCps.push_back({ {819, -71}, 0.05, 100 });
	tmpCps.push_back({ {821, -184}, 0.05, 100 });
	tmpCps.push_back({ {-826, -192}, 0.05, 100 });
	tmpCps.push_back({ {-824, -314}, 0.05, 100 });
	tmpCps.push_back({ {826, -348}, 0.05, 100 });
	tmpCps.push_back({ {832, -465}, 0.05, 100 });
	tmpCps.push_back({ {-1120, -502}, 0.05, 100 });
	tmpMp.Clear();
	tmpMp.FillCurve(false, tmpCps);
	this->c1.Init(tmpMp, stepDistance_);

}

xx::MovePathCache& curves::operator[](size_t index_) {
	assert(index_ < 1);
	return ((xx::MovePathCache*)this)[index_];
}
