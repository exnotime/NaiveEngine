#include "Camera.h"

#include <glm/gtx/transform2.hpp>	// lookAt, and perspectiveFov

void gfx::Camera::CalculateViewProjection() {
	m_CamData.Orientation = glm::normalize(m_CamData.Orientation);
	m_CamData.View	= glm::lookAt(m_CamData.Position, m_CamData.Position + this->GetForward(), this->GetUp() );
	m_CamData.Proj	= glm::perspective(m_CamData.Fov, m_CamData.Width / static_cast<float>(m_CamData.Height), m_CamData.Near, m_CamData.Far );
	m_CamData.ProjView = m_CamData.Proj * m_CamData.View;
}

void gfx::Camera::MoveWorld(const glm::vec3& distanceToMove) {
	m_CamData.Position += distanceToMove;
}

void gfx::Camera::MoveRelative(const glm::vec3& distanceToMove) {
	m_CamData.Position += distanceToMove.x * (m_CamData.Orientation * m_CamData.Right	);
	m_CamData.Position += distanceToMove.y * (m_CamData.Orientation * m_CamData.Up		);
	m_CamData.Position -= distanceToMove.z * (m_CamData.Orientation * m_CamData.Forward	);
}

void gfx::Camera::YawWorld(const float radians) {
	RotateAroundNormalizedAxis(m_CamData.Up, radians );
}

void gfx::Camera::YawRelative(const float radians) {
	RotateAroundNormalizedAxis(m_CamData.Orientation * m_CamData.Up, radians );
}

void gfx::Camera::PitchWorld(const float radians) {
	RotateAroundNormalizedAxis(m_CamData.Right, radians );
}

void gfx::Camera::PitchRelative(const float radians) {
	RotateAroundNormalizedAxis(m_CamData.Orientation * m_CamData.Right, radians );
}

void gfx::Camera::RollWorld(const float radians) {
	RotateAroundNormalizedAxis(m_CamData.Forward, radians );
}

void gfx::Camera::RollRelative(const float radians) {
	RotateAroundNormalizedAxis(m_CamData.Orientation * m_CamData.Forward, radians );
}

void gfx::Camera::RotateAroundNormalizedAxis(const glm::vec3& normalizedRotationAxis, const float radians) {
	float rotationAmount = radians * 0.5f;
	glm::quat rotation( glm::cos(rotationAmount), normalizedRotationAxis * glm::sin(rotationAmount) );
	m_CamData.Orientation = glm::normalize( rotation * m_CamData.Orientation);
}

void gfx::Camera::RotateWithQuaternion(const glm::quat& rotation) {
	m_CamData.Orientation = glm::normalize( rotation * m_CamData.Orientation);
}

void gfx::Camera::LookAt(const glm::vec3& position) {
	 glm::mat4 v = glm::lookAt(m_CamData.Position, position, this->GetUp());
	m_CamData.Orientation = glm::quat_cast(v);
}

const glm::vec3 gfx::Camera::GetForward() const {
	return m_CamData.Orientation * m_CamData.Forward;
}

const glm::vec3 gfx::Camera::GetUp() const {
	return m_CamData.Orientation * m_CamData.Up;
}

const glm::vec3 gfx::Camera::GetRight() const {
	return m_CamData.Orientation * m_CamData.Right;
}

const glm::vec3& gfx::Camera::GetPosition() const {
	return m_CamData.Position;
}

const glm::quat& gfx::Camera::GetOrientation() const {
	return m_CamData.Orientation;
}

const glm::mat4& gfx::Camera::GetView() const {
	return m_CamData.View;
}

const glm::mat4& gfx::Camera::GetProjection() const {
	return m_CamData.Proj;
}

const glm::mat4 gfx::Camera::GetViewProjection() const {
	return m_CamData.Proj * m_CamData.View;
}

const gfx::CameraData& gfx::Camera::GetData() const {
	return m_CamData;
}

glm::vec3& gfx::Camera::GetEditablePosition() {
	return m_CamData.Position;
}

gfx::CameraData& gfx::Camera::GetEditableData() {
	return m_CamData;
}

void gfx::Camera::SetPosition(const glm::vec3& newPosition) {
	m_CamData.Position = newPosition;
}

void gfx::Camera::SetOrientation(const glm::quat& newOrientation) {
	m_CamData.Orientation = newOrientation;
}

void gfx::Camera::SetMoveSpeed(const float newMoveSpeed) {
	// Just here to be overriden by its derived classes. But I don't want to force them to have an own implementation.
}