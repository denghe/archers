#include "pch.h"
#include "game.h"
#include "mm_scene.h"
#include "g_scenebase.h"
#include <3500_t0.h>

// global game instance
Global::Game gg;

// entry
int32_t main() {
	return gg.Run();
}

namespace Global {

	void Game::Init() {
		title = "archers";
		windowSize = designSize = cDesignSize;
		DisableIME();
	}

	void Game::GLInit() {
		// check some env args
		GLint maxTextureSize;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
		xx::CoutN("maxTextureSize = ", maxTextureSize);

		// init env
		sgrdd.Init(64, 64);

		// init curves
		curves.Init();

		// begin load res
		picsTex = pics.Load("res/_pics.png");

		// init tiled maps data for walls
		{
			// 预生成针对 gg.pics.walls_? 贴图的 uv 布局. 整张图 256 * 348, 一共 8 列, 每列 32
			// 第一排高度是 32 第二排是 26，如此交替 6 次, (32 + 26) * 6. 26 高度的是面向玩家的斜壁, 独立处理
			// 每张 tile 按 外围描边 开口, 分为 8 格
			// 123
			// 8 4
			// 765
			// 中间一格算本体，四个方向是出路( 如果有邻居 ), 四个角用来配合描边
			// 根据排列组合，如果哪一格有描边，就记为1. 没有记为0. 可以转为 "1 byte 数字"
			// 可以建立 256 长度的数组，用 "1 byte 数字" 作为下标来定位 uv
			// 对于 567 部位带描边的情况，可对应 4 种斜坡，可以放到另一个 256 长度的数组
			// 
			// 当前 walls 图例，内容布局如下
			// 
			// 123|123|123|1  |  3|123|123|123
			// 8  |   |  4|8  |  4|8  |  4|8 4
			// 7  |   |  5|7 5|7 5|7 5|7 5|7 5
			// ---+---+---+---+---+---+---+---
			// 199|  7| 31|209| 92|215| 95|223
			// ---+---+---+---+---+---+---+---
			// 1  |   |  3|1 3|1 3|1 3|1 3|1 3
			// 8  |   |  4|8  |  4|8  |  4|8 4
			// 7  |   |  5|7  |  5|765|765|7 5
			// ---+---+---+---+---+---+---+---
			// 193|  0| 28|197| 29|245|125|221
			// ---+---+---+---+---+---+---+---
			// 1  |   |  3|123|123|1 3|123|1 3
			// 8  |   |  4|   |   |8  |   |8 4
			// 765|765|765|  5|7  |7 5|7 5|765
			// ---+---+---+---+---+---+---+---
			// 241|112|124| 23| 71|213| 87|253
			// ---+---+---+---+---+---+---+---
			//   3|   |1  |  3|1  |1 3|1 3|123
			//    |   |   |   |   |   |  4|8 4
			// 7 5|7 5|7 5|765|765|765|7 5|765
			// ---+---+---+---+---+---+---+---
			// 	84| 80| 81|116|113|117| 93|255
			// ---+---+---+---+---+---+---+---
			//   3|1 3|1  |   |   |123|123|123
			//    |   |   |   |   |8  |   |  4
			//   5|7 5|7  |  5|7  |765|765|765
			// ---+---+---+---+---+---+---+---
			// 	20|	85| 65| 16| 64|247|119|127
			// ---+---+---+---+---+---+---+---
			// 1 3|1 3|1 3|  3|1  |  3|1  |   
			//    |   |   |   |   |   |   |   
			//   5|   |7  |   |   |7  |  5|   
			// ---+---+---+---+---+---+---+---
			// 	21|  5| 69|  4|  1| 68| 17|
			// ---+---+---+---+---+---+---+---
			// 

			// row, col  从表的左上角开始算( 0, 0 )
			assert(wallsTiles.size() == pics.walls_.size());
			auto FillUV = [this](size_t i, int32_t r, int32_t c, uint8_t b, bool includeFront = false) {
				auto& f = pics.walls_[i];
				auto& t = wallsTiles[i][b];
				t.tex = f.tex;
				auto& uv = t.uvRect;
				if (includeFront) {
					uv.w = 32;
					uv.h = 32 + 26;
				}
				else {
					uv.w = uv.h = 32;
				}
				uv.x = f.uvRect.x + 32 * c;
				uv.y = f.uvRect.y + (32 + 26) * r;
				};

			for (size_t i = 0; i < pics.walls_.size(); ++i) {
				FillUV(i, 0, 0, 199);
				FillUV(i, 0, 1, 7);
				FillUV(i, 0, 2, 31);
				FillUV(i, 0, 3, 209);
				FillUV(i, 0, 4, 92);
				FillUV(i, 0, 5, 215);
				FillUV(i, 0, 6, 95);
				FillUV(i, 0, 7, 223);

				FillUV(i, 1, 0, 193);
				FillUV(i, 1, 1, 0);
				FillUV(i, 1, 2, 28);
				FillUV(i, 1, 3, 197);
				FillUV(i, 1, 4, 29);
				FillUV(i, 1, 5, 245, true);
				FillUV(i, 1, 6, 125, true);
				FillUV(i, 1, 7, 221);

				FillUV(i, 2, 0, 241, true);
				FillUV(i, 2, 1, 112, true);
				FillUV(i, 2, 2, 124, true);
				FillUV(i, 2, 3, 23);
				FillUV(i, 2, 4, 71);
				FillUV(i, 2, 5, 213);
				FillUV(i, 2, 6, 87);
				FillUV(i, 2, 7, 253, true);

				FillUV(i, 3, 0, 84);
				FillUV(i, 3, 1, 80);
				FillUV(i, 3, 2, 81);
				FillUV(i, 3, 3, 116, true);
				FillUV(i, 3, 4, 113, true);
				FillUV(i, 3, 5, 117, true);
				FillUV(i, 3, 6, 93);
				FillUV(i, 3, 7, 255, true);

				FillUV(i, 4, 0, 20);
				FillUV(i, 4, 1, 85);
				FillUV(i, 4, 2, 65);
				FillUV(i, 4, 3, 16);
				FillUV(i, 4, 4, 64);
				FillUV(i, 4, 5, 247, true);
				FillUV(i, 4, 6, 119, true);
				FillUV(i, 4, 7, 127, true);

				FillUV(i, 5, 0, 21);
				FillUV(i, 5, 1, 5);
				FillUV(i, 5, 2, 69);
				FillUV(i, 5, 3, 4);
				FillUV(i, 5, 4, 1);
				FillUV(i, 5, 5, 68);
				FillUV(i, 5, 6, 17);
			}

			auto FillShadowMaskUV = [this](int32_t r, int32_t c, uint8_t b) {
				auto& f = pics.walls_shadowmask;
				auto& t = wallsShadowMaskTiles[b];
				t.tex = f.tex;
				auto& uv = t.uvRect;
				uv.w = uv.h = 32;
				uv.x = f.uvRect.x + 32 * c;
				uv.y = f.uvRect.y + (32 + 26) * r;
				};

			FillShadowMaskUV(0, 0, 199);
			FillShadowMaskUV(0, 1, 7);
			FillShadowMaskUV(0, 2, 31);
			FillShadowMaskUV(0, 3, 209);
			FillShadowMaskUV(0, 4, 92);
			FillShadowMaskUV(0, 5, 215);
			FillShadowMaskUV(0, 6, 95);
			FillShadowMaskUV(0, 7, 223);

			FillShadowMaskUV(1, 0, 193);
			FillShadowMaskUV(1, 1, 0);
			FillShadowMaskUV(1, 2, 28);
			FillShadowMaskUV(1, 3, 197);
			FillShadowMaskUV(1, 4, 29);
			FillShadowMaskUV(1, 5, 245);
			FillShadowMaskUV(1, 6, 125);
			FillShadowMaskUV(1, 7, 221);

			FillShadowMaskUV(2, 0, 241);
			FillShadowMaskUV(2, 1, 112);
			FillShadowMaskUV(2, 2, 124);
			FillShadowMaskUV(2, 3, 23);
			FillShadowMaskUV(2, 4, 71);
			FillShadowMaskUV(2, 5, 213);
			FillShadowMaskUV(2, 6, 87);
			FillShadowMaskUV(2, 7, 253);

			FillShadowMaskUV(3, 0, 84);
			FillShadowMaskUV(3, 1, 80);
			FillShadowMaskUV(3, 2, 81);
			FillShadowMaskUV(3, 3, 116);
			FillShadowMaskUV(3, 4, 113);
			FillShadowMaskUV(3, 5, 117);
			FillShadowMaskUV(3, 6, 93);
			FillShadowMaskUV(3, 7, 255);

			FillShadowMaskUV(4, 0, 20);
			FillShadowMaskUV(4, 1, 85);
			FillShadowMaskUV(4, 2, 65);
			FillShadowMaskUV(4, 3, 16);
			FillShadowMaskUV(4, 4, 64);
			FillShadowMaskUV(4, 5, 247);
			FillShadowMaskUV(4, 6, 119);
			FillShadowMaskUV(4, 7, 127);

			FillShadowMaskUV(5, 0, 21);
			FillShadowMaskUV(5, 1, 5);
			FillShadowMaskUV(5, 2, 69);
			FillShadowMaskUV(5, 3, 4);
			FillShadowMaskUV(5, 4, 1);
			FillShadowMaskUV(5, 5, 68);
			FillShadowMaskUV(5, 6, 17);

			// ...
		}


		// init font0/2
		auto charCount = pics._3500_t0_.size();
		assert(charCount == sizeof(charCodes_3500_t0) / sizeof(charCodes_3500_t0[0]));
		assert(pics._3500_t2_.size() == charCount);

		font0 = xx::MakeShared<xx::BMFont>();
		font0->Init(pics._3500_t0_.data(), charCount, charCodes_3500_t0);
		font2 = xx::MakeShared<xx::BMFont>();
		font2->Init(pics._3500_t2_.data(), charCount, charCodes_3500_t0);


		// init global ui
		ui.Emplace()->InitRoot(scale);
		(uiFPS = ui->Make<xx::Label>())->Init(2, p7, a7, 32.f);
		(uiText = ui->Make<xx::Label>())->Init(2, p8, a8, 32.f);
		(uiTips = ui->Make<xx::Label>())->Init(2, p9, a9, 32.f)(U"hotkeys: ESC, [, ]");

		// hide hardware mouse( scene can draw custom mouse cursor )
		SetMousePointerVisible(false);

		// create startup scene
		scene.Emplace<MainMenu::Scene>()->Init();
	}

	// game loop
	void Game::Update() {
		// handle inputs
		if (keyboard[GLFW_KEY_LEFT_BRACKET](0.2f)) {
			isShowInfo = !isShowInfo;
		}
		if (keyboard[GLFW_KEY_RIGHT_BRACKET](0.2f)) {
			isLimitFPS = !isLimitFPS;
		}
		if (keyboard[GLFW_KEY_GRAVE_ACCENT](0.2f)) {
			isShowDebugPanel = !isShowDebugPanel;			// todo
		}

		scene->Update();
		if (!minimized) {
			scene->Draw();

			// draw ui
			uiFPS->SetText(fpsVal);
			DrawNode(ui);

			// draw cursor
			scene->DrawCursor();
		}
		if (oldScene) {
			uiText->Clear();
			oldScene.Reset();
		}
	}

	void Game::Delay() {
		if (isLimitFPS) {
			SleepSecs(cDelta - (glfwGetTime() - time));	// for power saving
		}
	}

	void Game::OnResize(bool modeChanged_) {
		uiFPS->position = p7;
		uiText->position = p8;
		uiTips->position = p9;
		ui->Resize(scale);
		if (scene) {
			scene->OnResize(modeChanged_);
		}
	}

	void Game::OnFocus(bool focused_) {
		if (scene) {
			scene->OnFocus(focused_);
		}
	}

	void Game::Stat() {
		fpsVal = xx::ToString("FPS = ", drawFPS);	// for uiFPS
		if (isShowInfo) {
			xx::CoutN("FPS = ", drawFPS, " DrawCall = ", drawCall, " NumVerts = ", drawVerts);
		}
	}

}
