#include "pch.h"
#include "t3.h"
#include "mm_scene.h"

namespace Test3 {

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
		effectTexts.Update(time);
		UpdateItems(exploders);
		// 为了公平起见，生物的更新顺序每帧都反过来，这样就不会总是先更新某个阵营的生物了
		if (flag) {
			for (auto& oo : creaturess) {
				UpdateItems(oo);
				for (auto& o : oo) o->weapon->Update();
			}
		}
		else {
			for (auto i = creaturess.len - 1; i >= 0; --i) {
				auto& oo = creaturess[i];
				UpdateItems(oo);
				for (auto& o : oo) o->weapon->Update();
			}
		}
		flag = !flag;
		physCreatures.Step();
		UpdateItems(snakes);

		auto mp = cam.ToLogicPos(gg.mousePos);
		if (gg.mouse[GLFW_MOUSE_BUTTON_5](0.1f)) {
			xx::CoutN(mp);
		}
		// 确保鼠标点击的位置是地图内部, 避开外圈墙壁
		if (mp.x > cCellPixelSize && mp.x < mapPixelSize.x - cCellPixelSize
			&& mp.y > cCellPixelSize && mp.y < mapPixelSize.y - cCellPixelSize) {
			XYi cxy = mp * c1_CellPixelSize;
			if (gg.mouse[GLFW_MOUSE_BUTTON_1]) {
				creaturess[0].Emplace().Emplace()->Init(this, mp, 0);
			}
			if (gg.mouse[GLFW_MOUSE_BUTTON_2]) {
				creaturess[1].Emplace().Emplace()->Init(this, mp, 1);
			}
			if (gg.keyboard[GLFW_KEY_1]) {
				for (int32_t i = 0; i < 50; ++i) {
					XY offset{ gg.rnd.Next<float>(-11,11), gg.rnd.Next<float>(-11,11) };
					creaturess[0].Emplace().Emplace()->Init(this, mp + offset, 0);
				}
			}
			if (gg.keyboard[GLFW_KEY_2]) {
				for (int32_t i = 0; i < 50; ++i) {
					XY offset{ gg.rnd.Next<float>(-11,11), gg.rnd.Next<float>(-11,11) };
					creaturess[1].Emplace().Emplace()->Init(this, mp + offset, 1);
				}
			}
		}
	}

}
