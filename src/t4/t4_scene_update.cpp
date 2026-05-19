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

		// todo: 鼠标左键 画 wall，右键 挖 wall

		auto mp = cam.ToLogicPos(gg.mousePos);
		if (gg.mouse[GLFW_MOUSE_BUTTON_5](0.1f)) {
			xx::CoutN(mp);
		}
		// 确保鼠标点击的位置是地图内部, 避开外圈墙壁
		if (mp.x > cCellPixelSize && mp.x < mapPixelSize.x - cCellPixelSize
			&& mp.y > cCellPixelSize && mp.y < mapPixelSize.y - cCellPixelSize) {
			XYi cr = mp * c1_CellPixelSize;
			if (gg.mouse[GLFW_MOUSE_BUTTON_1]) {
				GenWall(cr);
			}
			if (gg.mouse[GLFW_MOUSE_BUTTON_2]) {
				DigWall(cr);
			}
		}

	}

}
