#include "UI.h"

using namespace reymenta;
/*
	4:3 w h
	btn: 48 36
	tex: 76 57
	pvw: 156 88
	*/
UI::UI(ParameterBagRef aParameterBag, ShadersRef aShadersRef, TexturesRef aTexturesRef, WindowRef aWindow)
{
	mParameterBag = aParameterBag;
	mShaders = aShadersRef;
	mTextures = aTexturesRef;
	mWindow = aWindow;

	mCbMouseDown = mWindow->getSignalMouseDown().connect(0, std::bind(&UI::mouseDown, this, std::placeholders::_1));
	mCbKeyDown = mWindow->getSignalKeyDown().connect(0, std::bind(&UI::keyDown, this, std::placeholders::_1));
	mVisible = true;
	mSetupComplete = false;

	mTimer = 0.0f;
	// tempo
	counter = 0;
	mParameterBag->iDeltaTime = 60 / mParameterBag->mTempo;
	previousTime = 0.0f;
	beatIndex = 0;
}

UIRef UI::create(ParameterBagRef aParameterBag, ShadersRef aShadersRef, TexturesRef aTexturesRef, app::WindowRef aWindow)
{
	return shared_ptr<UI>(new UI(aParameterBag, aShadersRef, aTexturesRef, aWindow));
}

void UI::setup()
{
	// load custom fonts (I do this once, in my UserInterface class)
	// UI fonts
	mParameterBag->mLabelFont = Font(loadResource(RES_HELVETICA_NEUE_REGULAR), 14 * 2);
	mParameterBag->mSmallLabelFont = Font(loadResource(RES_HELVETICA_NEUE_REGULAR), 12 * 2);
	mParameterBag->mIconFont = Font(loadResource(RES_GLYPHICONS_REGULAR), 18 * 2);
	mParameterBag->mHeaderFont = Font(loadResource(RES_HELVETICA), 24 * 2);
	mParameterBag->mBodyFont = Font(loadResource(RES_GARAMOND), 19 * 2);
	mParameterBag->mFooterFont = Font(loadResource(RES_GARAMOND_ITALIC), 14 * 2);

	setupGlobal();
	setupMiniControl();
	setupSliders();
	setupPreview();

	mSetupComplete = true;
}

void UI::setupMiniControl()
{
	mMiniControl = UIController::create("{ \"depth\":100, \"width\":936, \"panelColor\":\"0x44282828\", \"height\":150, \"backgroundImage\":\"0.jpg\" }");
	mMiniControl->setFont("label", mParameterBag->mLabelFont);
	mMiniControl->setFont("smallLabel", mParameterBag->mSmallLabelFont);
	mMiniControl->setFont("icon", mParameterBag->mIconFont);
	mMiniControl->setFont("header", mParameterBag->mHeaderFont);
	mMiniControl->setFont("body", mParameterBag->mBodyFont);
	mMiniControl->setFont("footer", mParameterBag->mFooterFont);
	mPanels.push_back(mMiniControl);


	// Textures select/layers
	// Button Group
	for (int i = 0; i < mTextures->getTextureCount(); i++)
	{
		buttonLayer[i] = mMiniControl->addButton(toString(i), std::bind(&UI::setLayer, this, i, std::placeholders::_1), "{ \"clear\":false, \"width\":48, \"stateless\":false, \"group\":\"layer\", \"exclusive\":true }");
	}
	mMiniControl->addButton("Save", std::bind(&UI::saveSettings, this, std::placeholders::_1), "{ \"clear\":false }");
	mMiniControl->addButton("Load", std::bind(&UI::restoreSettings, this, std::placeholders::_1), "{ \"clear\":false }");
	mMiniControl->addButton("Reset", std::bind(&UI::resetSettings, this, std::placeholders::_1), "{ \"clear\":false }");
	fpsMvg = mMiniControl->addMovingGraph("fps", &mParameterBag->mFps, "{ \"width\":76, \"min\":0.0, \"max\":120.0 }");

	// windows
	mMiniControl->addLabel("Rndr\nwin", "{ \"width\":48, \"clear\":false }");
	mMiniControl->addButton("1", std::bind(&UI::createRenderWindow, this, 1, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"group\":\"render\", \"exclusive\":true }");
	mMiniControl->addButton("2", std::bind(&UI::createRenderWindow, this, 2, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"group\":\"render\", \"exclusive\":true }");
	mMiniControl->addButton("3", std::bind(&UI::createRenderWindow, this, 3, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"group\":\"render\", \"exclusive\":true }");
	mMiniControl->addButton("+", std::bind(&UI::addRenderWindow, this, std::placeholders::_1), "{ \"clear\":false, \"stateless\":true, \"group\":\"render\", \"exclusive\":true }");
	mMiniControl->addButton("x", std::bind(&UI::deleteRenderWindows, this, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"group\":\"render\", \"exclusive\":true, \"pressed\":true }");
	mMiniControl->addButton("RCV", std::bind(&UI::initReceiver, this, std::placeholders::_1), "{ \"clear\":false }");
	mMiniControl->addButton("Cust", std::bind(&UI::toggleCustomRender, this, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"pressed\":true }");
	mMiniControl->addButton("PVW", std::bind(&UI::togglePreviewRender, this, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"pressed\":true }");
	mMiniControl->addButton("P0", std::bind(&UI::selectPreviewWindow, this, 0, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"group\":\"preview\", \"exclusive\":true }");
	mMiniControl->addButton("P1", std::bind(&UI::selectPreviewWindow, this, 1, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"group\":\"preview\", \"exclusive\":true }");
	mMiniControl->addButton("P2", std::bind(&UI::selectPreviewWindow, this, 2, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"group\":\"preview\", \"exclusive\":true }");
	mMiniControl->addButton("Size", std::bind(&UI::selectPreviewSize, this, std::placeholders::_1), "{ \"stateless\":false, \"pressed\":true }");
	// Color Sliders
	mMiniControl->addLabel("Draw color", "{ \"clear\":false }");
	sliderRed = mMiniControl->addToggleSlider("R", &mParameterBag->iColor.r, "a", std::bind(&UI::lockFR, this, std::placeholders::_1), "{ \"width\":36, \"clear\":false, \"handleVisible\":false, \"vertical\":true, \"nameColor\":\"0xEEFF0000\" }", "{ \"width\":9, \"stateless\":false, \"clear\":false }");
	sliderGreen = mMiniControl->addToggleSlider("G", &mParameterBag->iColor.g, "a", std::bind(&UI::lockFG, this, std::placeholders::_1), "{ \"width\":36, \"clear\":false, \"handleVisible\":false, \"vertical\":true, \"nameColor\":\"0xEE00FF00\" }", "{ \"width\":9, \"stateless\":false, \"clear\":false }");
	sliderBlue = mMiniControl->addToggleSlider("B", &mParameterBag->iColor.b, "a", std::bind(&UI::lockFB, this, std::placeholders::_1), "{ \"width\":36, \"clear\":false, \"handleVisible\":false, \"vertical\":true, \"nameColor\":\"0xEE0000FF\" }", "{ \"width\":9, \"stateless\":false, \"clear\":false }");
	sliderAlpha = mMiniControl->addToggleSlider("A", &mParameterBag->iAlpha, "a", std::bind(&UI::lockFA, this, std::placeholders::_1), "{ \"width\":36, \"clear\":false, \"handleVisible\":false, \"vertical\":true, \"nameColor\":\"0xFFFFFFFF\" }", "{ \"width\":9, \"stateless\":false, \"clear\":false }");

	mMiniControl->addLabel("Back color", "{ \"clear\":false }");
	sliderBackgroundRed = mMiniControl->addToggleSlider("R", &mParameterBag->iBackgroundColor.r, "a", std::bind(&UI::lockBR, this, std::placeholders::_1), "{ \"width\":36, \"clear\":false, \"handleVisible\":false, \"vertical\":true, \"nameColor\":\"0xEEFF0000\" }", "{ \"width\":9, \"stateless\":false, \"clear\":false }");
	sliderBackgroundGreen = mMiniControl->addToggleSlider("G", &mParameterBag->iBackgroundColor.g, "a", std::bind(&UI::lockBG, this, std::placeholders::_1), "{ \"width\":36, \"clear\":false, \"handleVisible\":false, \"vertical\":true, \"nameColor\":\"0xEE00FF00\" }", "{ \"width\":9, \"stateless\":false, \"clear\":false }");
	sliderBackgroundBlue = mMiniControl->addToggleSlider("B", &mParameterBag->iBackgroundColor.b, "a", std::bind(&UI::lockBB, this, std::placeholders::_1), "{ \"width\":36, \"clear\":false, \"handleVisible\":false, \"vertical\":true, \"nameColor\":\"0xEE0000FF\" }", "{ \"width\":9, \"stateless\":false, \"clear\":false }");
	sliderBackgroundAlpha = mMiniControl->addToggleSlider("A", &mParameterBag->iBackgroundAlpha, "a", std::bind(&UI::lockBA, this, std::placeholders::_1), "{ \"width\":36, \"clear\":false, \"handleVisible\":false, \"vertical\":true, \"nameColor\":\"0xFFFFFFFF\" }", "{ \"width\":9, \"stateless\":false }");
}
void UI::setupGlobal()
{
	//mParameterBag->mSpeed = 1.0f;
	gParams = UIController::create("{  \"depth\":300, \"width\":936, \"height\":360, \"y\":156, \"marginLarge\":2, \"fboNumSamples\":2, \"panelColor\":\"0x44282828\", \"defaultBackgroundColor\":\"0xFF0d0d0d\", \"defaultNameColor\":\"0xFF90a5b6\", \"defaultStrokeColor\":\"0xFF282828\", \"activeStrokeColor\":\"0xFF919ea7\" }", mWindow);
	//gParams = UIController::create( "{ \"width\":700, \"marginLarge\":2, \"fboNumSamples\":2, \"panelColor\":\"0x44282828\", \"defaultBackgroundColor\":\"0xFF0d0d0d\", \"defaultNameColor\":\"0xFF90a5b6\", \"defaultStrokeColor\":\"0xFF2828FF\", \"activeStrokeColor\":\"0xFFFF0000\", \"x\":0, \"y\":10 }", mWindow );
	gParams->setFont("label", mParameterBag->mLabelFont);
	gParams->setFont("smallLabel", mParameterBag->mSmallLabelFont);
	gParams->setFont("icon", mParameterBag->mIconFont);
	gParams->setFont("header", mParameterBag->mHeaderFont);
	gParams->setFont("body", mParameterBag->mBodyFont);
	gParams->setFont("footer", mParameterBag->mFooterFont);
	mPanels.push_back(gParams);

	// 2D Sliders
	//gParams->addSlider2D( "leftRenderXY", &mParameterBag->mLeftRenderXY, "{ \"clear\":false, \"minX\":-2.0, \"maxX\":2.0, \"minY\":-2.0, \"maxY\":2.0, \"width\":" + toString( mParameterBag->mPreviewWidth ) +" }" );
	labelXY = gParams->addLabel("MiddleXY", "{ \"clear\":false, \"width\":" + toString(mParameterBag->mPreviewWidth) + " }");
	labelPosXY = gParams->addLabel("MiddlePosXY", "{ \"clear\":false, \"width\":" + toString(mParameterBag->mPreviewWidth) + " }");
	labelResoXY = gParams->addLabel("MiddleResoXY", "{ \"clear\":false, \"width\":" + toString(mParameterBag->mPreviewWidth) + " }");
	labelOSC = gParams->addLabel("OSC", "{ \"width\":430 }");

	sliderRenderXY = gParams->addSlider2D("renderXY", &mParameterBag->mRenderXY, "{ \"clear\":false, \"minX\":-2.0, \"maxX\":2.0, \"minY\":-2.0, \"maxY\":2.0, \"width\":" + toString(mParameterBag->mPreviewWidth) + " }");

	string posXY = toString(mParameterBag->mRenderWidth * 3) + ", \"minY\":" + toString(mParameterBag->mRenderHeight) + ", \"maxY\":0.0, \"width\":" + toString(mParameterBag->mPreviewWidth) + " }";
	sliderRenderPosXY = gParams->addSlider2D("renderPosXY", &mParameterBag->mRenderPosXY, "{ \"clear\":false, \"minX\":0.0, \"maxX\":" + posXY);

	string reso = toString(mParameterBag->mRenderWidth * 3) + ", \"minY\":" + toString(mParameterBag->mRenderHeight) + ", \"maxY\":0.0, \"width\":" + toString(mParameterBag->mPreviewWidth) + " }";
	sliderResoXY = gParams->addSlider2D("renderResoXY", &mParameterBag->mRenderResoXY, "{ \"minX\":0.0, \"maxX\":" + reso);
	// x Anims
	gParams->addToggleSlider("animRenderX", &mParameterBag->mRenderXY.x, "a", std::bind(&UI::lockRenderX, this, std::placeholders::_1), "{ \"clear\":false, \"width\":" + toString(mParameterBag->mPreviewWidth - 13) + ", \"min\":-2.0, \"max\":2.0 }", "{ \"clear\":false, \"width\":9, \"stateless\":false }");
	gParams->addToggleSlider("animPoxX", &mParameterBag->mRenderPosXY.x, "a", std::bind(&UI::lockPosX, this, std::placeholders::_1), "{ \"clear\":false, \"width\":" + toString(mParameterBag->mPreviewWidth - 13) + ", \"min\":1, \"max\":" + toString(mParameterBag->mRenderWidth * 3) + " }", "{ \"clear\":false, \"width\":9, \"stateless\":false }");
	gParams->addToggleSlider("animResoX", &mParameterBag->mRenderResoXY.x, "a", std::bind(&UI::lockResoX, this, std::placeholders::_1), "{ \"clear\":false, \"width\":" + toString(mParameterBag->mPreviewWidth - 13) + ", \"min\":1, \"max\":" + toString(mParameterBag->mRenderWidth * 3) + " }", "{ \"clear\":false, \"width\":9, \"stateless\":false }");
	sliderXSpeed = gParams->addSlider("x speed", &mParameterBag->mXSpeed, "{ \"min\":1.0, \"max\":50.0, \"nameColor\":\"0xFFFFFFFF\", \"width\":" + toString(mParameterBag->mPreviewWidth) + " }");
	gParams->addToggleSlider("animRenderY", &mParameterBag->mRenderXY.y, "a", std::bind(&UI::lockRenderY, this, std::placeholders::_1), "{ \"clear\":false, \"width\":" + toString(mParameterBag->mPreviewWidth - 13) + ", \"min\":-2.0, \"max\":2.0 }", "{ \"clear\":false, \"width\":9, \"stateless\":false }");
	gParams->addToggleSlider("animPoxY", &mParameterBag->mRenderPosXY.y, "a", std::bind(&UI::lockPosY, this, std::placeholders::_1), "{ \"clear\":false, \"width\":" + toString(mParameterBag->mPreviewWidth - 13) + ", \"min\":1, \"max\":" + toString(mParameterBag->mRenderHeight) + " }", "{ \"clear\":false, \"width\":9, \"stateless\":false }");
	gParams->addToggleSlider("animRenderY", &mParameterBag->mRenderResoXY.y, "a", std::bind(&UI::lockResoY, this, std::placeholders::_1), "{ \"clear\":false, \"width\":" + toString(mParameterBag->mPreviewWidth - 13) + ", \"min\":1, \"max\":" + toString(mParameterBag->mRenderHeight) + " }", "{ \"clear\":false, \"width\":9, \"stateless\":false }");
	sliderYSpeed = gParams->addSlider("y speed", &mParameterBag->mYSpeed, "{ \"min\":1.0, \"max\":50.0, \"nameColor\":\"0xFFFFFFFF\", \"width\":" + toString(mParameterBag->mPreviewWidth) + " }");

	// Simple Button
	gParams->addButton("fade", std::bind(&UI::toggleFade, this, std::placeholders::_1), "{ \"clear\":false, \"width\":72, \"stateless\":false }");
	gParams->addButton("toggle", std::bind(&UI::toggleAudioReactive, this, std::placeholders::_1), "{ \"clear\":false, \"width\":72, \"stateless\":false }");
	gParams->addButton("light", std::bind(&UI::toggleLight, this, std::placeholders::_1), "{ \"clear\":false, \"width\":72, \"stateless\":false }");
	gParams->addButton("manual light", std::bind(&UI::toggleLightAuto, this, std::placeholders::_1), "{ \"clear\":false, \"width\":72, \"stateless\":false }");
	buttonBlendmodes = gParams->addButton("use blend", std::bind(&UI::useBlendmodes, this, std::placeholders::_1), "{ \"width\":72, \"stateless\":false }");

	gParams->addButton("send to\noutput", std::bind(&UI::toggleSendToOutput, this, std::placeholders::_1), "{ \"clear\":false, \"width\":72, \"stateless\":false }");
	gParams->addButton("render\nUI", std::bind(&UI::toggleRenderUI, this, std::placeholders::_1), "{ \"clear\":false, \"width\":72, \"stateless\":false }");
	gParams->addButton("origin\nupper left", std::bind(&UI::toggleOriginUpperLeft, this, std::placeholders::_1), "{ \"clear\":false, \"width\":72, \"pressed\":true, \"stateless\":false }");
	//gParams->addButton("tuio", std::bind(&UI::toggleTUIO, this, std::placeholders::_1), "{ \"width\":72, \"pressed\":true, \"stateless\":false }");

	labelInfo = gParams->addLabel("Info", "{ \"width\":430 }");
	labelError = gParams->addLabel("no error", "{ \"clear\":false, \"width\":430, \"nameColor\":\"0xFFAA0000\" }");
}
void UI::setupPreview()
{
	mPreviewPanel = UIController::create("{  \"depth\":303, \"width\":320, \"height\":240, \"y\":520, \"marginLarge\":2, \"fboNumSamples\":2, \"panelColor\":\"0x44282828\", \"defaultBackgroundColor\":\"0xFF0d0d0d\", \"defaultNameColor\":\"0xFF90a5b6\", \"defaultStrokeColor\":\"0xFF282828\", \"activeStrokeColor\":\"0xFF919ea7\" }", mWindow);
	mPanels.push_back(mPreviewPanel);
}
void UI::setupSliders()
{
	mSlidersPanel = SlidersPanel::create(mParameterBag, mShaders, mTextures, mWindow);
}
void UI::setLayer(const int &aLayer, const bool &pressed)
{
	if (pressed)
	{
		mParameterBag->currentSelectedIndex = aLayer;
		mParameterBag->iChannel0 = aLayer;
		mTextures->setLayer(aLayer);
		mParameterBag->isUIDirty = true;
	}
}


void UI::draw()
{
	// normal alpha blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (mVisible) {
		for (auto & panel : mPanels) panel->draw();
		if (mSlidersPanel) mSlidersPanel->draw();

	}

	// needed because of what the ping pong fbo is doing, at least
	gl::disableAlphaBlending();
}
string UI::formatNumber(float f)
{
	f *= 100;
	f = (float)((int)f);
	return toString(f);
}
void UI::update()
{
	for (auto & panel : mPanels) panel->update();
	// animation
	mParameterBag->mZPosition = mParameterBag->mLockZ ? sin(getElapsedFrames() / 100.0) : mParameterBag->mZPosition;
	mParameterBag->iColor.r = mParameterBag->mLockFR ? sin(getElapsedFrames() / mParameterBag->mSpeed) : mParameterBag->iColor.r;
	mParameterBag->iColor.g = mParameterBag->mLockFG ? sin(getElapsedFrames() / mParameterBag->mSpeed) : mParameterBag->iColor.g;
	mParameterBag->iColor.b = mParameterBag->mLockFB ? sin(getElapsedFrames() / mParameterBag->mSpeed) : mParameterBag->iColor.b;
	mParameterBag->iAlpha = mParameterBag->mLockFA ? sin(getElapsedFrames() / mParameterBag->mSpeed) : mParameterBag->iAlpha;
	mParameterBag->iBackgroundColor.r = mParameterBag->mLockBR ? sin(getElapsedFrames() / mParameterBag->mSpeed) : mParameterBag->iBackgroundColor.r;
	mParameterBag->iBackgroundColor.g = mParameterBag->mLockBG ? sin(getElapsedFrames() / mParameterBag->mSpeed) : mParameterBag->iBackgroundColor.g;
	mParameterBag->iBackgroundColor.b = mParameterBag->mLockBB ? sin(getElapsedFrames() / mParameterBag->mSpeed) : mParameterBag->iBackgroundColor.b;
	mParameterBag->iBackgroundAlpha = mParameterBag->mLockBA ? sin(getElapsedFrames() / mParameterBag->mSpeed) : mParameterBag->iBackgroundAlpha;

	mParameterBag->mRenderXY.x = mParameterBag->autoRenderX ? (sin(getElapsedFrames() / mParameterBag->mXSpeed) * 1.1) : mParameterBag->mRenderXY.x;
	mParameterBag->mRenderXY.y = mParameterBag->autoRenderY ? (sin(getElapsedFrames() / mParameterBag->mYSpeed) * 1.1) : mParameterBag->mRenderXY.y;

	mParameterBag->mRenderPosXY.x = mParameterBag->autoPosX ? (sin(getElapsedFrames() / mParameterBag->mXSpeed) + 1.0) * mParameterBag->mRenderWidth * 1.5 : mParameterBag->mRenderPosXY.x;
	mParameterBag->mRenderPosXY.y = mParameterBag->autoPosY ? (sin(getElapsedFrames() / mParameterBag->mYSpeed) + 1.0) * mParameterBag->mRenderWidth * 1.5 : mParameterBag->mRenderPosXY.y;

	mParameterBag->mRenderResoXY.x = mParameterBag->autoResoX ? (sin(getElapsedFrames() / mParameterBag->mXSpeed) + 1.0) * mParameterBag->mRenderWidth * 1.5 : mParameterBag->mRenderResoXY.x;
	mParameterBag->mRenderResoXY.y = mParameterBag->autoResoY ? (sin(getElapsedFrames() / mParameterBag->mYSpeed) + 1.0) * mParameterBag->mRenderWidth * 1.5 : mParameterBag->mRenderResoXY.y;
	// loop on textures
	mParameterBag->iChannel1 = mParameterBag->autoChannel1Speed ? int((sin(getElapsedFrames() / mParameterBag->mChannel1Speed) + 1.0) * 3.9) : mParameterBag->iChannel1;
	mParameterBag->iChannel2 = mParameterBag->autoChannel2Speed ? int((sin(getElapsedFrames() / mParameterBag->mChannel2Speed) + 1.0) * 3.9) : mParameterBag->iChannel2;
	mParameterBag->iChannel7 = mParameterBag->autoChannel7Speed ? int((sin(getElapsedFrames() / mParameterBag->mChannel7Speed) + 1.0) * 3.9) : mParameterBag->iChannel7;
	// exposure
	currentTime = timer.getSeconds();
	//mParameterBag->iExposure = sin(getElapsedFrames() / mParameterBag->iExposure) + 1.0;
	int time = (currentTime - startTime)*1000000.0;
	int elapsed = mParameterBag->iDeltaTime*1000000.0;
	if (elapsed > 0)
	{
		double modulo = (time % elapsed) / 1000000.0;
		mParameterBag->iTempoTime = (float)modulo;
		if (mParameterBag->iTempoTime < previousTime)
		{
			beatIndex++;
			if (beatIndex > 3) beatIndex = 0;
		}
		previousTime = mParameterBag->iTempoTime;
		// exposure
		if (mSlidersPanel->tExposure)
		{
			(modulo < 0.1) ? mParameterBag->iExposure = mSlidersPanel->maxExposure : mParameterBag->iExposure = mSlidersPanel->minExposure;
		}
		else
		{
			mParameterBag->iExposure = mSlidersPanel->autoExposure ? (sin(getElapsedFrames() / mParameterBag->mSpeed) + 1.0) : mParameterBag->iExposure;
		}
		// zoom
		if (mSlidersPanel->tZoom)
		{
			(modulo < 0.1) ? mParameterBag->iZoom = mSlidersPanel->maxZoom : mParameterBag->iZoom = mSlidersPanel->minZoom;
		}
		else
		{
			mParameterBag->iZoom = mSlidersPanel->autoZoom ? (sin(getElapsedFrames() / mParameterBag->mSpeed) + 1.0) : mParameterBag->iZoom;
		}
		// ratio
		if (mSlidersPanel->tRatio)
		{
			(modulo < 0.1) ? mParameterBag->iRatio = mSlidersPanel->maxRatio : mParameterBag->iRatio = mSlidersPanel->minRatio;
		}
		else
		{
			mParameterBag->iRatio = mSlidersPanel->autoRatio ? (sin(getElapsedFrames() / mParameterBag->mSpeed) + 1.0) : mParameterBag->iRatio;
		}
		// use frequencies
		if (!mParameterBag->mUseAudioFrequencies)
		{
			for (int i = 0; i < 4; i++)
			{
				(i == beatIndex) ? mParameterBag->iFreqs[i] = 20.0 : mParameterBag->iFreqs[i] = 0.0;
			}
		}
	}
	// iColor slider
	sliderRed->setBackgroundColor(ColorA(mParameterBag->iColor.r, 0, 0));
	sliderGreen->setBackgroundColor(ColorA(0, mParameterBag->iColor.g, 0));
	sliderBlue->setBackgroundColor(ColorA(0, 0, mParameterBag->iColor.b));
	sliderAlpha->setBackgroundColor(ColorA(mParameterBag->iColor.r, mParameterBag->iColor.g, mParameterBag->iColor.b, mParameterBag->iAlpha));
	// iBackColor sliders
	sliderBackgroundRed->setBackgroundColor(ColorA(mParameterBag->iBackgroundColor.r, 0, 0));
	sliderBackgroundGreen->setBackgroundColor(ColorA(0, mParameterBag->iBackgroundColor.g, 0));
	sliderBackgroundBlue->setBackgroundColor(ColorA(0, 0, mParameterBag->iBackgroundColor.b));
	sliderBackgroundAlpha->setBackgroundColor(ColorA(mParameterBag->iBackgroundColor.r, mParameterBag->iBackgroundColor.g, mParameterBag->iBackgroundColor.b, mParameterBag->iBackgroundAlpha));

	// other sliders
	sliderXSpeed->setName("x speed " + toString(floor(mParameterBag->mXSpeed)));
	sliderYSpeed->setName("y speed " + toString(floor(mParameterBag->mYSpeed)));
	labelXY->setName(toString(int(mParameterBag->mRenderXY.x * 100) / 100) + "x" + toString(int(mParameterBag->mRenderXY.y * 100) / 100));
	labelPosXY->setName("mouse " + toString(floor(mParameterBag->mRenderPosXY.x)) + "x" + toString(floor(mParameterBag->mRenderPosXY.y)));
	labelResoXY->setName(toString(floor(mParameterBag->mRenderResoXY.x)) + "x" + toString(floor(mParameterBag->mRenderResoXY.y)));
	// fps
	fpsMvg->setName(toString(floor(mParameterBag->mFps)) + " fps");
	if (mParameterBag->mFps > 12.0)
	{
		mParameterBag->FPSColor = UIController::DEFAULT_NAME_COLOR;
	}
	else
	{
		mParameterBag->FPSColor = mParameterBag->ColorRed;
	}
	fpsMvg->setNameColor(mParameterBag->FPSColor);

	labelOSC->setName(mParameterBag->OSCMsg);
	labelInfo->setName(mParameterBag->InfoMsg);
	labelError->setName(mShaders->getFragError());

	for (int i = 0; i < mTextures->getTextureCount(); i++)
	{
		buttonLayer[i]->setBackgroundTexture(mTextures->getTexture(i));
	}
	//gParams->setBackgroundTexture(mTextures->getTexture(((int)mParameterBag->mFps) % 6));
}

void UI::resize()
{
	for (auto & panel : mPanels) panel->resize();
}

void UI::mouseDown(MouseEvent &event)
{

}

void UI::keyDown(KeyEvent &event)
{
	switch (event.getChar())
	{
		// toggle params & mouse
	case 'h':
		toggleVisibility();
		break;
	}
}

void UI::show()
{
	mVisible = true;
	AppBasic::get()->showCursor();
}

void UI::hide()
{
	mVisible = false;
	AppBasic::get()->hideCursor();
}
void UI::initReceiver(const bool &pressed)
{
	mParameterBag->mWindowToCreate = RECEIVER;
}
void UI::toggleCustomRender(const bool &pressed)
{
	mParameterBag->mCustomRender = pressed;
}

void UI::togglePreviewRender(const bool &pressed)
{
	mParameterBag->mPreviewEnabled = pressed;
}

void UI::toggleFade(const bool &pressed)
{
	mParameterBag->iFade = pressed;
}
void UI::toggleAudioReactive(const bool &pressed)
{
	mParameterBag->iToggle = pressed;
}
void UI::toggleLight(const bool &pressed)
{
	mParameterBag->iLight = pressed;
}
void UI::toggleLightAuto(const bool &pressed)
{
	mParameterBag->iLightAuto = pressed;
}
// aether
void UI::toggleSendToOutput(const bool &pressed)
{
	mParameterBag->mSendToOutput = pressed;
}
void UI::toggleRenderUI(const bool &pressed)
{
	mParameterBag->mRenderUI = pressed;
}
void UI::toggleOriginUpperLeft(const bool &pressed)
{
	mParameterBag->mOriginUpperLeft = pressed;
}
void UI::useBlendmodes(const bool &pressed)
{
	mParameterBag->mUseBlendmodes = pressed;
}

void UI::setMode(const int &aMode, const bool &pressed)
{
	if (mParameterBag->mMode != aMode)
	{
		mParameterBag->iAlpha = 1.0f;
		mParameterBag->iBackgroundAlpha = 1.0f;

		mParameterBag->mPreviousMode = mParameterBag->mMode;
		switch (mParameterBag->mPreviousMode)
		{

		case 2:
			gParams->show();
			//mParams->show();
			break;
		}
		mParameterBag->mMode = aMode;
		switch (aMode)
		{
		case 6:
			mParameterBag->mGreyScale = true;
			mParameterBag->currentSelectedIndex = 6;

			break;
		}

	}

}
// tempo
void UI::tapTempo(const bool &pressed)
{
	startTime = currentTime = timer.getSeconds();

	timer.stop();
	timer.start();

	// check for out of time values - less than 50% or more than 150% of from last "TAP and whole time budder is going to be discarded....
	if (counter > 2 && (buffer.back() * 1.5 < currentTime || buffer.back() * 0.5 > currentTime))
	{
		buffer.clear();
		counter = 0;
		averageTime = 0;
		//mParameterBag->OSCMsg = " !!!!  TEMPO CHANGE - START AGAIN !!! " ;
	}
	if (counter >= 1)
	{
		buffer.push_back(currentTime);
		calculateTempo();
	}
	counter++;
}
void UI::calculateTempo()
{
	// NORMAL AVERAGE
	double tAverage = 0;
	for (int i = 0; i < buffer.size(); i++)
	{
		tAverage += buffer[i];
	}
	averageTime = (double)(tAverage / buffer.size());
	mParameterBag->iDeltaTime = averageTime;
	mParameterBag->mTempo = 60 / averageTime;
}
void UI::saveSettings(const bool &pressed)
{
	if (mParameterBag->save()) {
	}
	else 
	{
	}
}

void UI::restoreSettings(const bool &pressed)
{
	if (mParameterBag->restore()) {
	}
	else 
	{
	}
}

void UI::resetSettings(const bool &pressed)
{
	mParameterBag->reset();
}
