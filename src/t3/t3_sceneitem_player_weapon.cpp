#include "pch.h"
#include "t3.h"

namespace Test3 {

	void PlayerWeapon::Init(Player* owner_) {
		typeId = cTypeId;
		scene = owner_->scene;
		owner = xx::WeakFromThis(owner_);

		pos = owner_->pos;
		y = pos.y + 1.f;
		radius = cCreatureRadius * 0.5f;
		scale = radius * 2.f / gg.pics.c64_bullet.uvRect.h;

		// 方向暂时向上
		radians = -gPI_2;
	}

	void PlayerWeapon::Update() {
		// 同步 owner 坐标
		pos = owner->pos;
		y = pos.y + 1.f;

		// todo: 射击时挥动武器?

		if (nextShootTime < scene->time) {
			nextShootTime = scene->time + shootDelay;

			// 计算每颗子弹的发射位置
			// todo:
			// 如果默认间距小于实际间距 就按实际来 转为下一粒子弹的 x offset 步进

			float bulletMargin{ 10.f };
			float bulletRadius{ radius };
			auto totalWidth = bulletRadius * 2.f * shootCountPerRound + bulletMargin * (shootCountPerRound - 1);
			auto step = bulletMargin + bulletRadius * 2.f;

			XY offset{ -totalWidth * 0.5f + bulletRadius, -radius };
			for (int32_t i = 0; i < shootCountPerRound; ++i) {
				auto p = pos + offset;
				if (p.x >= 0 && p.x < scene->mapPixelSize.x) {
					scene->playerBullets.Emplace().Emplace<PlayerBullet>()->Init(owner, this, p, radius);
				}
				offset.x += step;
			}
		}
	}

	void PlayerWeapon::Draw() {
		auto& f = gg.pics.sword1;
		gg.Quad().DrawFrame(f, scene->cam.ToGLPos(pos)
			, cWidth / 7.f * scene->cam.scale, radians);
	}

	void PlayerWeapon::DrawLight() {
		auto& f = gg.pics.light_sword1;
		gg.Quad().DrawFrame(f, scene->cam.ToGLPos(pos)
			, cWidth / 7.f * scene->cam.scale, radians);
	}

	void PlayerWeapon::Dispose() {
		assert(scene);
		assert(!disposing);

		// 设置标记
		disposing = true;

		// 自杀
		owner->weapon.Reset();
	}

}
