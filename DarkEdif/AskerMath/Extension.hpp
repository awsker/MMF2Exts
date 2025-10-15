#pragma once
#include "DarkEdif.hpp"

class Extension
{
public:

	RunHeader* rhPtr;
	RunObjectMultiPlatPtr rdPtr; // you should not need to access this
#ifdef __ANDROID__
	global<jobject> javaExtPtr;
#elif defined(__APPLE__)
	void* const objCExtPtr;
#endif

	Edif::Runtime Runtime;

	static const int MinimumBuild = 254;
	static const int Version = 1;

	// If you change OEFLAGS, make sure you modify RUNDATA so the data is available, or you'll get crashes!
	// For example, OEFLAGS::VALUES makes use of the AltVals rv struct.
	static const OEFLAGS OEFLAGS = OEFLAGS::NONE;
	static const OEPREFS OEPREFS = OEPREFS::NONE;

	static const int WindowProcPriority = 100;


#ifdef _WIN32
	Extension(RunObject* const rdPtr, const EDITDATA* const edPtr, const CreateObjectInfo* const cobPtr);
#elif defined(__ANDROID__)
	Extension(const EDITDATA* const edPtr, const jobject javaExtPtr);
#else
	Extension(const EDITDATA* const edPtr, void* const objCExtPtr);
#endif
	~Extension();

	// To add items to the Fusion Debugger, just uncomment this line.
	//DarkEdif::FusionDebugger FusionDebugger;
	// After enabling it, you run FusionDebugger.AddItemToDebugger() inside Extension's constructor
	// As an example:
	//std::tstring exampleDebuggerTextItem;

	/*  Add any data you want to store in your extension to this class
		(eg. what you'd normally store in rdPtr in old SDKs).

		Unlike rdPtr, you can store real C++ objects with constructors
		and destructors, without having to call them manually or store
		a pointer.
	*/

	float Clamp01(float value);

	static float (Extension::* EasingFunc[19])(float);

    //Easing functions
	float Linear(float time);
	float EaseInQuad(float time);
	float EaseOutQuad(float time);
	float EaseInOutQuad(float time);
	float EaseInCubic(float time);
	float EaseOutCubic(float time);
	float EaseInOutCubic(float time);
	float EaseInCirc(float time);
	float EaseOutCirc(float time);
	float EaseInOutCirc(float time);
	float EaseInBack(float time);
	float EaseOutBack(float time);
	float EaseInOutBack(float time);
	float EaseInElastic(float time);
	float EaseOutElastic(float time);
	float EaseInOutElastic(float time);
	float EaseInBounce(float time);
	float EaseOutBounce(float time);
	float EaseInOutBounce(float time);

	/*  Add your actions, conditions and expressions as real class member
		functions here. The arguments (and return type for expressions) must
		match EXACTLY what you defined in the JSON.

		Remember to link the actions, conditions and expressions to their
		numeric IDs in the class constructor (Extension.cpp)
	*/

	/// Actions

	/// Conditions

	/// Expressions
		float Dist(float x1, float y1, float x2, float y2);
		float Dot(float x1, float y1, float x2, float y2);
		float DistToLine(float px, float py, float x1, float y1, float x2, float y2);
		float DistToLineLastX();
		float DistToLineLastY();
		float DistToLineLastNormX();
		float DistToLineLastNormY();
		float DistToLineLastXU();
		float DistToLineLastYU();

		float Lerp(float startValue, float endValue, float time);
		float LerpUnclamped(float startValue, float endValue, float time);

		float LerpAngle(float startAngle, float endAngle, float time);

		float Move(float currentValue, float targetValue, float delta);
		float Rotate(float currentAngle, float targetAngle, float delta);

		float InvLerp(float startValue, float endValue, float value);
		float InvLerpUnclamped(float startValue, float endValue, float value);

		float Easing(int easingType, float startValue, float endValue, float time);
		float Easing2(int easingType, float startValue, float endValue, float startTime, float endTime, float time);

		float Repeat(float value, float length);
		float Sign(float value);
		float DeltaAngle(float currentAngle, float targetAngle);

		int BitShiftLeft(int value, int steps);
		int BitShiftRight(int value, int steps);

	/* These are called if there's no function linked to an ID */

	void UnlinkedAction(int ID);
	long UnlinkedCondition(int ID);
	long UnlinkedExpression(int ID);

	/*  These replace the functions like HandleRunObject that used to be
		implemented in Runtime.cpp. They work exactly the same, but they're
		inside the extension class.
	*/

	REFLAG Handle();
	REFLAG Display();

	short FusionRuntimePaused();
	short FusionRuntimeContinued();
};
