#include "collision.h"
#include <cassert>
#include <algorithm>
#include <GL/glut.h>
//构造函数
AABB::AABB()
{
	reset(); //初始化最大顶点与最小顶点
}

AABB::AABB(const Vec3& min, const Vec3& max)
{
	set(min, max); //设置最大顶点与最小顶点
}

AABB::AABB(const AABB& box)
{
	set(box._min, box._max); //设置最大顶点与最小顶点
}
//获取包围盒中心点坐标
Vec3 AABB::getCenter()
{
	Vec3 center;
	center.x = 0.5f*(_min.x + _max.x);
	center.y = 0.5f*(_min.y + _max.y);
	center.z = 0.5f*(_min.z + _max.z);

	return center;
}
//获取包围盒八个顶点信息
void AABB::getCorners(Vec3 *dst) const
{
	assert(dst);

	// 朝着Z轴正方向的面
	// 左上顶点坐标
	dst[0].set(_min.x, _max.y, _max.z);
	// 左下顶点坐标
	dst[1].set(_min.x, _min.y, _max.z);
	// 右下顶点坐标
	dst[2].set(_max.x, _min.y, _max.z);
	// 右上顶点坐标
	dst[3].set(_max.x, _max.y, _max.z);

	// 朝着Z轴负方向的面
	// 右上顶点坐标
	dst[4].set(_max.x, _max.y, _min.z);
	// 右下顶点坐标
	dst[5].set(_max.x, _min.y, _min.z);
	// 左下顶点坐标
	dst[6].set(_min.x, _min.y, _min.z);
	// 左上顶点坐标
	dst[7].set(_min.x, _max.y, _min.z);
}
//判断两个包围盒是否碰撞
bool AABB::intersects(const AABB& aabb) const
{
	return ((_min.x >= aabb._min.x && _min.x <= aabb._max.x) || (aabb._min.x >= _min.x && aabb._min.x <= _max.x)) &&
		((_min.y >= aabb._min.y && _min.y <= aabb._max.y) || (aabb._min.y >= _min.y && aabb._min.y <= _max.y)) &&
		((_min.z >= aabb._min.z && _min.z <= aabb._max.z) || (aabb._min.z >= _min.z && aabb._min.z <= _max.z));
}
//判断点和包围盒是否碰撞
bool AABB::containPoint(const Vec3& point) const
{
	if (point.x < _min.x) return false;
	if (point.y < _min.y) return false;
	if (point.z < _min.z) return false;
	if (point.x > _max.x) return false;
	if (point.y > _max.y) return false;
	if (point.z > _max.z) return false;
	return true;
}
//生成一个新的包围盒 同时容纳两个包围盒
void AABB::merge(const AABB& box)
{
	// 计算新的最小点坐标
	_min.x = std::min(_min.x, box._min.x);
	_min.y = std::min(_min.y, box._min.y);
	_min.z = std::min(_min.z, box._min.z);

	// 计算新的最大点坐标
	_max.x = std::max(_max.x, box._max.x);
	_max.y = std::max(_max.y, box._max.y);
	_max.z = std::max(_max.z, box._max.z);
}
//设置最大顶点与最小顶点
void AABB::set(const Vec3& min, const Vec3& max)
{
	this->_min = min;
	this->_max = max;
}
//顶点复位 初始化信息
void AABB::reset()
{
	_min.set(99999.0f, 99999.0f, 99999.0f);
	_max.set(-99999.0f, -99999.0f, -99999.0f);
}
//检测坐标信息是否有误
bool AABB::isEmpty() const
{
	return _min.x > _max.x || _min.y > _max.y || _min.z > _max.z;
}
//由给定点坐标点重新确定最大最小的坐标向量
void AABB::updateMinMax(const Vec3* point, size_t num)
{
	for (size_t i = 0; i < num; i++)
	{
		// 最小x坐标
		if (point[i].x < _min.x)
			_min.x = point[i].x;

		// 最小y坐标
		if (point[i].y < _min.y)
			_min.y = point[i].y;

		// 最小z坐标
		if (point[i].z < _min.z)
			_min.z = point[i].z;

		// 最大x坐标
		if (point[i].x > _max.x)
			_max.x = point[i].x;

		// 最大y坐标
		if (point[i].y > _max.y)
			_max.y = point[i].y;

		// 最大z坐标
		if (point[i].z > _max.z)
			_max.z = point[i].z;
	}
}
//通过给定的变换矩阵对包围盒进行变换
void AABB::transform(Mat4& mat)
{
	Vec3 corners[8]; //保存包围盒八个顶点
					 //朝向z轴正方向的面
					 //左上顶点坐标
	corners[0].set(_min.x, _max.y, _max.z);
	//左下顶点坐标
	corners[1].set(_min.x, _min.y, _max.z);
	//右下顶点坐标
	corners[2].set(_max.x, _min.y, _max.z);
	//右上顶点坐标
	corners[3].set(_max.x, _max.y, _max.z);

	//朝向z轴负方向的面
	//右上顶点坐标
	corners[4].set(_max.x, _max.y, _min.z);
	//右下顶点坐标
	corners[5].set(_max.x, _min.y, _min.z);
	//左下顶点坐标
	corners[6].set(_min.x, _min.y, _min.z);
	//左上顶点坐标
	corners[7].set(_min.x, _max.y, _min.z);

	//顶点变换
	for (int i = 0; i < 8; i++)
		mat.transformPoint(corners[i]);
	//复位最大顶点最小顶点
	reset();
	//重新计算最大最小点信息
	updateMinMax(corners, 8);
}

