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
	* ���캯��
	*/
	AABB();

	/**
	* ���캯�� ��������С�������꣬��󶥵�����
	*/
	AABB(const Vec3& min, const Vec3& max);

	/**
	* ���캯�� ������AABB��Χ��
	*/
	AABB(const AABB& box);

	/**
	* ��ȡ��Χ�����ĵ�����
	*/
	Vec3 getCenter();

	/* ��ȡ��Χ�а˸�������Ϣ
	* Z�����������
	* verts[0] : ���϶���
	* verts[1] : ���¶���
	* verts[2] : ���¶���
	* verts[3] : ���϶���
	*
	* Z�Ḻ�������
	* verts[4] : ���϶���
	* verts[5] : ���¶���
	* verts[6] : ���¶���
	* verts[7] : ���϶���
	*/
	void getCorners(Vec3 *dst) const;

	/**
	* �ж�������Χ���Ƿ��غ�
	*/
	bool intersects(const AABB& aabb) const;

	/**
	* �ж�һ�����Ƿ��ڰ�Χ����
	*/
	bool containPoint(const Vec3& point) const;

	/**
	��������Χ������һ����ͬʱ��Χ��������Χ�е���С��Χ��
	*/
	void merge(const AABB& box);

	/**
	* ���ð�Χ�е���󶥵�����С����
	*/
	void set(const Vec3& min, const Vec3& max);

	/**
	* ��λ���� ��ʼ�������С������Ϣ
	*/
	void reset();

	bool isEmpty() const;

	/**
	* ������󶥵�����С������Ϣ
	*/
	void updateMinMax(const Vec3* point, size_t num);

	/**
	* ��һ������Զ԰�Χ�н��ж���任
	*/
	void transform(Mat4& mat);

public:
	Vec3 _min;   //��ά���� ������С������
	Vec3 _max;   //��ά���� ������������
};
#endif // ! _COLLISION_H_

