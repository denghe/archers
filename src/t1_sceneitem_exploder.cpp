#include "pch.h"
#include "t1_.h"

namespace Test1 {

	void Exploder::Init(Monster* tar_) {
		typeId = cTypeId;
		scene = tar_->scene;
		pos = tar_->pos;	// todo: 考虑 anchor point 在脚下 而爆炸的 anchor point 在中心 故需要位移
		y = pos.y;
		radius = tar_->radius;
		scale = (radius * 2.f / gg.pics.explosion_1_[0].uvRect.w) * cExplodeRadiusRatio;
		radians = gg.rnd.Next<float>(g2PI) - gPI;
		tarFrameIndex = tar_->frameIndex;
		// 播放爆炸帧动画. 计算需要多少帧，每帧的帧编号增量
		inc = cExplodeNumFrames / (cExplodeDuration * gg.cFps);
		// 在地板上留下痕迹
		scene->floorMasks.Emplace(FloorMask{
			.frame = gg.pics.creature_1_[tarFrameIndex],
			.pos = pos,
			.scale = radius / gg.pics.creature_1_[tarFrameIndex].uvRect.w * 2,
			.radians = 0,
			.colorplus = 1.f,
			.color = {0,0,0,222}
		});

		indexAtContainer = scene->exploders.len - 1;
		assert(scene->exploders[indexAtContainer].pointer == this);
	}

	void Exploder::Update() {
		XX_BEGIN(_1);
		// 开始播放帧动画
		for (; frameIndex < cExplodeNumFrames; frameIndex += inc) {
			XX_YIELD(_1);
		}
		// 自杀
		Dispose();
		XX_END(_1);
	}

	void Exploder::Draw() {
		gg.Quad().DrawFrame(gg.pics.explosion_1_[frameIndex], scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians);
	}

	void Exploder::DrawLight() {
		gg.Quad().DrawFrame(gg.pics.c64_light, scene->cam.ToGLPos(pos)
			, (radius * cExplodeRadiusRatio * 2.f * 5.f / 64.f) * scene->cam.scale, 0, 1.f);
	}

	void Exploder::Dispose() {
		assert(scene);
		assert(!disposing);
		assert(indexAtContainer != -1);
		auto& container = scene->exploders;
		assert(container[indexAtContainer].pointer == this);

		// 设置标记
		disposing = true;

		// 从容器中移除对象( 释放内存 )
		auto i = indexAtContainer;
		container.Back()->indexAtContainer = i;
		indexAtContainer = -1;
		container.SwapRemoveAt(i);
	}

}
