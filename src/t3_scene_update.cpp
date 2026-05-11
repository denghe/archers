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
				static constexpr float cMouseHitRange{ 16.f };
				auto cri = gridSnakes.PosToCRIndex(mp);
				gridSnakes.ForeachBy9(cri.y, cri.x, [this, mp](decltype(gridSnakes)::Node& o, float range) {
					if (o.value->HitCheck(mp, cMouseHitRange)) {
						effectTexts.Add(o.cache.pos, { 0, -1 * cam.scale }, xx::RGBA8_Red, 2.f, 123);
						o.value->Remove();	// unsafe
					}
				});
			}
			//if (gg.mouse[GLFW_MOUSE_BUTTON_2]) {
			//	static constexpr int32_t cHitRange{ 256 };
			//	auto cInnerRange = cHitRange - gridSnakes.cellSize * 0.5f * 1.4142f;
			//	auto mp = cam.ToLogicPos(gg.mousePos);
			//	auto cri = gridSnakes.PosToCRIndex(mp);
			//	gridSnakes.ForeachByRange(cri.y, cri.x, cHitRange, gg.sgrdd, [this, mp, cInnerRange](decltype(gridSnakes)::Node& o, float range) {
			//	});
			//}
		}
	}

}
