#pragma once

glm::vec4 VEC4_ADD(const glm::vec4& v1, const glm::vec4 &v2) { return v1 + v2; }
glm::vec4 VEC4_SUBTRACT(const glm::vec4& v1, const glm::vec4 &v2) { return v1 - v2; }
glm::vec4 VEC4_MULTIPLY(const glm::vec4& v1, const glm::vec4 &v2) { return v1 * v2; }
glm::vec4 VEC4_SCALE(const glm::vec4& v, float scale) { return v*scale; }
glm::vec4 VEC4_DEG(const glm::vec4* v) { return glm::degrees(*v); }
glm::vec4 VEC4_RAD(const glm::vec4* v) { return glm::radians(*v); }

glm::vec3 VEC3_ADD(const glm::vec3& v1, const glm::vec3 &v2) { return v1 + v2; }
glm::vec3 VEC3_SUBTRACT(const glm::vec3& v1, const glm::vec3 &v2) { return v1 - v2; }
glm::vec3 VEC3_MULTIPLY(const glm::vec3& v1, const glm::vec3 &v2) { return v1 * v2; }
glm::vec3 VEC3_SCALE(const glm::vec3& v, float scale) { return v*scale; }
glm::vec3 VEC3_DEG(const glm::vec3* v) { return glm::degrees(*v); }
glm::vec3 VEC3_RAD(const glm::vec3* v) { return glm::radians(*v); }

glm::quat ROTATE_QUAT(float angle, const glm::vec3& axis) { return glm::rotate(glm::quat(), glm::radians(angle), axis); }
glm::quat QUAT_MULTIPLY(const glm::quat &q1, const glm::quat q2) { return q1*q2; }
glm::quat QUAT_ADD(const glm::quat &q1, const glm::quat q2) { return q1 + q2; }
glm::quat QUAT_SCALE(const glm::quat& v, float scale) { return v*scale; }

glm::mat4 MAT4_ADD(const glm::mat4& v1, const glm::mat4 &v2) { return v1 + v2; }
glm::mat4 MAT4_SUBTRACT(const glm::mat4& v1, const glm::mat4 &v2) { return v1 - v2; }
glm::mat4 MAT4_MULTIPLY(const glm::mat4& v1, const glm::mat4 &v2) { return v1 * v2; }
glm::mat4 MAT4_SCALE(const glm::mat4& v, float scale) { return v*scale; }
glm::vec4 MAT4_TRANSFORM(const glm::mat4& v1, const glm::vec4 &v2) { return v1*v2; }

glm::mat3 MAT3_ADD(const glm::mat3& v1, const glm::mat3 &v2) { return v1 + v2; }
glm::mat3 MAT3_SUBTRACT(const glm::mat3& v1, const glm::mat3 &v2) { return v1 - v2; }
glm::mat3 MAT3_MULTIPLY(const glm::mat3& v1, const glm::mat3 &v2) { return v1 * v2; }
glm::mat3 MAT3_SCALE(const glm::mat3& v, float scale) { return v*scale; }
glm::vec3 MAT3_TRANSFORM(const glm::mat3& v1, const glm::vec3 &v2) { return v1*v2; }

glm::vec4 GET_VEC_4(const glm::quat* self) { return glm::vec4(self->x, self->y, self->z, self->w); }
glm::vec3 EULER_QUAT(const glm::quat* self) { return glm::eulerAngles(*self); }
glm::quat GET_QUAT(const glm::mat4* self) { return glm::quat_cast(*self); }
glm::mat4 GET_MAT_4(const glm::quat* self) { return glm::mat4_cast(*self); }
glm::mat4 GET_MAT_4(const glm::mat3* self) { return glm::mat4(*self); }
glm::mat3 GET_MAT_3(const glm::mat4* self) { return glm::mat3(*self); }
template<int x>
glm::vec3 GET_VEC_3(const glm::mat3* self) { return (*self)[x]; }
template<int x>
glm::vec4 GET_VEC_4(const glm::mat4* self) { return (*self)[x]; }


glm::mat4 ROTATE_MAT(float angle, const glm::vec3& axis) { return glm::rotate(glm::mat4(), glm::radians(angle), axis); }
glm::mat4 SCALE_MAT(const glm::vec3& scale) { return glm::scale(glm::mat4(), scale); }
glm::mat4 TRANSLATE_MAT(const glm::vec3& pos) { return glm::translate(glm::mat4(), pos); }

