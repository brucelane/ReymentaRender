//
//  ParameterBag.cpp
//  From Nathan Selikoff on 4/26/14.
//
//

#include "ParameterBag.h"

using namespace ci;
using namespace std;
using namespace reymenta;

ParameterBag::ParameterBag()
{
	// reset no matter what, so we don't miss anything
	reset();

	// check to see if ReymentaRenderSettings.xml file exists and restore if it does
	fs::path params = getDocumentsDirectory() / "Reymenta" / "ReymentaRenderSettings.xml";
	if (fs::exists(params))
		restore();
}

ParameterBagRef ParameterBag::create()
{
	return shared_ptr<ParameterBag>(new ParameterBag());
}

bool ParameterBag::save()
{
	string filename = "ReymentaRenderSettings.xml";

	// attempt to create "Beautiful Chaos" directory in Documents directory
	fs::path directory = getDocumentsDirectory() / "Reymenta";
	if (!fs::exists(directory)) {
		if (!createDirectories(directory / filename)) {
			return false;
		}
	}

	fs::path path = directory / filename;

	XmlTree settings("settings", "");

	XmlTree OutputVideoTexture("OutputVideoTexture", "");
	OutputVideoTexture.setAttribute("value", toString(mOutputVideoTexture));
	settings.push_back(OutputVideoTexture);

	// TODO: test for successful writing of XML
	settings.write(writeFile(path));

	return true;
}

bool ParameterBag::restore()
{
	// check to see if ReymentaRenderSettings.xml file exists
	fs::path params = getDocumentsDirectory() / "Reymenta" / "ReymentaRenderSettings.xml";
	if (fs::exists(params)) {
		// if it does, restore
		const XmlTree xml(loadFile(params));

		if (!xml.hasChild("settings")) {
			return false;
		}
		else {
			const XmlTree settings = xml.getChild("settings");


			if (settings.hasChild("OutputVideoTexture")) {
				XmlTree OutputVideoTexture = settings.getChild("OutputVideoTexture");
				mOutputVideoTexture = OutputVideoTexture.getAttributeValue<bool>("value");
			}


			return true;
		}
	}
	else {
		// if it doesn't, return false
		return false;
	}
}

void ParameterBag::reset()
{
	// parameters exposed in XML

	mFboResolution = 2048;

	mOutputVideoTexture = false;
	mOutputVideoResolution = 1024;
#if defined( CINDER_COCOA )
	mOutputPlugin = "Syphon";
#else
	mOutputPlugin = "Spout";
#endif

	// parameters not exposed in XML
	mMainWindowX = 148;
	mMainWindowY = 30;
	mMainWindowWidth = 1164;
	mMainWindowHeight = 768;

	// custom render windows
	mCustomRender = false;
	mWindowLeftX = 1600;
	mWindowLeftY = 0;
	mWindowMiddleX = 2880;
	mWindowMiddleY = 0;
	mWindowRightX = 4160;
	mWindowRightY = 0;
	mSizeLeftW = 800;
	mSizeLeftH = 600;
	mSizeMiddleW = 800;
	mSizeMiddleH = 600;
	mSizeRightW = 800;
	mSizeRightH = 600;
	mWindowToCreate = NONE;

	iColor = ci::Color(1.0f, 0.0f, 0.0f);
	iBackgroundColor = ci::Color(1.0f, 1.0f, 0.0f);
	iBlendmode = 0;
	iAlpha = 1.0;
	iCrossfade = 1.0;
	//iPreview = 1;
	mCurrentFilePath = "currentMix.frag";
	iBackgroundAlpha = 0.2;
	mPreviewWidth = 156;
	mPreviewHeight = 88;
	mMarginSmall = 2;

	// OSC
	OSCMsg = "OSC listening on port 9000";
	InfoMsg = "Ready";
	if (mCustomRender)
	{
		mRenderWidth = mSizeMiddleW;
		mRenderHeight = mSizeMiddleH;
	}
	else
	{
		mRenderWidth = 1024;
		mRenderHeight = 768;
	}
	//mLeftRenderXY = mLeftRenderPosXY = Vec2f::zero();
	mRenderXY = mRenderPosXY = Vec2f::zero();
	//mRightRenderXY = mRightRenderPosXY = Vec2f::zero();
	mRenderResoXY = Vec2f(mRenderWidth, mRenderHeight);
	mCount = 1;
	mZPosition = -0.7f;
	mLockZ = mLockFR = mLockFG = mLockFB = mLockFA = mLockBR = mLockBG = mLockBB = autoChannel1Speed = autoChannel2Speed = autoChannel7Speed = false;
	mLockF0 = mLockF0 = mLockF1 = mLockF2 = false;
	autoRenderX = autoRenderY = autoPosX = autoPosY = autoResoX = autoResoY = mLockBA = false;
	iRotationSpeed = 0.05;
	// ratio
	iRatio = 20.0;
	// transition
	iTransition = 1;
	mTransitionDuration = 1.0f;
	iAnim = 0.0;
	// zoom
	iZoom = 1.0f;
	// exposure
	iExposure = 1.0;
	// freq0
	iF0 = defaultF0 = 1.0;
	minF0 = 0.1;
	maxF0 = 50.0;
	tF0 = autoF0 = false;

	// freq1
	iF1 = defaultF1 = 1.0;
	minF1 = 0.1;
	maxF1 = 50.0;
	tF1 = autoF1 = false;

	// freq2
	iF2 = defaultF2 = 1.0;
	minF2 = 0.1;
	maxF2 = 50.0;
	tF2 = autoF2 = false;

	// freq3
	iF3 = defaultF3 = 1.0;
	minF3 = 0.1;
	maxF3 = 50.0;
	tF3 = autoF3 = false;
	// audio in multiplication factor
	mAudioMultFactor = 1.0;

	iFade = mSendToOutput = iToggle = iLight = iLightAuto = false;
	iPixelate = 60.0;
	mSpeed = mXSpeed = mYSpeed = mChannel1Speed = mChannel2Speed = mChannel7Speed = 12.0;
	mSpoutInitialized = mPreviewEnabled = true;
	mCurrentPreview = 1;
	mPreviewFullScreen = true;
	//audio
	maxVolume = 0.0f;
	mData = new float[1024];
	for (int i = 0; i < 1024; i++)
	{
		mData[i] = 0;
	}
	for (int i = 0; i < 4; i++)
	{
		iFreqs[i] = i;
	}
	mUseAudioFrequencies = true;
	for (int i = 0; i < 4; i++)
	{
		iChannelTime[i] = i;
	}
	for (int i = 0; i < 4; i++)
	{
		iChannelResolution[i] = Vec3f(mRenderWidth, mRenderHeight, 1.0);
	}
	for (int c = 0; c < 128; c++)
	{
		controlValues[c] = 0;
	}
	iResolution = Vec3f(mRenderWidth, mRenderHeight, 1.0);
	mRenderResolution = Vec2i(mRenderWidth, mRenderHeight);
	mRenderWindowsCount = 0;

	mOriginUpperLeft = true;
	iSteps = 16;
	iMouse = Vec4f(mRenderWidth / 2, mRenderHeight / 2, 1.0, 1.0);
	iGlobalTime = 1.0f;
	multFactor = 126.0;
	currentSelectedIndex = 0;
	iChannel0 = 1;
	iChannel1 = 1;
	iChannel2 = 2;
	iChannel3 = 3;
	iChannel4 = 4;
	iChannel5 = 5;
	iChannel6 = 6;
	iChannel7 = 7;

	FPSColor = ColorA(0.0f, 1.0f, 0.0f, 1.0f);
	ColorGreen = ColorA(0.0f, 1.0f, 0.0f, 1.0f);
	ColorRed = ColorA(1.0f, 0.0f, 0.0f, 1.0f);
	ColorYellow = ColorA(1.0f, 1.0f, 0.0f, 1.0f);
	ColorPurple = ColorA(0.5f, 0.0f, 1.0f, 1.0f);
	isUIDirty = true;
	isShaderDirty = false;
	mShaderToLoad = "";
	mFboResolution = 2048;

	mStateTransition = 1.0;

	mFboResolution = 2048;

	mFps = 60.0;
	mTempo = 166.0;
	iDeltaTime = 60 / mTempo;
	iTempoTime = 0.0;

	// spout
	mOutputPlugin = "Spout";
	// sequence
	sequenceSubFolder = "/batchass/fox227/";
	// 
	mRenderUI = mGreyScale = mUseBlendmodes = false;

}

