#include "pch.h"
#include "g_cursor.h"

namespace Global {

    // 怪状态机示例代码

    enum class 怪_Cmds {
        休息, 移动到, 攻击
    };
    template<typename T> struct 怪_移动 {
        int32_t _n{};
        XX_INLINE void operator()(T& self) {
            XX_BEGIN(_n);
            while (true) {
                self.x += 1;
                XX_YIELD(_n);
            }
            XX_END(_n);
        }
    };
    template<typename T> struct 怪_Idle {
        int32_t _n{};
        XX_INLINE void operator()(T& self) {
            XX_BEGIN(_n);
            while (true) {
                self.frameIndex++;
                if (self.frameIndex > 10) {
                    self.frameIndex = 0;
                }
                XX_YIELD(_n);
            }
            XX_END(_n);
        }
    };
    template<typename T> struct 怪_攻击 {
        XX_INLINE void operator()(T& self) {
            // 传统逻辑
        }
    };
    struct 怪 {
        float x{}, y{};
        int frameIndex{};
        怪_Cmds cmd{ 怪_Cmds::休息 };

        怪_移动<怪> Move;
        怪_Idle<怪> Idle;
        怪_攻击<怪> Attack;

        int32_t _n{};
        void Update() {                  // 主控
            XX_BEGIN(_n);
            while (true) {
                switch (cmd) {
                case 怪_Cmds::休息:
                    Idle(*this);
                    break;
                case 怪_Cmds::移动到:
                    Idle(*this);
                    Move(*this);
                    break;
                case 怪_Cmds::攻击:
                    Attack(*this);
                    // ...
                    break;
                }
                XX_YIELD(_n);
            }
            XX_END(_n);
        }
    };



	void CursorBase::Init() {
#if 0
        怪 m;
        m.cmd = 怪_Cmds::移动到;
        auto secs = xx::NowEpochSeconds();
        for (int32_t i = 0; i < 200000000; ++i) {
            m.Update();
        }
        xx::CoutN("secs = ", xx::NowEpochSeconds(secs));
        xx::CoutN("m.x = ", m.x, " m.frameIndex = ", m.frameIndex);
#endif

		frame = gg.pics.cursor_default;
		radius = 24.f;
	}

	void CursorBase::Update() {
	}

	void CursorBase::Draw() {
		auto frameRadius = frame.uvRect.w * 0.5f;
		auto scale = radius / frameRadius * gg.scale;
		auto mp = gg.mousePos;

		auto&& shader = gg.Quad();
		//shader.Commit();
		//frame.tex->SetParm(GL_NEAREST);
		shader.DrawFrame(frame, mp, scale);
	}

}
