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
	}

}
