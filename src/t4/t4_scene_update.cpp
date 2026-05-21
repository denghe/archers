#include "pch.h"
#include "t4.h"
#include "mm_scene.h"

namespace Test4 {

	void Scene::Update() {
		// handle inputs
		if (gg.keyboard[GLFW_KEY_ESCAPE](0.2f)) {
			gg.MakeScene<MainMenu::Scene>()->Init();
			return;
		}

		// fixed update
		auto d = float(std::min((float)gg.delta, gg.cMaxDelta) * timeScale);
		timePool += d;
		while (timePool >= gg.cDelta) {
			time += gg.cDelta;
			timePool -= gg.cDelta;
			FixedUpdate();
		}
	}

	void Scene::FixedUpdate() {
		// 从 UI 同步数据
		for (auto& o : walls) {
			o->wallsIndex = cWallTexIndex;
		}

		//effectTexts.Update(time);
		//UpdateItems(exploders);
		if (player) player->Update();

		auto mp = cam.ToLogicPos(gg.mousePos);
		// 确保鼠标点击的位置是地图内部, 避开 ui & 外圈墙壁
		if (!gg.uiHandler
			&& mp.x > cCellPixelSize && mp.x < mapPixelSize.x - cCellPixelSize
			&& mp.y > cCellPixelSize && mp.y < mapPixelSize.y - cCellPixelSize) {
			// 鼠标左键 画 wall，右键 挖 wall
			XYi cr = mp * c1_CellPixelSize;
			if (gg.mouse[GLFW_MOUSE_BUTTON_1]) {
				GenWall(cr);
			}
			if (gg.mouse[GLFW_MOUSE_BUTTON_2]) {
				DigWall(cr);
			}
			if (gg.mouse[GLFW_MOUSE_BUTTON_3](0.2f)) {
				SetWallOre(cr);
			}
		}

	}

}
