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
		UpdateItems(playerBullets);
		if (player) player->Update();

		if (gg.keyboard[GLFW_KEY_R](0.1f)) {
			GenSnake();
		}
	}

}
