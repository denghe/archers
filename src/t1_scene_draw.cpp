#include "pch.h"
#include "t1_.h"

namespace Test1 {

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
			for (auto& o : lavas) o->Draw();
			for (auto& o : walls) o->Draw();

			// 地板污染痕迹绘制
			gg.Quad().Draw(*floorMaskTex, *floorMaskTex, cam.ToGLPos(mapPixelSize * 0.5f), 0.5f, cam.scale, 0, 1.f, {222,222,222,222});

			// sort order by y
			for (auto& o : monsters) SortContainerAdd(o.pointer);
			for (auto& o : archers) SortContainerAdd(o.pointer);
			for (auto& o : archerArrows) SortContainerAdd(o.pointer);
			SortContainerDraw();

			// 爆炸特效覆盖在最上层
			for (auto& o : exploders) o->Draw();

			// todo: 影子
		});

		// 设置内容绘制时插值, 让光影过渡柔和
		gg.picsTex->SetParm(GL_LINEAR);

		// 准备光照贴图
		auto lightTexScale{ 270.f / gg.windowSize.y };	// 用更小的绘制比例以节省填充率( 太小会画质恶劣 )
		cam.SetBaseScale(gg.scale * lightTexScale);
		auto bgColor = xx::RGBA8{ 10,10,10,255 };
		auto lightTex = frameBuffer.Draw(gg.windowSize * lightTexScale, true, bgColor, [&] {
			gg.GLBlendFunc({ GL_SRC_COLOR, GL_ONE, GL_FUNC_ADD });
			for (auto& o : monsters) o->DrawLight();
			for (auto& o : archers) o->DrawLight();
			for (auto& o : archerArrows) o->DrawLight();
			for (auto& o : exploders) o->DrawLight();
			// ...
		});
		lightTex->SetParm(GL_LINEAR);
		cam.SetBaseScale(gg.scale);

		// 合并绘制: 内容 + 光照
		gg.QuadLight().Draw(tex, lightTex, xx::RGBA8_White, 2);
		// 立即提交以防止 tex, lightTex 出函数后失效
		gg.ShaderEnd();

		// 血条
		for (auto& o : monsters) o->DrawHPBar();

		// 伤害文字
		effectTexts.Draw();

		// 设置内容绘制时不插值, 让图像清晰锐利
		gg.picsTex->SetParm(GL_NEAREST);

		// 设置顶部信息文字显示内容
		gg.uiText->SetText(xx::ToString("monsters.len = ", monsters.len, "  archerArrows.len = ", archerArrows.len));
		gg.DrawNode(ui);
	}

	void Scene::OnResize(bool modeChanged_) {
		ui->Resize(gg.scale * cUIScale);
		cam.SetBaseScale(gg.scale);
	}

}
