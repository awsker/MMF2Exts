#include "Common.hpp"

///
/// EXTENSION CONSTRUCTOR/DESTRUCTOR
///

float (Extension::* Extension::EasingFunc[19])(float) = {
	& Extension::Linear,
	& Extension::EaseInQuad,
	& Extension::EaseOutQuad,
	& Extension::EaseInOutQuad,
	& Extension::EaseInCubic,
	& Extension::EaseOutCubic,
	& Extension::EaseInOutCubic,
	& Extension::EaseInCirc,
	& Extension::EaseOutCirc,
	& Extension::EaseInOutCirc,
	& Extension::EaseInBack,
	& Extension::EaseOutBack,
	& Extension::EaseInOutBack,
	& Extension::EaseInElastic,
	& Extension::EaseOutElastic,
	& Extension::EaseInOutElastic,
	& Extension::EaseInBounce,
	& Extension::EaseOutBounce,
	& Extension::EaseInOutBounce
};

#ifdef _WIN32
Extension::Extension(RunObject* const _rdPtr, const EDITDATA* const edPtr, const CreateObjectInfo* const cobPtr) :
	rdPtr(_rdPtr), rhPtr(_rdPtr->get_rHo()->get_AdRunHeader()), Runtime(this)
#elif defined(__ANDROID__)
Extension::Extension(const EDITDATA* const edPtr, const jobject javaExtPtr) :
	javaExtPtr(javaExtPtr, "Extension::javaExtPtr from Extension ctor"),
	Runtime(this, this->javaExtPtr)
#else
Extension::Extension(const EDITDATA* const edPtr, void* const objCExtPtr) :
	objCExtPtr(objCExtPtr), Runtime(this, objCExtPtr)
#endif
{
	/*
		Link all your action/condition/expression functions to their IDs to match the
		IDs in the JSON here
	*/

	LinkExpression(0, Lerp);
	LinkExpression(1, LerpUnclamped);
	LinkExpression(2, LerpAngle);
	LinkExpression(3, InvLerp);
	LinkExpression(4, InvLerpUnclamped);
	LinkExpression(5, Easing);
	LinkExpression(6, Easing2);
	LinkExpression(7, Repeat);
	LinkExpression(8, Sign);
	LinkExpression(9, DeltaAngle);
	LinkExpression(10, Move);
	LinkExpression(11, Rotate);
	LinkExpression(12, BitShiftLeft);
	LinkExpression(13, BitShiftRight);
	LinkExpression(14, Dist);
	LinkExpression(15, Dot);
	LinkExpression(16, DistToLine);
	LinkExpression(17, DistToLineLastX);
	LinkExpression(18, DistToLineLastY);
	LinkExpression(19, DistToLineLastXU);
	LinkExpression(20, DistToLineLastYU);
	LinkExpression(21, DistToLineLastNormX);
	LinkExpression(22, DistToLineLastNormY);

	/*
		This is where you'd do anything you'd do in CreateRunObject in the original SDK

		It's the only place you'll get access to edPtr at runtime, so you should transfer
		anything from edPtr to the extension class here.

	*/
}

Extension::~Extension()
{

}


REFLAG Extension::Handle()
{
	/*
		If your extension will draw to the MMF window you should first
		check if anything about its display has changed :

			if (rdPtr->roc.rcChanged)
			  return REFLAG::DISPLAY;
			else
			  return REFLAG::NONE;

		You will also need to make sure you change this flag yourself
		to 1 whenever you want to redraw your object

		If your extension won't draw to the window, but it still needs
		to do something every MMF loop use :

			return REFLAG::NONE;

		If you don't need to do something every loop, use :

			return REFLAG::ONE_SHOT;

		This doesn't mean this function can never run again. If you want MMF
		to handle your object again (causing this code to run) use this function:

			Runtime.Rehandle();

		At the end of the loop this code will run

	*/

	// Will not be called next loop
	return REFLAG::ONE_SHOT;
}


REFLAG Extension::Display()
{
	/*
		If you return REFLAG_DISPLAY in Handle() this routine will run.
	*/

	// Ok
	return REFLAG::DISPLAY;
}

short Extension::FusionRuntimePaused()
{

	// Ok
	return 0;
}

short Extension::FusionRuntimeContinued()
{

	// Ok
	return 0;
}


// These are called if there's no function linked to an ID

void Extension::UnlinkedAction(int ID)
{
	DarkEdif::MsgBox::Error(_T("Extension::UnlinkedAction() called"), _T("Running a fallback for action ID %d. Make sure you ran LinkAction()."), ID);
}

long Extension::UnlinkedCondition(int ID)
{
	DarkEdif::MsgBox::Error(_T("Extension::UnlinkedCondition() called"), _T("Running a fallback for condition ID %d. Make sure you ran LinkCondition()."), ID);
	return 0;
}

long Extension::UnlinkedExpression(int ID)
{
	DarkEdif::MsgBox::Error(_T("Extension::UnlinkedExpression() called"), _T("Running a fallback for expression ID %d. Make sure you ran LinkExpression()."), ID);
	// Unlinked A/C/E is fatal error, but try not to return null string and definitely crash it
	if ((size_t)ID < Edif::SDK->ExpressionInfos.size() && Edif::SDK->ExpressionInfos[ID]->Flags.ef == ExpReturnType::String)
		return (long)Runtime.CopyString(_T(""));
	return 0;
}
