#include "pch.h"
#include "mm_scene.h"
#include "t1.h"
#include "t2.h"
#include "t3.h"
#include "t4.h"
#include "t5.h"

namespace MainMenu {

	void Scene::MakeUI() {
		SceneBase::MakeUI();
		static constexpr float cLineHeight{ 100 };
		static constexpr float cItemHeight{ 80 };
		static constexpr float cMargin{ 20 };
		auto fontSize = cItemHeight - gg.embed.cfg_s9bN->paddings.TopBottom();

		auto C = ui->Make<xx::Node>();
		xx::Layouter L;
		auto ds = gg.designSize;
		auto ds_2 = ds / 2;
		L.InitBegin(C, 2, gg.p5 + XY{ 0, ds_2.y }, { 0.5f, 1.f }, ds.x)
			.HAlign(xx::HAligns::Center)
			.LeftMargin(cMargin)
			.DefaultLineHeight(cLineHeight);

		L.Append(C->Make<xx::Label>()->Init(2, 0, 0, fontSize * 3).SetFont(gg.font2)(U"两队小兵对冲模拟器")).SetBorder();
		L.EndLine();

		L.Append(C->Make<xx::LabelButton>()->Init(2, 0, 0, fontSize)(U"弓箭手隔岸射怪物(鼠标左右键点岩浆左边或最右边出怪区)")).SetLabelBorder().onClicked = [this] {
			SetDelayCmd([this] {
				gg.MakeScene<Test1::Scene>()->Init();
			});
		};

		L.Append(C->Make<xx::LabelButton>()->Init(2, 0, 0, fontSize)(U"近战挥刀怪(鼠标左右键分阵营 键盘 1 2 巨量投放)")).SetLabelBorder().onClicked = [this] {
			SetDelayCmd([this] {
				gg.MakeScene<Test2::Scene>()->Init();
			});
		};

		L.Append(C->Make<xx::LabelButton>()->Init(2, 0, 0, fontSize)(U"蛇模拟")).SetLabelBorder().onClicked = [this] {
			SetDelayCmd([this] {
				gg.MakeScene<Test3::Scene>()->Init();
			});
		};

		L.Append(C->Make<xx::LabelButton>()->Init(2, 0, 0, fontSize)(U"测试地牢场景(鼠标左右键增删墙,中键放矿)")).SetLabelBorder().onClicked = [this] {
			SetDelayCmd([this] {
				gg.MakeScene<Test4::Scene>()->Init();
			});
		};

		L.Append(C->Make<xx::LabelButton>()->Init(2, 0, 0, fontSize)(U"测试青蛙砍罐子")).SetLabelBorder().onClicked = [this] {
			SetDelayCmd([this] {
				gg.MakeScene<Test5::Scene>()->Init();
			});
		};

		L.InitEnd();
	}

	void Scene::Update() {
		// 按 ESC 可以关闭 顶层 popup UI 或直接退出程序
		if (gg.keyboard[GLFW_KEY_ESCAPE](0.2f)) {
			if (auto node = ui->FindTopESCHandler(); node) {
				node->HandleESC();
			}
			else {
				gg.running = false;
			}
			return;
		}

		SceneBase::Update();

		// 延迟执行指令( 给一些按键释放留点时间 避免误触 )
		if (delayCmd) {
			if (time >= delayCmdExecTime) {
				delayCmd();
			}
		}
	}


	void Scene::Draw() {
		gg.clearColor = { 111, 111, 111, 255 };
		SceneBase::Draw();
	}

}
