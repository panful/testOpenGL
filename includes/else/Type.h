#pragma once
#include <iostream>

//向量
struct Vector2
{
	float x, y;
};

struct Vector3
{
	float x, y, z;
};

struct Vector4
{
	float x, y, z, w;
};

//四元数
struct Quaternion
{
	float x, y, z, w;
};

//变换
struct Transform
{
	Vector3 scale;			// 缩放
	Quaternion rotation;	// 旋转
	Vector3 translation;	// 平移
};

//包围盒
struct BoundingBox
{
	Vector3 min, max;
};

//材质
struct Material
{
	Vector3 albedo;	// 颜色
	float alpha;	// 透明度，0表示全透明，1表示不透明
};

//摄像机
struct Camera
{
	enum Type { PERSPECTIVE, ORTHOGONAL };
	Type type;
	Vector3 position;		// 世界空间内的坐标
	Quaternion rotation;
	float fov;
	int width;
	int height;
	float nearPlane;
	float farPlane;
};

//图元类型
enum class PrimitiveType
{
	POINT_LIST,		// 点列表
	LINE_LIST,		// 线段列表，每两个顶点表示一条线段。顶点个数或索引个数必须是2的整数倍。
	TRIANGLE_LIST,	// 三角形列表，每三个顶点表示一个三角形，顶点个数或索引个数必须是3的整数倍。顶点或索引的排列顺序必须满足右手法则。


	LINE_STRIP,		// 多段线，n个顶点构成n-1段连续的折线
	TRIANGLE_STRIP,	// 三角形带【后期补充说明】
	TRIANGLE_FAN,	// 三角形扇【后期补充说明】

	POINT_LIST_2D,
	LINE_LIST_2D,
	TRIANGLE_LIST_2D,
};

//渲染对象
struct Primitive
{
	int id;						// 图元ID
	PrimitiveType primType;		// 图元类型
	Transform* trsf;			// [opt]变换
	BoundingBox* bndBox;		// [opt]包围盒（在图元所在局部坐标系下的包围盒，且未经trsf变换过）
	Material* mtrl;				// [opt]材质
	uint32_t nbVertices;	    // 顶点个数，且大于0
	float* vertices;			// 顶点列表
	float* normals;				// [opt]法线列表,且大小必须和顶点列表相同
	uint32_t* colorPerVertex;	// [opt]每个顶点的颜色，用于云图渲染，每个顶点对应一个颜色值。如果mtrl不为空，则忽略此项
	uint32_t nbIndices;			// 索引值个数, 如果值为0，则表示当前图元不使用索引形式。
	uint32_t* indices;			// 索引值列表
};

// @brief 在指定图层上添加渲染对象
// @params @param layer [in] 图层id或序号，图层的渲染顺序是从小到大。
//         @param prim  [in] 图元信息。
// @returns 错误码
int AddPrimitive(uint16_t layer, Primitive* prim);