#include "convars2.h"

template<>
EConVarType TranslateConVarType<bool>(void) {
	return EConVarType_Bool;
}

template<>
EConVarType TranslateConVarType<int16>(void) {
	return EConVarType_Int16;
}

template<>
EConVarType TranslateConVarType<uint16>(void) {
	return EConVarType_UInt16;
}

template<>
EConVarType TranslateConVarType<int32>(void) {
	return EConVarType_Int32;
}

template<>
EConVarType TranslateConVarType<uint32>(void) {
	return EConVarType_UInt32;
}

template<>
EConVarType TranslateConVarType<int64>(void) {
	return EConVarType_Int64;
}

template<>
EConVarType TranslateConVarType<uint64>(void) {
	return EConVarType_UInt64;
}

template<>
EConVarType TranslateConVarType<float>(void) {
	return EConVarType_Float32;
}

template<>
EConVarType TranslateConVarType<double>(void) {
	return EConVarType_Float64;
}

template<>
EConVarType TranslateConVarType<const char*>(void) {
	return EConVarType_String;
}

template<>
EConVarType TranslateConVarType<Color>(void) {
	return EConVarType_Color;
}

template<>
EConVarType TranslateConVarType<Vector2D>(void) {
	return EConVarType_Vector2;
}

template<>
EConVarType TranslateConVarType<Vector>(void) {
	return EConVarType_Vector3;
}

template<>
EConVarType TranslateConVarType<Vector4D>(void) {
	return EConVarType_Vector4;
}

template<>
EConVarType TranslateConVarType<QAngle>(void) {
	return EConVarType_Qangle;
}

template<>
EConVarType TranslateConVarType<void*>(void) {
	return EConVarType_Invalid;
}

template<>
void CConVarBaseData::ValueToString<bool>(const bool& val, char* dst, size_t length) {
	const char* src = (val) ? "true" : "false";
	memcpy(dst, src, length);
}

template<>
void CConVarBaseData::ValueToString<uint16>(const uint16& val, char* dst, size_t length) {
	snprintf(dst, length, "%u", val);
}

template<>
void CConVarBaseData::ValueToString<int16>(const int16& val, char* dst, size_t length) {
	snprintf(dst, length, "%d", val);
}

template<>
void CConVarBaseData::ValueToString<uint32>(const uint32& val, char* dst, size_t length) {
	snprintf(dst, length, "%u", val);
}

template<>
void CConVarBaseData::ValueToString<int32>(const int32& val, char* dst, size_t length) {
	snprintf(dst, length, "%d", val);
}

template<>
void CConVarBaseData::ValueToString<uint64>(const uint64& val, char* dst, size_t length) {
	snprintf(dst, length, "%lu", (unsigned long)val);
}

template<>
void CConVarBaseData::ValueToString<int64>(const int64& val, char* dst, size_t length) {
	snprintf(dst, length, "%ld", (long)val);
}

template<>
void CConVarBaseData::ValueToString<float>(const float& val, char* dst, size_t length) {
	snprintf(dst, length, "%f", val);
}

template<>
void CConVarBaseData::ValueToString<double>(const double& val, char* dst, size_t length) {
	snprintf(dst, length, "%lf", val);
}

template<>
void CConVarBaseData::ValueToString<const char*>(const char* const& val, char* dst, size_t length) {
	memcpy(dst, val, length);
}

template<>
void CConVarBaseData::ValueToString<Color>(const Color& val, char* dst, size_t length) {
	snprintf(dst, length, "%d %d %d %d", val.r(), val.g(), val.b(), val.a());
}

template<>
void CConVarBaseData::ValueToString<Vector2D>(const Vector2D& val, char* dst, size_t length) {
	snprintf(dst, length, "%f %f", val.x, val.y);
}

template<>
void CConVarBaseData::ValueToString<Vector>(const Vector& val, char* dst, size_t length) {
	snprintf(dst, length, "%f %f %f", val.x, val.y, val.z);
}

template<>
void CConVarBaseData::ValueToString<Vector4D>(const Vector4D& val, char* dst, size_t length) {
	snprintf(dst, length, "%f %f %f %f", val.x, val.y, val.z, val.w);
}

template<>
void CConVarBaseData::ValueToString<QAngle>(const QAngle& val, char* dst, size_t length) {
	snprintf(dst, length, "%f %f %f", val.x, val.y, val.z);
}

template<>
bool CConVarBaseData::ValueFromString<bool>(const char* val) {
	if (strcmp(val, "true") == 0 || strcmp(val, "1") == 0) {
		return true;
	}
	return false;
}

template<>
uint16_t CConVarBaseData::ValueFromString<uint16>(const char* val) {
	unsigned int ret;
	sscanf(val, "%u", &ret);
	return ret;
}

template<>
int16_t CConVarBaseData::ValueFromString<int16>(const char* val) {
	int ret;
	sscanf(val, "%d", &ret);
	return ret;
}

template<>
uint32_t CConVarBaseData::ValueFromString<uint32>(const char* val) {
	uint32_t ret;
	sscanf(val, "%u", &ret);
	return ret;
}

template<>
int32_t CConVarBaseData::ValueFromString<int32>(const char* val) {
	int32_t ret;
	sscanf(val, "%d", &ret);
	return ret;
}

template<>
uint64_t CConVarBaseData::ValueFromString<uint64>(const char* val) {
	uint64_t ret;
	sscanf(val, "%lu", (unsigned long*)&ret);
	return ret;
}

template<>
int64_t CConVarBaseData::ValueFromString<int64>(const char* val) {
	int64_t ret;
	sscanf(val, "%ld", (long*)&ret);
	return ret;
}

template<>
float CConVarBaseData::ValueFromString<float>(const char* val) {
	float ret;
	sscanf(val, "%f", &ret);
	return ret;
}

template<>
double CConVarBaseData::ValueFromString<double>(const char* val) {
	double ret;
	sscanf(val, "%lf", &ret);
	return ret;
}

template<>
const char* CConVarBaseData::ValueFromString<const char*>(const char* val) {
	return val;
}

template<>
Color CConVarBaseData::ValueFromString<Color>(const char* val) {
	int r, g, b, a;
	sscanf(val, "%d %d %d %d", &r, &g, &b, &a);
	return Color(r, g, b, a);
}

template<>
Vector2D CConVarBaseData::ValueFromString<Vector2D>(const char* val) {
	float x, y;
	sscanf(val, "%f %f", &x, &y);
	return Vector2D(x, y);
}

template<>
Vector CConVarBaseData::ValueFromString<Vector>(const char* val) {
	float x, y, z;
	sscanf(val, "%f %f %f", &x, &y, &z);
	return Vector(x, y, z);
}

template<>
Vector4D CConVarBaseData::ValueFromString<Vector4D>(const char* val) {
	float x, y, z, w;
	sscanf(val, "%f %f %f %f", &x, &y, &z, &w);
	return Vector4D(x, y, z, w);
}

template<>
QAngle CConVarBaseData::ValueFromString<QAngle>(const char* val) {
	float x, y, z;
	sscanf(val, "%f %f %f", &x, &y, &z);
	return QAngle(x, y, z);
}

void RegisterConVarS2(ConVarCreation_t& cvar) {
	reinterpret_cast<ICvarS2*>(g_pCVar)->RegisterConVar(cvar, 0, cvar.m_pHandle, cvar.m_pConVarData);
	if (!cvar.m_pHandle->IsValid()) {
		SURF_ASSERT(false);
	}
}

void UnRegisterConVar(ConVarHandleS2& cvar) {
	if (cvar.IsValid()) {
		reinterpret_cast<ICvarS2*>(g_pCVar)->UnregisterConVar(cvar);

		cvar.Invalidate();
	}
}
