#include "Camera.h"

#include <glm/gtx/transform2.hpp>	// lookAt, and perspectiveFov

void gfx_dx::Camera::CalculateViewProjection() {
	m_Orientation	= glm::normalize( m_Orientation );
	m_CamData.View	= glm::lookAt(m_CamData.Position, m_CamData.Position + this->GetForward(), this->GetUp() );
	m_CamData.Proj	= glm::perspective(m_CamData.Fov, m_CamData.Width / static_cast<float>(m_CamData.Height), m_CamData.Near, m_CamData.Far );
	m_CamData.ProjView = m_CamData.Proj * m_CamData.View;
}

void gfx_dx::Camera::MoveWorld(const glm::vec3& distanceToMove) {
	m_CamData.Position += distanceToMove;
}

void gfx_dx::Camera::MoveRelative(const glm::vec3& distanceToMove) {
	m_CamData.Position += distanceToMove.x * (m_Orientation * m_CamData.Right	);
	m_CamData.Position += distanceToMove.y * (m_Orientation * m_CamData.Up		);
	m_CamData.Position -= distanceToMove.z * (m_Orientation * m_CamData.Forward	);
}

void gfx_dx::Camera::YawWorld(const float radians) {
	RotateAroundNormalizedAxis(m_CamData.Up, radians );
}

void gfx_dx::Camera::YawRelative(const float radians) {
	RotateAroundNormalizedAxis( m_Orientation * m_CamData.Up, radians );
}

void gfx_dx::Camera::PitchWorld(const float radians) {
	RotateAroundNormalizedAxis(m_CamData.Right, radians );
}

void gfx_dx::Camera::PitchRelative(const float radians) {
	RotateAroundNormalizedAxis( m_Orientation * m_CamData.Right, radians );
}

void gfx_dx::Camera::RollWorld(const float radians) {
	RotateAroundNormalizedAxis(m_CamData.Forward, radians );
}

void gfx_dx::Camera::RollRelative(const float radians) {
	RotateAroundNormalizedAxis( m_Orientation * m_CamData.Forward, radians );
}

void gfx_dx::Camera::RotateAroundNormalizedAxis(const glm::vec3& normalizedRotationAxis, const float radians) {
	float rotationAmount = radians * 0.5f;
	glm::quat rotation( glm::cos(rotationAmount), normalizedRotationAxis * glm::sin(rotationAmount) );
	m_Orientation = glm::normalize( rotation * m_Orientation );
}

void gfx_dx::Camera::RotateWithQuaternion(const glm::quat& rotation) {
	m_Orientation = glm::normalize( rotation * m_Orientation );
}

const glm::vec3 gfx_dx::Camera::GetForward() const {
	return m_Orientation * m_CamData.Forward;
}

const glm::vec3 gfx_dx::Camera::GetUp() const {
	return m_Orientation * m_CamData.Up;
}

const glm::vec3 gfx_dx::Camera::GetRight() const {
	return m_Orientation * m_CamData.Right;
}

const glm::vec3& gfx_dx::Camera::GetPosition() const {
	return m_CamData.Position;
}

const glm::quat& gfx_dx::Camera::GetOrientation() const {
	return m_Orientation;
}

const glm::mat4& gfx_dx::Camera::GetView() const {
	return m_CamData.View;
}

const glm::mat4& gfx_dx::Camera::GetProjection() const {
	return m_CamData.Proj;
}

const glm::mat4 gfx_dx::Camera::GetViewProjection() const {
	return m_CamData.Proj * m_CamData.View;
}

const gfx_dx::CameraData& gfx_dx::Camera::GetData() const {
	return m_CamData;
}

glm::vec3& gfx_dx::Camera::GetEditablePosition() {
	return m_CamData.Position;
}

gfx_dx::CameraData& gfx_dx::Camera::GetEditableData() {
	return m_CamData;
}

void gfx_dx::Camera::SetPosition(const glm::vec3& newPosition) {
	m_CamData.Position = newPosition;
}

void gfx_dx::Camera::SetOrientation(const glm::quat& newOrientation) {
	m_Orientation = newOrientation;
}

void gfx_dx::Camera::SetMoveSpeed(const float newMoveSpeed) {
	// Just here to be overriden by its derived classes. But I don't want to force them to have an own implementation.
}