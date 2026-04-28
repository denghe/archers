#include "pch.h"
#include "t2.h"

namespace Test2 {

	void Scene::GenWorld() {
		// 填充地图数据，地图文本内容可以直接在这里编辑，方便调整
		// 最终要填充 mapData mapSize


		// 　：怪活动区域
		// 墙：容器外边框，避免怪物挤出去
		std::u32string_view mapText = UR"(
墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　墙
墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙
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
				mapData[y * mapWidth + x] = c; break;
			}
			++x;
		}

		mapSize = { mapWidth, mapHeight };

	}

}
