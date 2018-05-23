#ifndef GLM_VIRTUAL_CAMERA
#define GLM_VIRTUAL_CAMERA

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

class GLMVirtualCamera {
public:
	const int LEFT_EYE  = 0;
	const int RIGHT_EYE = 1;

	GLMVirtualCamera();
	void reset();	

	void moveForwards (float distance);
	void moveBackwards(float distance);
	void moveRight    (float distance);
	void moveLeft     (float distance);
	void moveUp       (float distance);
	void moveDown     (float distance);

	void lookUp   (float rads);
	void lookDown (float rads);
	void lookRight(float rads);
	void lookLeft (float rads);
	void rollRight(float rads);
	void rollLeft (float rads);

	void setProjectionMatrix(float near, float far, float fov, float width, float height);
	void setOrientation(glm::quat &orientation);
	void setInvertedLook(bool v);
	void setInvertedMove(bool v);
	void setPosition(glm::vec3 &pos);

	glm::mat4* getProjectionMatrix(); 
	glm::mat4* getTranslationMatrix();
	glm::mat4* getInverseTranslationMatrix();
	glm::mat4* getRotationMatrix();
	glm::mat4* getInverseRotationMatrix();
	glm::mat4* getViewMatrix();
	glm::mat4* getInverseViewMatrix();

	// -----------------------------------------
	// Print functions
	// -----------------------------------------

	// Vector 
	void printGLMVec3(glm::vec3 & vec);
	void printGLMVec4(glm::vec4 & vec);
	void printForward();
	void printUp();
	void printRight();
	void printPosition();
	void printQuat(glm::quat &quat);
	void printQuat();

	// Matrix
	void printGLMMat4(glm::mat4 & mat);
	void printProjectionMatrix();
	void printTranslationMatrix();
	void printRotationMatrix();
	void printViewMatrix();

protected:

private:
	// Necessary matrices
	glm::mat4 mProjectionMatrix;
	glm::mat4 mRotationMatrix;
	glm::mat4 mInverseRotationMatrix;
	glm::mat4 mTranslationMatrix;
	glm::mat4 mInverseTranslationMatrix;
	glm::mat4 mViewMatrix;
	glm::mat4 mInverseViewMatrix;

	// The quaternion
	glm::quat mQuat;

	// Tracking position and rotation
	glm::vec3 mPosition;

	// Orientation tracking vectors
	glm::vec3 mForward;
	glm::vec3 mUp;
	glm::vec3 mRight;

	// Movement values
	float mForwardMovement;
	float mUpwardsMovement;
	float mRightMovement;

	// Rotation values (radians)
	float mRads;

	// Head measurements for use as an eye (in metres)
	float mLengthSpineToEyeY;
	float mLengthBackOfHeadEyeX;
	float mLengthInterEye;

	// Controlling whether or not to invert movement or look
	bool mInvertedLook;
	bool mInvertedMove;

	// Private methods

	// For the movement part
	void zeroMovement();
	void invertedMoveAdd(float &movementType, float distance);
	void updateTranslationMatrices();                                   // Updates both mTranslationMatrix and mInverseTranslationMatrix


	void invertedLookAdd(float &rotationType, float rads);
	void updateRotationMatrices();										// Updates both mRotationMatrix and mInverseRotationMatrix
	void rotate(float rads, glm::vec3 &axis);
};

#endif

