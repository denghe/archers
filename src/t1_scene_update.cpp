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
		if (gg.mouse[GLFW_MOUSE_BUTTON_3](0.1f)) {
			xx::CoutN(mp);
		}
		// 确保鼠标点击的位置是地图内部, 避开外圈墙壁
		if (mp.x > cCellPixelSize && mp.x < mapPixelSize.x - cCellPixelSize
			&& mp.y > cCellPixelSize && mp.y < mapPixelSize.y - cCellPixelSize) {
			XYi cxy = mp * c1_CellPixelSize;
			if (gg.mouse[GLFW_MOUSE_BUTTON_1]) {
				// 进一步判断鼠标点击的部位
				// 弓位
				if (archerPoss.Find(cxy) != -1) {
					GenArchers(1);
				}
				// 进怪位
				else if (enterPoss.Find(cxy) != -1) {
					for (size_t i = 0; i < 5; i++) {
						monsters.Emplace().Emplace()->Init(this, mp);
					}
				}
			}
			// BOSS
			if (gg.mouse[GLFW_MOUSE_BUTTON_2](0.1f)) {
				// 进一步判断鼠标点击的部位
				// 弓位
				if (archerPoss.Find(cxy) != -1) {
					GenArchers(100);
				}
				// 进怪位
				if (enterPoss.Find(cxy) != -1) {
					// 创建一个怪物队伍
					monsters.Emplace().Emplace<MonsterLeader>()->Init(this, mp, 0);
				}
			}
		}
	}

}
