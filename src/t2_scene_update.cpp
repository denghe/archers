#include "pch.h"
#include "t2.h"
#include "mm_scene.h"

namespace Test2 {

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
		for (auto& oo : creaturess) {
			UpdateItems(oo);
		}
		physCreatures.Step();
		for (auto& oo : creaturess) {
			for (auto& o : oo) o->weapon->Update();
		}
		effectTexts.Update(time);

		auto mp = cam.ToLogicPos(gg.mousePos);
		if (gg.mouse[GLFW_MOUSE_BUTTON_3](0.1f)) {
			xx::CoutN(mp);
		}
		// 确保鼠标点击的位置是地图内部, 避开外圈墙壁
		if (mp.x > cCellPixelSize && mp.x < mapPixelSize.x - cCellPixelSize
			&& mp.y > cCellPixelSize && mp.y < mapPixelSize.y - cCellPixelSize) {
			XYi cxy = mp * c1_CellPixelSize;
			if (gg.mouse[GLFW_MOUSE_BUTTON_1](0.02f)) {
				creaturess[0].Emplace().Emplace()->Init(this, mp, 0);
			}
			if (gg.mouse[GLFW_MOUSE_BUTTON_2](0.02f)) {
				creaturess[1].Emplace().Emplace()->Init(this, mp, 1);
			}
		}
	}

}
