#include "pch.h"
#include "t1_.h"
#include "mm_scene.h"

namespace Test1 {

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
		UpdateItems(monsters);
		UpdateItems(archers);
		UpdateItems(archerArrows);
		UpdateItems(exploders);
		physMonsters->Step();
		effectTexts.Update(time);

		auto mp = cam.ToLogicPos(gg.mousePos);
		// 确保鼠标点击的位置是地图内部, 避开外圈墙壁
		if (mp.x > cCellPixelSize && mp.x < mapPixelSize.x - cCellPixelSize
			&& mp.y > cCellPixelSize && mp.y < mapPixelSize.y - cCellPixelSize) {
			// 鼠标左键：生成怪
			if (gg.mouse[GLFW_MOUSE_BUTTON_1]) {
				monsters.Emplace().Emplace()->Init(this, mp);
			}
		}

	}

}
