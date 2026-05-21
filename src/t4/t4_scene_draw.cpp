#include "pch.h"
#include "t4.h"

namespace Test4 {

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
			// 背景部分绘制
			// 绘制地板纹理	// todo: 理论上讲可以合并成使用 1 个 shader + 4 个顶点画所有
			for (int32_t i = 0; i < mapSize.y; ++i) {
				for (int32_t j = 0; j < mapSize.x; ++j) {
					XY p{ j * cCellPixelSize, i * cCellPixelSize };
					gg.Quad().DrawTinyFrame(gg.pics.floor_[cFloorTexIndex], cam.ToGLPos(p), { 0,1 }, cCellPixelSize / 32.f * cam.scale, 0);
				}
			}


			// 地板污染痕迹绘制
			gg.Quad().Draw(*floorMaskTex, *floorMaskTex, cam.ToGLPos(mapPixelSize * 0.5f), 0.5f, cam.scale, 0, 1.f, {222,222,222,222});

			// 影子

			// 需要按 y 排序的内容
			for (auto& o : walls) SortContainerAdd(o.pointer);
			if (player) SortContainerAdd(player.pointer);
			SortContainerDraw();
		});

		// 设置内容绘制时插值, 让光影过渡柔和
		gg.picsTex->SetParm(GL_LINEAR);

		// 准备光照贴图
		auto lightTexScale{ 270.f / gg.windowSize.y };	// 用更小的绘制比例以节省填充率( 太小会画质恶劣 )
		cam.SetBaseScale(gg.scale * lightTexScale);
#if 1
		auto bgColor = xx::RGBA8{ 0,0,0,255 };
#else
		auto bgColor = xx::RGBA8{ 255,255,255,255 };
#endif
		auto lightTex = frameBuffer.Draw(gg.windowSize * lightTexScale, true, bgColor, [&] {
			gg.GLBlendFunc({ GL_SRC_COLOR, GL_ONE, GL_FUNC_ADD });

			// 在鼠标处打个光看效果
#if 0
			gg.Quad().DrawFrame(gg.pics.c512_light, gg.mousePos * lightTexScale, 1.f);
#else
			gg.Quad().DrawFrame(gg.pics.c256_light, gg.mousePos * lightTexScale, 2.f);
#endif

			// ...
		});
		lightTex->SetParm(GL_LINEAR);
		cam.SetBaseScale(gg.scale);

		// 合并绘制: 内容 + 光照
		gg.QuadLight().Draw(tex, lightTex, xx::RGBA8_White, 1.f);
		// 立即提交以防止 tex, lightTex 出函数后失效
		gg.ShaderEnd();

		// 遮黑层
		for (auto& o : walls) o->DrawLightMask();

		// 血条
		// 伤害文字

		// 设置内容绘制时不插值, 让图像清晰锐利
		gg.picsTex->SetParm(GL_NEAREST);

		// 设置顶部信息文字显示内容
		//gg.uiText->SetText(xx::ToString("snake elements count = ", count));
		gg.DrawNode(ui);

		// gizmos
		if (gg.isShowDebugPanel) {
			// ...
		}
	}

	void Scene::OnResize(bool modeChanged_) {
		ui->Resize(gg.scale * cUIScale);
		cam.SetBaseScale(gg.scale);
	}

}
