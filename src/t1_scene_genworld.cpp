#include "pch.h"
#include "t1_.h"

namespace Test1 {

	void Scene::GenWorld() {
		// 　：怪活动区域
		// 墙：容器外边框，避免怪物挤出去
		// 弓：弓箭手布置区
		// 进：怪从天降区
		// 出：回头怪离开区
		// 浆：岩浆，能杀死部分怪 或 导致部分怪回头
		std::u32string_view mapText = UR"(
墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙
墙弓弓浆　　　　　　　　　进进出墙
墙弓弓浆　　　　　　　　　进进出墙
墙弓弓浆　　　　　　　　　进进出墙
墙弓弓浆　　　　　　　　　进进出墙
墙弓弓浆　　　　　　　　　进进出墙
墙弓弓浆　　　　　　　　　进进出墙
墙弓弓浆　　　　　　　　　进进出墙
墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙
)";

		// 移除首行空换行
		while (true) {
			assert(!mapText.empty());
			if (auto c = mapText[0]; c == U'\r' || c == U'\n') {
				mapText = mapText.substr(1, mapText.size() - 1);
				continue;
			}
			break;
		}

		// 移除尾行空换行
		while (true) {
			assert(!mapText.empty());
			if (auto c = mapText.back(); c == U'\r' || c == U'\n') {
				mapText = mapText.substr(0, mapText.size() - 1);
				continue;
			}
			break;
		}

		// 计算总宽度
		int32_t mapWidth{};
		for (int i = 0, e = (int)mapText.size(); i < e; ++i) {
			if (auto c = mapText[i]; c == U'\r' || c == U'\n')
				break;
			++mapWidth;
		}

		// 计算总高度
		int32_t mapHeight{ 1 };
		int32_t x{};
		for (int i = 0, e = (int)mapText.size(); i < e; ++i) {
			switch (auto c = mapText[i]) {
			case U'\r':
				continue;
			case U'\n':
				assert(x == mapWidth);	// 确保每一行长度一致
				x = 0;
				++mapHeight;
				continue;
			}
			++x;
		}

		// 预处理，规范数据内容，扫文本 翻译成对象类型编号二维数组
		mapData.Resize<true, 0>(mapWidth * mapHeight);	// 默认填 0 代表空格
		x = 0;
		int32_t y{};
		for (int i = 0, e = (int)mapText.size(); i < e; ++i) {
			switch (auto c = mapText[i]) {
			case U'\r': continue;
			case U'\n':
				x = 0;
				++y;
				continue;
			case U'墙':
			case U'进':
			case U'出':
			case U'浆':
			case U'弓':
				mapData[y * mapWidth + x] = c; break;
			}
			++x;
		}

		mapSize = { mapWidth, mapHeight };
		mapPixelSize = mapSize * cCellPixelSize;
		cam.Init(gg.scale, 1.f, mapPixelSize / 2);
		sortContainer.Resize<true>((int32_t)mapPixelSize.y);
		gridWalls.Init(cCellPixelSize, mapHeight, mapWidth);
		gridLavas.Init(cCellPixelSize, mapHeight, mapWidth);
		physMonsters.Emplace()->Init(this);
		floorMaskTex.Emplace()->Make(mapPixelSize);

		// 逐行扫内容并 各种预生成 / 填充
		for (int32_t y = 0; y < mapHeight; y++) {
			for (x = 0; x < mapWidth; x++) {
				auto i = y * mapWidth + x;
				switch (mapData[i]) {
				case U'墙':
				{
					// 计算出左上角坐标
					auto p = XY{ x, y } * cCellPixelSize;
					// 创建墙壁主体
					walls.Emplace().Emplace()->Init(this, p + cCellPixelHalfSize);
					// 判断左边和上边如果也有，在中缝补一个墙增加密度
					if (x > 0 && mapData[i - 1] == U'墙') {
						walls.Emplace().Emplace()->Init(this, p + XY{ 0, cCellPixelHalfSize });
					}
					if (y > 0 && mapData[i - mapWidth] == U'墙') {
						walls.Emplace().Emplace()->Init(this, p + XY{ cCellPixelHalfSize, 0 });
					}
					// 岩浆插值
					if (x > 0 && mapData[i - 1] == U'浆') {
						lavas.Emplace().Emplace()->Init(this, p + XY{ 0, cCellPixelHalfSize });
					}
					if (y > 0 && mapData[i - mapWidth] == U'浆') {
						lavas.Emplace().Emplace()->Init(this, p + XY{ cCellPixelHalfSize, 0 });
					}

					break;
				}
				case U'浆':
				{
					// 计算出左上角坐标
					auto p = XY{ x, y } * cCellPixelSize;
					// 创建墙壁主体
					lavas.Emplace().Emplace()->Init(this, p + cCellPixelHalfSize);
					// 判断左边和上边如果也有，在中缝补一个进增加密度
					if (x > 0 && mapData[i - 1] == U'浆' || mapData[i - 1] == U'墙') {
						lavas.Emplace().Emplace()->Init(this, p + XY{ 0, cCellPixelHalfSize });
					}
					if (y > 0 && mapData[i - mapWidth] == U'浆' || mapData[i - mapWidth] == U'墙') {
						lavas.Emplace().Emplace()->Init(this, p + XY{ cCellPixelHalfSize, 0 });
					}
					break;
				}
				case U'出':
					outletPoss.Emplace(x, y);
					break;
				case U'弓':
					archerPoss.Emplace(x, y);
					break;
				case U'进':
					enterPoss.Emplace(x, y);
					break;
				}
			}
		}

	}

}
