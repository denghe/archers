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
		//effectTexts.Update(time);
		//UpdateItems(exploders);

		//// 从 UI 同步数据
		//if (player) {
		//	if (player->weapon) {
		//		player->weapon->baseDamage = cBulletAttack;
		//		player->weapon->shootDelay = 1.f / cBulletShootSpeed;
		//		player->weapon->shootCountPerRound = cBulletCount;
		//		player->weapon->pierceCount = cBulletPierceCount;
		//		player->weapon->bulletFlySpeed = cBulletFlySpeed;
		//	}
		//	player->criticalChance = cBulletCriticalChance / 100.f;
		//	player->criticalDamage = cBulletCriticalDamage / 100.f;
		//}
	}

}
