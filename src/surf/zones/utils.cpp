#include "surf_zones.h"
#include <fmt/format.h>

void SURF::ZONE::CreatePoints2D(const Vector& vecMin, const Vector& vecMax, Vector out[4]) {
	out[0] = Vector(vecMin.x, vecMin.y, vecMin.z);
	out[1] = Vector(vecMax.x, vecMin.y, vecMin.z);
	out[2] = Vector(vecMax.x, vecMax.y, vecMin.z);
	out[3] = Vector(vecMin.x, vecMax.y, vecMin.z);
}

void SURF::ZONE::CreatePoints3D(const Vector& vecMin, const Vector& vecMax, Vector out[8]) {
	out[0] = vecMin;
	out[7] = vecMax;

	for (int i = 1; i < 7; i++) {
		out[i].x = (i & 4) ? vecMax.x : vecMin.x;
		out[i].y = (i & 2) ? vecMax.y : vecMin.y;
		out[i].z = (i & 1) ? vecMax.z : vecMin.z;
	}
}

void SURF::ZONE::FillBoxMinMax(Vector& vecMin, Vector& vecMax, bool resize) {
	if (vecMin.x > vecMax.x) {
		std::swap((float&)vecMin.x, (float&)vecMax.x);
	}
	if (vecMin.y > vecMax.y) {
		std::swap((float&)vecMin.y, (float&)vecMax.y);
	}
	if (vecMin.z > vecMax.z) {
		std::swap((float&)vecMin.z, (float&)vecMax.z);
	}

	if (!resize) {
		return;
	}

	// Calculate the size of the original bounding box
	Vector size(vecMax.x - vecMin.x, vecMax.y - vecMin.y, vecMax.z - vecMin.z);

	// Generate new mins and maxs centered at (0, 0, 0)
	vecMin.x = -size.x / 2;
	vecMin.y = -size.y / 2;
	vecMin.z = -size.z / 2;

	vecMax.x = size.x / 2;
	vecMax.y = size.y / 2;
	vecMax.z = size.z / 2;
}

Vector SURF::ZONE::GetCenter(const Vector& vecMin, const Vector& vecMax) {
	return (vecMin + vecMax) / 2.0f;
}

std::string SURF::ZONE::GetZoneNameByType(EZoneType type) {
	switch (type) {
		case Zone_Start:
			return "起点";
		case Zone_End:
			return "终点";
		case Zone_Stage:
			return "关卡";
		case Zone_Checkpoint:
			return "检查点";
		case Zone_Stop:
			return "停止";
		case Zone_Teleport:
			return "传送区域";
		case Zone_Mark:
			return "标记区域";
		default:
			return "未知类型";
	}
}
