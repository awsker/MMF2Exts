#include "Common.hpp"
#include <iomanip>
using namespace std::chrono_literals;

///
/// EXTENSION CONSTRUCTOR/DESTRUCTOR
///

float (Extension::* Extension::EasingFunc[19])(float) = {
	&Extension::Linear,
	&Extension::EaseInQuad,
	&Extension::EaseOutQuad,
	&Extension::EaseInOutQuad,
	&Extension::EaseInCubic,
	&Extension::EaseOutCubic,
	&Extension::EaseInOutCubic,
	&Extension::EaseInCirc,
	&Extension::EaseOutCirc,
	&Extension::EaseInOutCirc,
	&Extension::EaseInBack,
	&Extension::EaseOutBack,
	&Extension::EaseInOutBack,
	&Extension::EaseInElastic,
	&Extension::EaseOutElastic,
	&Extension::EaseInOutElastic,
	&Extension::EaseInBounce,
	&Extension::EaseOutBounce,
	&Extension::EaseInOutBounce
};

#ifdef _WIN32
Extension::Extension(RunObject* const _rdPtr, const EDITDATA* const edPtr, const CreateObjectInfo* const cobPtr) :
	rdPtr(_rdPtr), rhPtr(_rdPtr->get_rHo()->get_AdRunHeader()), Runtime(this)
#elif defined(__ANDROID__)
Extension::Extension(const EDITDATA* const edPtr, const jobject javaExtPtr, const CreateObjectInfo* const cobPtr) :
	javaExtPtr(javaExtPtr, "Extension::javaExtPtr from Extension ctor"),
	Runtime(this, this->javaExtPtr)
#else
Extension::Extension(const EDITDATA* const edPtr, void* const objCExtPtr, const CreateObjectInfo* const cobPtr) :
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
	LinkExpression(23, LerpColor);
	LinkExpression(24, Dist3D);

	/*
		This is where you'd do anything you'd do in CreateRunObject in the original SDK

		It's the only place you'll get access to edPtr at runtime, so you should transfer
		anything from edPtr to the extension class here.

	*/

#if TEXT_OEFLAG_EXTENSION
	// Copy from edittime data into runtime data
	font.CopyFromEditFont(this, edPtr->font);

	// Set Runtime.fontChangedFunc to trigger a function if the runtime
	// changes your ext's font via the built-in font actions
	Runtime.fontChangedFunc = &Extension::OnFontChanged;
#endif

#if DARKEDIF_DISPLAY_TYPE == DARKEDIF_DISPLAY_SIMPLE
	Runtime.SetSurfaceWithSize(edPtr->objSize.width, edPtr->objSize.height);
	// surf is already constructed
	// To start, let's make the image a solid blue fill
	surf->FillImageWith(DarkEdif::SurfaceFill::Solid(DarkEdif::ColorRGB(0, 0, 127)));
#endif
}

Extension::~Extension()
{

}

// Runs every tick of Fusion's runtime, can be toggled off and back on
REFLAG Extension::Handle()
{
	/*	If your extension won't draw to the window, but it still needs
		to do something every Fusion loop, use:
			return REFLAG::NONE;

		If you don't need to do something in Handle anymore, use:
			return REFLAG::ONE_SHOT;
		...you can later re-enable Handle with Runtime.Rehandle(),
		but don't use one-shot with a displaying ext.

		If you're drawing with simple display, use:
			return surf->GetAndResetAltered() ? REFLAG::DISPLAY : REFLAG::NONE;

		If you're drawing manually, use rdPtr->get_roc()->get_changed(),
		and potentially your surfaces GetAndResetAltered(), to decide
		what REFLAG to return.
		DarkEdif expects you to do manual drawing in Extension::Display()
		and other funcs.
	*/

#if DARKEDIF_DISPLAY_TYPE >= DARKEDIF_DISPLAY_SIMPLE
	// Example variables for showing how display can be modified.
	// These values should be in your Extension, not static; this is just a brief demo,
	// to show how to easily time your ext's display changes.
	// Display does not have to be done in Handle, but you do need to return
	// REFLAG::DISPLAY here and/or do rdPtr->get_roc()->set_changed(true) when display is changed.
	static std::chrono::steady_clock clock;
	static decltype(clock)::time_point nextTick;
	static const std::uint32_t colors[] = {
		DarkEdif::ColorRGB(128, 0, 0), DarkEdif::ColorRGB(168, 157, 50),
		DarkEdif::ColorRGB(50, 168, 64), DarkEdif::ColorRGB(50, 54, 168)
	};
	static std::size_t colorIndex = 0;
	if (nextTick < clock.now())
	{
		nextTick = clock.now() + 200ms;
		if (++colorIndex >= std::size(colors))
			colorIndex = 0;
		surf->FillImageWith(DarkEdif::SurfaceFill::Solid(colors[colorIndex]));
	}
	return surf->GetAndResetAltered() ? REFLAG::DISPLAY : REFLAG::NONE;
#endif

	// Will not be called next event tick
	return REFLAG::ONE_SHOT;
}

#if PAUSABLE_EXTENSION
// Called when Fusion runtime is pausing - not just the F3 pause dialog
void Extension::FusionRuntimePaused()
{

}

// Called when Fusion runtime is resuming after a pause
void Extension::FusionRuntimeContinued()
{

}
#endif // PAUSABLE_EXTENSION

#if TEXT_OEFLAG_EXTENSION
/* Triggered when the runtime changes the font name or size via built-in actions
 * @param colorEdit If true, font color was edited. If false, the font typeface was.
 * @param rect		If not null, contains a hint as to what size the object should be.
 *					This is based on differences between original font size and current one.
 *					You can ignore this hint, but it indicates you may want to call
 *					rdPtr->get_roc()->SetSize(). */
void Extension::OnFontChanged(bool colorEdit, DarkEdif::Rect * rect)
{
	// Prevent warnings about unused variables
	(void)colorEdit;
	(void)rect;

	// We could call Runtime.Redraw() to redraw ext immediately,
	// but we'll be patient and wait for next event tick
	rdPtr->get_roc()->set_changed(true);
	Runtime.Rehandle();
}

#endif

#if DARKEDIF_DISPLAY_TYPE == DARKEDIF_DISPLAY_MANUAL

void Extension::Display()
{
	// You should manually draw the surface(s) here.
	// Surface::BlitToFrameWithExtEffects will let you draw with the ext's sprites/X/Y accounted for,
	// but note you should also manage the collision mask in GetCollisionMask().
	// If you have a single surface, and don't want simple drawing, use the GetDisplaySurface().
	//
	//surf->BlitToFrameWithExtEffects(this, Point { x offset, y offset });
}


void Extension::GetZoneInfos()
{
	LOGI(_T("##### Extension::GetZoneInfos() called.\n"));
	// You can safely do nothing here. Some extensions do:
	// rdPtr->get_rHo()->set_ImgWidth()/set_ImgHeight()
	// but it is done automatically in CF2.5, so shouldn't be necessary.
}

DarkEdif::Surface * Extension::GetDisplaySurface()
{
	// Old style MMF2 fade-in/fade-out transistions requires this function,
	// Auto-rendering of Sprite effects (alpha coeff etc) will be applied by returning a surface here.
	// If you have multiple surfaces, you should can return null here and draw them yourself.
	// If you return null here, Display() will be called instead.
	return nullptr;
}
DarkEdif::CollisionMask * Extension::GetCollisionMask(std::uint32_t flags)
{
	// If your ext supports fine collision, you should return the collision mask corresponding to
	// which pixels have collision and which don't. DarkEdif::Surface can generate collision masks,
	// but it can be computationally expensive.
	// If your ext uses only box collision, DE will detect that and never call this function.
	(void)flags;

	return nullptr;
}

#endif // DARKEDIF_DISPLAY_MANUAL

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
	if ((std::size_t)ID < Edif::SDK->ExpressionInfos.size() && Edif::SDK->ExpressionInfos[ID]->Flags.ef == ExpReturnType::String)
		return (long)Runtime.CopyString(_T(""));
	return 0;
}
