#ifndef  _COLLISION_H_
#define _COLLISION_H_
#include <math.h>
#include <iostream>
class Vec3 {
public:
	float x, y, z;
	void set(float xx,float yy,float zz) {
		x = xx;
		y = yy;
		z = zz;
	}
	void output() { std::cout << x << ": " << y << ": " << z << std::endl; }
	Vec3 operator + (const Vec3& rhs) {
		Vec3 res;
		res.set(x + rhs.x, y + rhs.y, z + rhs.z);
		return res;
	}
};
class Mat4{
	float ele[4][4];
public:
	float* m;
	void transformPoint(Vec3 &point) {
		point.x = ele[0][0] * point.x + ele[0][1] * point.y + ele[0][2] * point.z + ele[0][3];
		point.y = ele[1][0] * point.x + ele[1][1] * point.y + ele[1][2] * point.z + ele[1][3];
		point.z = ele[2][0] * point.x + ele[2][1] * point.y + ele[2][2] * point.z + ele[2][3];
	}
	Mat4():m(&ele[0][0]) {}
	Mat4& IDENTITY() {
		
	}
};
class AABB
{
public:
	/**
	* 构造函数
	*/
	AABB();

	/**
	* 构造函数 参数：最小顶点坐标，最大顶点坐标
	*/
	AABB(const Vec3& min, const Vec3& max);

	/**
	* 构造函数 参数：AABB包围盒
	*/
	AABB(const AABB& box);

	/**
	* 获取包围盒中心点坐标
	*/
	Vec3 getCenter();

	/* 获取包围盒八个顶点信息
	* Z轴正方向的面
	* verts[0] : 左上顶点
	* verts[1] : 左下顶点
	* verts[2] : 右下顶点
	* verts[3] : 右上顶点
	*
	* Z轴负方向的面
	* verts[4] : 右上顶点
	* verts[5] : 右下顶点
	* verts[6] : 左下顶点
	* verts[7] : 左上顶点
	*/
	void getCorners(Vec3 *dst) const;

	/**
	* 判断两个包围盒是否重合
	*/
	bool intersects(const AABB& aabb) const;

	/**
	* 判断一个点是否在包围盒内
	*/
	bool containPoint(const Vec3& point) const;

	/**
	由两个包围盒生成一个能同时包围这两个包围盒的最小包围盒
	*/
	void merge(const AABB& box);

	/**
	* 设置包围盒的最大顶点与最小顶点
	*/
	void set(const Vec3& min, const Vec3& max);

	/**
	* 复位函数 初始化最大最小顶点信息
	*/
	void reset();

	bool isEmpty() const;

	/**
	* 更新最大顶点与最小顶点信息
	*/
	void updateMinMax(const Vec3* point, size_t num);

	/**
	* 由一个矩阵对对包围盒进行顶点变换
	*/
	void transform(Mat4& mat);

public:
	Vec3 _min;   //三维向量 保存最小点坐标
	Vec3 _max;   //三维向量 保存最大点坐标
};
#endif // ! _COLLISION_H_

