#include "pch.h"
#include "t5.h"

namespace Test5 {

	void Scene::GenWall(XYi cr_) {
		assert(gridWalls.pixelSize == mapPixelSize);
		auto idx = cr_.y * mapSize.x + cr_.x;
		if (mapData[idx] == U'墙') return;
		mapData[idx] = U'墙';
		walls.Emplace().Emplace()->Init(this, cr_);
		// 更新当前格 + 周围 8 格
		gridWalls.ForeachBy9(cr_.y, cr_.x, [](decltype(gridWalls)::Node& o, float range)->void {
			o.value->FillTilesIndex();
		});
	}

	void Scene::DigWall(XYi cr_) {
		assert(gridWalls.pixelSize == mapPixelSize);
		auto idx = cr_.y * mapSize.x + cr_.x;
		if (mapData[idx] == 0) return;
		mapData[idx] = 0;
		gridWalls.nodes[gridWalls.buckets[idx]].value->Dispose();
		// 更新当前格 + 周围 8 格
		gridWalls.ForeachBy9(cr_.y, cr_.x, [](decltype(gridWalls)::Node& o, float range)->void {
			o.value->FillTilesIndex();
		});
	}

	void Scene::SetWallOre(XYi cr_) {
		assert(gridWalls.pixelSize == mapPixelSize);
		auto idx = cr_.y * mapSize.x + cr_.x;
		if (mapData[idx] == 0) return;
		gridWalls.nodes[gridWalls.buckets[idx]].value->RndOre();
	}

}
