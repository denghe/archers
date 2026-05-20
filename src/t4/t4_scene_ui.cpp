#include "pch.h"
#include "t4.h"

namespace Test4 {

	void Scene::MakeUI() {
		static constexpr float cSliderWidths[]{ 400, 500, 200 };
		static constexpr float cMargin{ 5 }, cLineSpace{ 0 };
		static constexpr float cLineHeight{ 100 };
		static constexpr XY cItemSize{ cSliderWidths[0] + cSliderWidths[1] + cSliderWidths[2], cLineHeight - cLineSpace };
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

		MakeSlider(uiWallTexIndex, &cWallTexIndex, cWallTexIndexRange, U"墙壁图切换");
		MakeSlider(uiFloorTexIndex, &cFloorTexIndex, cFloorTexIndexRange, U"地板图切换");
	}

}
