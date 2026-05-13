#include "pch.h"
#include "t3.h"

namespace Test3 {

	void Scene::MakeUI() {
		static constexpr float cSliderWidths[]{ 400, 500, 200 };
		static constexpr float cMargin{ 5 };
		static constexpr float cLineHeight{ 100 };
		static constexpr XY cItemSize{ cSliderWidths[0] + cSliderWidths[1] + cSliderWidths[2], cLineHeight - cMargin };
		auto fontSize = cItemSize.y - gg.embed.cfg_s9bN->paddings.TopBottom();
		auto anchor = gg.a7;
		auto offset = gg.p7 * 2 + XY{ cMargin, -cMargin };

		auto MakeSlider = [&](auto& tarSlider_, int32_t* value_, xx::FromTo<int32_t> range_, auto txt_) {
			offset.y -= cLineHeight;
			tarSlider_ = ui->Make<xx::Slider>();
			tarSlider_->callbackWhenBlockMoving = true;
			tarSlider_->valueToString = [range_](double v)->std::string {
				return xx::ToString(range_.from + (int32_t)(v * (range_.to - range_.from)));
			};
			tarSlider_->Init(2, offset, anchor, cItemSize.y, cSliderWidths[0], cSliderWidths[1], cSliderWidths[2]
				, (double)(*value_ - range_.from) / (range_.to - range_.from))(txt_);
			tarSlider_->onChanged = [this, value_, range_](double v) {
				*value_ = int32_t(v * (range_.to - range_.from) + range_.from);
			};
			tarSlider_->SetAlphaRecursive(0.5f);
		};

		MakeSlider(uiBulletAttack, &cBulletAttack, cBulletAttackRange, U"子弹威力");
		MakeSlider(uiBulletCount, &cBulletCount, cBulletCountRange, U"子弹数量");
		MakeSlider(uiBulletFlySpeed, &cBulletFlySpeed, cBulletFlySpeedRange, U"子弹飞速");
		MakeSlider(uiBulletShootSpeed, &cBulletShootSpeed, cBulletShootSpeedRange, U"子弹射速");
		MakeSlider(uiBulletPierceCount, &cBulletPierceCount, cBulletPierceCountRange, U"子弹穿透次数");

		offset.y -= cLineHeight;
		ui->Make<xx::LabelButton>()->Init(2, offset, anchor, fontSize)(U"生成1蛇").SetLabelBorder().onClicked = [this] {
			GenSnake();
		};
		offset.y -= cLineHeight;
		ui->Make<xx::LabelButton>()->Init(2, offset, anchor, fontSize)(U"生成100蛇").SetLabelBorder().onClicked = [this] {
			for (int32_t i = 0; i < 100; ++i) {
				GenSnake();
			}
		};
	}

}
