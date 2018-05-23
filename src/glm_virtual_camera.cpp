#include "glm_virtual_camera.h"

GLMVirtualCamera::GLMVirtualCamera() {
	mLengthInterEye       = 0.065f;
	mLengthBackOfHeadEyeX = 0.25f;
	mLengthSpineToEyeY    = 0.15f;

	reset();
}

void GLMVirtualCamera::reset() {
	mInvertedMove = false;
	mInvertedLook = false;

	mForward   = glm::vec3(0.0f, 0.0f, -1.0f);
	mUp        = glm::vec3(0.0f, 1.0f,  0.0f);
	mRight     = glm::vec3(1.0f, 0.0f,  0.0f);

	mRads = 0.0f;

	mPosition  = glm::vec3(0.0f, 0.0f, 0.0f);

	zeroMovement();

	setProjectionMatrix(0.1f, 100.0f, 67.0 * M_PI /180.0f, 1920, 1080);
	
}

// -----------------------------------------------------------------------------
// 								Move functions (position)
// -----------------------------------------------------------------------------

void GLMVirtualCamera::moveForwards(float distance) {
	invertedMoveAdd(mForwardMovement, distance);
	updateTranslationMatrices();
}

void GLMVirtualCamera::moveBackwards(float distance) {
	invertedMoveAdd(mForwardMovement, -distance);
	updateTranslationMatrices();
}

void GLMVirtualCamera::moveRight(float distance) {
	invertedMoveAdd(mRightMovement, distance);
	updateTranslationMatrices();
}

void GLMVirtualCamera::moveLeft(float distance) {
	invertedMoveAdd(mRightMovement, -distance);
	updateTranslationMatrices();
}

void GLMVirtualCamera::moveUp(float distance) {
	invertedMoveAdd(mUpwardsMovement, distance);
	updateTranslationMatrices();
}

void GLMVirtualCamera::moveDown(float distance) {
	invertedMoveAdd(mUpwardsMovement, -distance);
	updateTranslationMatrices();
}

void GLMVirtualCamera::zeroMovement() {
	mForwardMovement = 0.0f;
	mRightMovement   = 0.0f;
	mUpwardsMovement = 0.0f;
}

void GLMVirtualCamera::invertedMoveAdd(float &movementType, float distance) {
	if (mInvertedMove) {
		movementType -= distance;
	} else {
		movementType += distance;
	}
}

void GLMVirtualCamera::updateTranslationMatrices() {
	mPosition += mRightMovement   * mRight;
	mPosition += mUpwardsMovement * mUp;
	mPosition += mForwardMovement * mForward;

	mTranslationMatrix[3][0] = mPosition.x;
	mTranslationMatrix[3][1] = mPosition.y;
	mTranslationMatrix[3][2] = mPosition.z;

	mInverseTranslationMatrix = glm::inverse(mTranslationMatrix);

	zeroMovement();
}

// -----------------------------------------------------------------------------
// 								Look functions (rotations)
// -----------------------------------------------------------------------------

void GLMVirtualCamera::lookUp(float rads) {
	invertedLookAdd(mRads, rads);
	rotate(mRads, mRight);
}

void GLMVirtualCamera::lookDown(float rads) {
	invertedLookAdd(mRads, -rads);
	rotate(mRads, mRight);
}

void GLMVirtualCamera::lookRight(float rads) {
	invertedLookAdd(mRads, -rads);
	rotate(mRads, mUp);
}

void GLMVirtualCamera::lookLeft(float rads) {
	invertedLookAdd(mRads, rads);
	rotate(mRads, mUp);
}

void GLMVirtualCamera::rollRight(float rads) {
	invertedLookAdd(mRads, rads);
	rotate(mRads, mForward);
}

void GLMVirtualCamera::rollLeft(float rads) {
	invertedLookAdd(mRads, -rads);
	rotate(mRads, mForward);
}

void GLMVirtualCamera::invertedLookAdd(float &rotationType, float rads) {
	if (mInvertedLook) {
		rotationType -= rads;
	} else {
		rotationType += rads;
	}
}

void GLMVirtualCamera::updateRotationMatrices() {
	mRotationMatrix = mat4_cast(mQuat);
	mInverseRotationMatrix = glm::inverse(mRotationMatrix);

	mForward = glm::vec3(mRotationMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
	mUp      = glm::vec3(mRotationMatrix * glm::vec4(0.0f, 1.0f,  0.0f, 0.0f));
	mRight   = glm::vec3(mRotationMatrix * glm::vec4(1.0f, 0.0f,  0.0f, 0.0f));
}

void GLMVirtualCamera::rotate(float rads, glm::vec3 &axis) {
	glm::quat rotation;
	rotation.x = sinf(rads/2.0f) * axis.x;
	rotation.y = sinf(rads/2.0f) * axis.y;
	rotation.z = sinf(rads/2.0f) * axis.z;
	rotation.w = cosf(rads/2.0f);

	mQuat = rotation * mQuat;

	updateRotationMatrices();

	mRads = 0.0f;
}

// -----------------------------------------------------------------------------
// 								Getter and Setter functions
// -----------------------------------------------------------------------------

glm::mat4* GLMVirtualCamera::getProjectionMatrix() {
	return &mProjectionMatrix;
}

glm::mat4* GLMVirtualCamera::getTranslationMatrix() {
	updateTranslationMatrices();
	return &mTranslationMatrix;
}

glm::mat4* GLMVirtualCamera::getInverseTranslationMatrix() {
	updateTranslationMatrices();
	return &mInverseTranslationMatrix;
}

glm::mat4* GLMVirtualCamera::getRotationMatrix() {
	updateRotationMatrices();
	return &mRotationMatrix;
}

glm::mat4* GLMVirtualCamera::getInverseRotationMatrix() {
	updateRotationMatrices();
	return &mInverseRotationMatrix;
}

glm::mat4* GLMVirtualCamera::getViewMatrix() {
	mViewMatrix = *getRotationMatrix() * *getTranslationMatrix();
	return &mViewMatrix;
}

glm::mat4* GLMVirtualCamera::getInverseViewMatrix() {
	mInverseViewMatrix = *getInverseRotationMatrix() * *getInverseTranslationMatrix();
	return &mInverseViewMatrix;
}

void GLMVirtualCamera::setProjectionMatrix(float near, float far, float fov, float width, float height) {
	float range = tan(fov / 2.0) * near;
	float aspect = width / height;

	float sx = (2.0f * near) / (2 * range * aspect); 
	float sy = near / range;
	float sz = -(far + near) / (far - near);
	float pz = -(2.0f * far * near) / (far - near);

	mProjectionMatrix[0][0] = sx;
	mProjectionMatrix[0][1] = 0.0f;
	mProjectionMatrix[0][2] = 0.0f;
	mProjectionMatrix[0][3] = 0.0f;

	mProjectionMatrix[1][0] = 0.0f;
	mProjectionMatrix[1][1] = sy;
	mProjectionMatrix[1][2] = 0.0f;
	mProjectionMatrix[1][3] = 0.0f;

	mProjectionMatrix[2][0] = 0.0f;
	mProjectionMatrix[2][1] = 0.0f;
	mProjectionMatrix[2][2] = sz;
	mProjectionMatrix[2][3] = -1.0f;

	mProjectionMatrix[3][0] = 0.0f;
	mProjectionMatrix[3][1] = 0.0f;
	mProjectionMatrix[3][2] = pz;
	mProjectionMatrix[3][3] = 0.0f;
}

void GLMVirtualCamera::setOrientation(glm::quat &orientation) {
	mQuat.x = orientation.x;
	mQuat.y = orientation.y;
	mQuat.z = orientation.z;
	mQuat.w = orientation.w;

	updateRotationMatrices();
}

void GLMVirtualCamera::setInvertedLook(bool v) {
	mInvertedLook = v;
}

void GLMVirtualCamera::setInvertedMove(bool v) {
	mInvertedMove = v;
}

void GLMVirtualCamera::setPosition(glm::vec3 &pos) {
	mPosition = pos;
	mViewMatrix = glm::lookAt(mPosition, mPosition + mForward, mUp);
}

// -----------------------------------------------------------------------------
// 								Print functions
// -----------------------------------------------------------------------------

void GLMVirtualCamera::printGLMVec3(glm::vec3 &vec) {
	std::cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")" << std::endl;
}

void GLMVirtualCamera::printGLMVec4(glm::vec4 &vec) {
	std::cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")" << std::endl;
}

void GLMVirtualCamera::printForward() {
	updateRotationMatrices();
	std::cout << "mForward vector: " << std::endl;
	printGLMVec3(mForward);
}

void GLMVirtualCamera::printUp() {
	updateRotationMatrices();
	std::cout << "mUp vector: " << std::endl;
	printGLMVec3(mUp);
}

void GLMVirtualCamera::printRight() {
	updateRotationMatrices();
	std::cout << "mRight vector: " << std::endl;
	printGLMVec3(mRight);
}

void GLMVirtualCamera::printPosition() {
	std::cout << "Position: " << std::endl;
	printGLMVec3(mPosition);
}

void GLMVirtualCamera::printQuat(glm::quat &quat) {
	std::cout << "(" << quat.x << ", " << quat.y << ", " << quat.z << ", " << quat.w << ")" << std::endl;
}

void GLMVirtualCamera::printQuat() {
	std::cout << "Quaternion: " << std::endl;
	printQuat(mQuat);
}

void GLMVirtualCamera::printGLMMat4(glm::mat4 &mat) {
	std::string output;

	for (int i = 0; i < 4; i++) {
		std::cout << "| " << mat[0][i] << " " << mat[1][i] << " " << mat[2][i] << " " << mat[3][i] << " |" << std::endl;
	}
}

void GLMVirtualCamera::printProjectionMatrix() {
	std::cout << "Projection Matrix: " << std::endl;
	printGLMMat4(mProjectionMatrix);
}

void GLMVirtualCamera::printTranslationMatrix() {
	updateTranslationMatrices();
	std::cout << "Translation Matrix: " << std::endl;
	printGLMMat4(mTranslationMatrix);
}

void GLMVirtualCamera::printRotationMatrix() {
	updateRotationMatrices();
	std::cout << "Rotation Matrix: " << std::endl;
	printGLMMat4(mRotationMatrix);
}

void GLMVirtualCamera::printViewMatrix() {
	getViewMatrix();
	std::cout << "View Matrix: " << std::endl;
	printGLMMat4(mViewMatrix);
}







