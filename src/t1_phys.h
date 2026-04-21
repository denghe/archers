#pragma once
#include "t1.h"
#include <xx_grid2d_circle.h>

namespace Test1 {

	// 物理系统，负责处理物体的运动和碰撞
	// 依赖 Scene 的 gridBuildings pixelSize
	// 依赖 SceneItem 的 pos radius indexAtGrid typeId

	struct Scene;
	struct SceneItem;

	// 对象池 / 缓存
	struct PhysCache {
		// 当前位置，上一帧位置，加速度
		XY pos, lastPos, accel;
		// 碰撞半径
		float radius{};
		void operator=(SceneItem* p);
	};

	struct PhysSystem : xx::Grid2dCircle<SceneItem*, PhysCache> {
		using Base = xx::Grid2dCircle<SceneItem*, PhysCache>;

		// 下面这些计算相关参数随时可以修改
		// 速度阻尼
		float cVelocityDamping{ 300.f };
		// 重力加速度( 每 Step 都会加在所有 item 的加速度上 )
		XY cGravity{ 0, 0 };
		// 碰撞响应系数( 数值越大，碰撞后弹得越远 )
		float cResponseCoef{ 0.5f };
		// 物体最大速度( 超过这个速度会被强行限制 )
		float cMaxSpeed{ 500.f / gg.cFps };
		// 最大邻居数( 检测次数限制 )
		int32_t maxNumNeighbors{ 3 };

		// 指向场景，方便访问场景数据( 比如地图尺寸，物体列表 )
		Scene* scene{};

		// 初始化，capacity 是 nodes 的预分配数量
		// maxNumNeighbors_ 视情况而定，如果对象都是相同大小， 3~4 就够了
		// 体积相差 2 倍啥的则可能需要 15( 想象一下大圆圈周围需要多少个小圆圈能围满 )
		void Init(Scene* scene_, int32_t numRows_, int32_t numCols_, int32_t cellPixelSize_, int32_t capacity_, int32_t maxNumNeighbors_ = 3);
		// 添加对象( 复制数据到 nodes，并填充 indexAtGrid )
		void Add(SceneItem* item_);
		// 移除对象( 交换删除并同步 indexAtGrid )
		void Remove(SceneItem* item_);
		// 尝试移除对象（如果对象不在系统中，则不执行任何操作）
		void TryRemove(SceneItem* item_);
		// 定位到节点.cache（方便直接改数据）
		PhysCache& At(SceneItem* item_) const;
		// 帧逻辑. 调用 FillBuckets、Calc、Writeback 完成物理计算( 遍历 nodes )
		void Step();
		// 下面是 Step 的具体步骤
		void Calc();
		void CalcBB(int32_t b1_, int32_t b2_);
		void CalcNN(PhysCache& d1_, PhysCache& d2_);
		void Writeback();

		// todo: 增加一些 PhysCache 的辅助修改函数，替代 At() 返回的 Node& 直接修改 cache 的方式

		// todo: 当 nodes 里面夹杂的未使用节点过多时，可能会导致性能问题。
		// 可以考虑定期清理未使用节点( 可以将它们移动到数组末尾，并更新 freeHead 和 freeCount )
		// 触发条件可以是 freeCount 超过某个阈值，或者每隔一定时间清理一次。
	};

}
