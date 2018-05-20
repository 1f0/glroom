#include "collision.h"
#include <cassert>
#include <algorithm>
#include <GL/glut.h>
//���캯��
AABB::AABB()
{
	reset(); //��ʼ����󶥵�����С����
}

AABB::AABB(const Vec3& min, const Vec3& max)
{
	set(min, max); //������󶥵�����С����
}

AABB::AABB(const AABB& box)
{
	set(box._min, box._max); //������󶥵�����С����
}
//��ȡ��Χ�����ĵ�����
Vec3 AABB::getCenter()
{
	Vec3 center;
	center.x = 0.5f*(_min.x + _max.x);
	center.y = 0.5f*(_min.y + _max.y);
	center.z = 0.5f*(_min.z + _max.z);

	return center;
}
//��ȡ��Χ�а˸�������Ϣ
void AABB::getCorners(Vec3 *dst) const
{
	assert(dst);

	// ����Z�����������
	// ���϶�������
	dst[0].set(_min.x, _max.y, _max.z);
	// ���¶�������
	dst[1].set(_min.x, _min.y, _max.z);
	// ���¶�������
	dst[2].set(_max.x, _min.y, _max.z);
	// ���϶�������
	dst[3].set(_max.x, _max.y, _max.z);

	// ����Z�Ḻ�������
	// ���϶�������
	dst[4].set(_max.x, _max.y, _min.z);
	// ���¶�������
	dst[5].set(_max.x, _min.y, _min.z);
	// ���¶�������
	dst[6].set(_min.x, _min.y, _min.z);
	// ���϶�������
	dst[7].set(_min.x, _max.y, _min.z);
}
//�ж�������Χ���Ƿ���ײ
bool AABB::intersects(const AABB& aabb) const
{
	return ((_min.x >= aabb._min.x && _min.x <= aabb._max.x) || (aabb._min.x >= _min.x && aabb._min.x <= _max.x)) &&
		((_min.y >= aabb._min.y && _min.y <= aabb._max.y) || (aabb._min.y >= _min.y && aabb._min.y <= _max.y)) &&
		((_min.z >= aabb._min.z && _min.z <= aabb._max.z) || (aabb._min.z >= _min.z && aabb._min.z <= _max.z));
}
//�жϵ�Ͱ�Χ���Ƿ���ײ
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
//����һ���µİ�Χ�� ͬʱ����������Χ��
void AABB::merge(const AABB& box)
{
	// �����µ���С������
	_min.x = std::min(_min.x, box._min.x);
	_min.y = std::min(_min.y, box._min.y);
	_min.z = std::min(_min.z, box._min.z);

	// �����µ���������
	_max.x = std::max(_max.x, box._max.x);
	_max.y = std::max(_max.y, box._max.y);
	_max.z = std::max(_max.z, box._max.z);
}
//������󶥵�����С����
void AABB::set(const Vec3& min, const Vec3& max)
{
	this->_min = min;
	this->_max = max;
}
//���㸴λ ��ʼ����Ϣ
void AABB::reset()
{
	_min.set(99999.0f, 99999.0f, 99999.0f);
	_max.set(-99999.0f, -99999.0f, -99999.0f);
}
//���������Ϣ�Ƿ�����
bool AABB::isEmpty() const
{
	return _min.x > _max.x || _min.y > _max.y || _min.z > _max.z;
}
//�ɸ��������������ȷ�������С����������
void AABB::updateMinMax(const Vec3* point, size_t num)
{
	for (size_t i = 0; i < num; i++)
	{
		// ��Сx����
		if (point[i].x < _min.x)
			_min.x = point[i].x;

		// ��Сy����
		if (point[i].y < _min.y)
			_min.y = point[i].y;

		// ��Сz����
		if (point[i].z < _min.z)
			_min.z = point[i].z;

		// ���x����
		if (point[i].x > _max.x)
			_max.x = point[i].x;

		// ���y����
		if (point[i].y > _max.y)
			_max.y = point[i].y;

		// ���z����
		if (point[i].z > _max.z)
			_max.z = point[i].z;
	}
}
//ͨ�������ı任����԰�Χ�н��б任
void AABB::transform(Mat4& mat)
{
	Vec3 corners[8]; //�����Χ�а˸�����
					 //����z�����������
					 //���϶�������
	corners[0].set(_min.x, _max.y, _max.z);
	//���¶�������
	corners[1].set(_min.x, _min.y, _max.z);
	//���¶�������
	corners[2].set(_max.x, _min.y, _max.z);
	//���϶�������
	corners[3].set(_max.x, _max.y, _max.z);

	//����z�Ḻ�������
	//���϶�������
	corners[4].set(_max.x, _max.y, _min.z);
	//���¶�������
	corners[5].set(_max.x, _min.y, _min.z);
	//���¶�������
	corners[6].set(_min.x, _min.y, _min.z);
	//���϶�������
	corners[7].set(_min.x, _max.y, _min.z);

	//����任
	for (int i = 0; i < 8; i++)
		mat.transformPoint(corners[i]);
	//��λ��󶥵���С����
	reset();
	//���¼��������С����Ϣ
	updateMinMax(corners, 8);
}

