#pragma once
#include "t3_scene.h"

namespace Test3 {

	struct Snake;
	struct SnakeElement : SceneItem {
		static constexpr float cSpeed{ 1.1f };
		static constexpr float cStickRatio{ 10.f };
		static constexpr float cRadiusAnimStep{ 0.01f };
		static constexpr float cDrawScale{ 1.f };
		static constexpr float cNodeDistanceRatio{ 0.6f };
		static constexpr float cFaceAnimSpeed{ 10.f };
		static constexpr xx::FromTo<float> cRadiusRange{ 16.f, 17.f };

		Snake* owner{};
		SnakeElementTypes elementType{};
		int32_t pathwayCursor{ -1 };

		void PreInit(Snake* owner_, SnakeElementTypes elementType_);
		void Init(float radius_);
		void Remove();				// remove from owner and sync all index
		bool HitCheck(XY p_, float hitRadius_);
		SnakeElement* GetPrev();	// -> tail
		SnakeElement* GetNext();	// -> head
		void BaseUpdate();
		virtual int32_t ElementUpdate() { return 0; }	// return 1 to remove self
	};

	struct Snake : SceneItem {
		Scene* scene{};
		Pathway* pathway{};
		xx::List<xx::Shared<SnakeElement>> elements;	// [0].elementType == Tail
		void Init(Scene* scene_, Pathway* pathway_, int32_t bodyLen_);
		void Update() override;
		void Draw() override;
		void DrawLight() override;
	};

	struct SnakeHead : SnakeElement {
		xx::Shaker faceShaker;
		void Init();
		int32_t ElementUpdate() override;
		void Draw() override;
		void DrawLight() override;
	};

	struct SnakeTail : SnakeElement {
		void Init();
		int32_t ElementUpdate() override;
		void Draw() override;
		void DrawLight() override;
	};

	struct SnakeBody : SnakeElement {
		int32_t U1_n{};
		void U1_RadiusAnim();

		void Init();
		int32_t ElementUpdate() override;
		void Draw() override;
		void DrawLight() override;
	};



}
