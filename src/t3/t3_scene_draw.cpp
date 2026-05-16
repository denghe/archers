#include "pch.h"
#include "t3.h"

namespace Test3 {

	void Scene::Draw() {
		// 设置内容绘制时不插值, 让图像清晰锐利
		gg.picsTex->SetParm(GL_NEAREST);

		// 准备地板污染痕迹贴图
		if (floorMasks.len) {
			// 将数据里的东西画到 render texture 上并清空
			frameBuffer.DrawTo(floorMaskTex, {}, [this] {
				// 这里的绘制坐标不受 cam 影响, 直接映射到逻辑地图. 以贴图左上角为 0,0 开始绘制
				auto leftTopPos = mapPixelSize * XY{ -0.5f, 0.5f };
				for (auto& o : floorMasks) {
					auto p = leftTopPos + o.pos.FlipY();
					gg.Quad().DrawFrame(o.frame, p, o.scale, o.radians, o.colorplus, o.color);
				}
			});
			floorMasks.Clear();
		}

		// 准备内容贴图( 需要被 light 照亮的部分 )
		auto tex = frameBuffer.Draw(gg.windowSize, true, xx::RGBA8{ 0,0,0,0 }, [&]() {
			// 绘制地板纹理
			for (int32_t i = 0; i < mapSize.y; ++i) {
				for (int32_t j = 0; j < mapSize.x; ++j) {
					XY p{ j * cCellPixelSize, i * cCellPixelSize };
					gg.Quad().DrawTinyFrame(gg.pics.c128_floor, cam.ToGLPos(p), { 0,1 }, cam.scale, 0);
				}
			}

			// 背景部分绘制
			for (auto& o : walls) o->Draw();

			// 地板污染痕迹绘制
			gg.Quad().Draw(*floorMaskTex, *floorMaskTex, cam.ToGLPos(mapPixelSize * 0.5f), 0.5f, cam.scale, 0, 1.f, {222,222,222,222});

			// 影子
			for (auto& o : snakes) o->DrawShadow();
			for (auto& oo : creaturess) {
				for (auto& o : oo) {
					o->DrawShadow();
					o->weapon->DrawShadow();
				}
			}
			for (auto& o : playerBullets) o->DrawShadow();
			if (player) player->DrawShadow();

			// sort order by y
			for (auto& oo : creaturess) {
				for (auto& o : oo) {
					SortContainerAdd(o.pointer);
					//SortContainerAdd(o->weapon.pointer);
				}
			}
			for (auto& oo : snakes) {
				for (auto& o : oo->elements) {
					SortContainerAdd(o.pointer);
				}
			}
			for (auto& o : exploders) SortContainerAdd(o.pointer);
			for (auto& o : playerBullets) SortContainerAdd(o.pointer);
			if (player) SortContainerAdd(player.pointer);
			SortContainerDraw();
		});

		// 设置内容绘制时插值, 让光影过渡柔和
		gg.picsTex->SetParm(GL_LINEAR);

		// 准备光照贴图
		auto lightTexScale{ 270.f / gg.windowSize.y };	// 用更小的绘制比例以节省填充率( 太小会画质恶劣 )
		cam.SetBaseScale(gg.scale * lightTexScale);
		auto bgColor = xx::RGBA8{ 30,30,30,255 };
		auto lightTex = frameBuffer.Draw(gg.windowSize * lightTexScale, true, bgColor, [&] {
			gg.GLBlendFunc({ GL_SRC_COLOR, GL_ONE, GL_FUNC_ADD });
			for (auto& oo : creaturess) {
				for (auto& o : oo) {
					o->DrawLight();
					//o->weapon->DrawLight();
				}
			}
			for (auto& oo : snakes) {
				for (auto& o : oo->elements) {
					o->DrawLight();
				}
			}
			for (auto& o : playerBullets) o->DrawLight();
			for (auto& o : exploders) o->DrawLight();
			if (player) player->DrawLight();
			// ...
		});
		lightTex->SetParm(GL_LINEAR);
		cam.SetBaseScale(gg.scale);

		// 合并绘制: 内容 + 光照
		gg.QuadLight().Draw(tex, lightTex, xx::RGBA8_White, 1.2f);	// 1.2: 稍微弄点曝光过度
		// 立即提交以防止 tex, lightTex 出函数后失效
		gg.ShaderEnd();

		// 血条
		for (auto& oo : creaturess) {
			for (auto& o : oo) {
				o->DrawHPBar();
			}
		}
		for (auto& oo : snakes) {
			for (auto& o : oo->elements) {
				o->DrawHPBar();
			}
		}

		// 伤害文字
		effectTexts.Draw();

		// 设置内容绘制时不插值, 让图像清晰锐利
		gg.picsTex->SetParm(GL_NEAREST);

		// 设置顶部信息文字显示内容
		int32_t count{};
		for (auto& oo : snakes) count += oo->elements.len;
		gg.uiText->SetText(xx::ToString("snake elements count = ", count));
		gg.DrawNode(ui);

		// gizmos
		if (gg.isShowDebugPanel) {
			for (auto& oo : creaturess) {
				for (auto& o : oo) {
					//o->DrawGizmos();
					o->weapon->DrawGizmos();
				}
			}

			// pathway
			auto& q = gg.Quad();
			for (auto& pathway : pathways) {
				for (int32_t s = (int32_t)pathway->points.size(), i = 0; i < s; /*++i*/i += 200) {
					q.DrawFrame(
						gg.pics.c64
						, cam.ToGLPos(pathway->points[i].pos)
						, cam.scale
					);
				}
			}
			// ...
		}
	}

	void Scene::OnResize(bool modeChanged_) {
		ui->Resize(gg.scale * cUIScale);
		cam.SetBaseScale(gg.scale);
	}

}
