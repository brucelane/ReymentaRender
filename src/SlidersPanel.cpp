#include "SlidersPanel.h"

using namespace reymenta;
/*
4:3 w h
btn: 48 36
tex: 76 57
pvw: 156 88
*/
SlidersPanel::SlidersPanel(ParameterBagRef aParameterBag, ShadersRef aShadersRef, TexturesRef aTexturesRef, WindowRef aWindow)
{
	mParameterBag = aParameterBag;
	mShaders = aShadersRef;
	mTextures = aTexturesRef;
	mWindow = aWindow;

	mCbMouseDown = mWindow->getSignalMouseDown().connect(0, std::bind(&SlidersPanel::mouseDown, this, std::placeholders::_1));
	mCbKeyDown = mWindow->getSignalKeyDown().connect(0, std::bind(&SlidersPanel::keyDown, this, std::placeholders::_1));

	// zoom
	defaultZoom = 1.0f;
	minZoom = 0.1;
	maxZoom = 5.0;
	tZoom = autoZoom = false;
	// exposure
	defaultExposure = 1.0;
	minExposure = 0.0001;
	maxExposure = 2.0;
	tExposure = autoExposure = false;
	// ratio
	defaultRatio = 20.0;
	minRatio = 255.0;
	maxRatio = 0.00000000001;
	tRatio = autoRatio = false;

	setupParams();
}

SlidersPanelRef SlidersPanel::create(ParameterBagRef aParameterBag, ShadersRef aShadersRef, TexturesRef aTexturesRef, WindowRef aWindow)
{
	return shared_ptr<SlidersPanel>(new SlidersPanel(aParameterBag, aShadersRef, aTexturesRef, aWindow));
}

void SlidersPanel::setupParams()
{
	mParams = UIController::create("{ \"visible\":true, \"x\":944, \"y\":1, \"depth\":250, \"panelColor\":\"0x44282828\", \"height\":450 }");
	// set custom fonts for a UIController
	mParams->setFont("label", mParameterBag->mLabelFont);
	mParams->setFont("smallLabel", mParameterBag->mSmallLabelFont);
	mParams->setFont("icon", mParameterBag->mIconFont);
	mParams->setFont("header", mParameterBag->mHeaderFont);
	mParams->setFont("body", mParameterBag->mBodyFont);
	mParams->setFont("footer", mParameterBag->mFooterFont);

	// ratio
	sliderRatio = mParams->addToggleSlider("ratio", &mParameterBag->iRatio, "a", std::bind(&SlidersPanel::lockRatio, this, std::placeholders::_1), "{ \"clear\":false, \"width\":" + toString(mParameterBag->mPreviewWidth - 39) + ", \"min\":" + toString(minRatio) + ", \"max\":" + toString(maxRatio) + " }", "{ \"width\":9, \"stateless\":false, \"group\":\"ratio\", \"exclusive\":true, \"clear\":false }");
	mParams->addButton("t", std::bind(&SlidersPanel::tempoRatio, this, std::placeholders::_1), "{ \"width\":9, \"stateless\":false, \"group\":\"ratio\", \"exclusive\":true, \"clear\":false }");
	mParams->addButton("x", std::bind(&SlidersPanel::resetRatio, this, std::placeholders::_1), "{ \"width\":9, \"stateless\":false, \"group\":\"ratio\", \"exclusive\":true, \"pressed\":true, \"clear\":false }");
	mParams->addSlider("m", &minRatio, "{ \"min\":" + toString(minRatio) + ", \"max\":" + toString(maxRatio) + ", \"handleVisible\":false, \"width\":16, \"vertical\":true, \"clear\":false }");
	mParams->addSlider("M", &maxRatio, "{ \"min\":" + toString(minRatio) + ", \"max\":" + toString(maxRatio) + ", \"handleVisible\":false, \"width\":16, \"vertical\":true }");

	// exposure
	sliderExposure = mParams->addToggleSlider("exposure", &mParameterBag->iExposure, "a", std::bind(&SlidersPanel::lockExposure, this, std::placeholders::_1), "{ \"clear\":false, \"width\":" + toString(mParameterBag->mPreviewWidth - 39) + ", \"min\":" + toString(minExposure) + ", \"max\":" + toString(maxExposure) + " }", "{ \"width\":9, \"stateless\":false, \"group\":\"exposure\", \"exclusive\":true, \"clear\":false }");
	mParams->addButton("t", std::bind(&SlidersPanel::tempoExposure, this, std::placeholders::_1), "{ \"width\":9, \"stateless\":false, \"group\":\"exposure\", \"exclusive\":true, \"clear\":false }");
	mParams->addButton("x", std::bind(&SlidersPanel::resetExposure, this, std::placeholders::_1), "{ \"width\":9, \"stateless\":false, \"group\":\"exposure\", \"exclusive\":true, \"pressed\":true, \"clear\":false }");
	mParams->addSlider("m", &minExposure, "{ \"min\":" + toString(minExposure) + ", \"max\":" + toString(maxExposure) + ", \"handleVisible\":false, \"width\":16, \"vertical\":true, \"clear\":false }");
	mParams->addSlider("M", &maxExposure, "{ \"min\":" + toString(minExposure) + ", \"max\":" + toString(maxExposure) + ", \"handleVisible\":false, \"width\":16, \"vertical\":true }");
	// zoom
	//sliderZoom = mParams->addSlider("zoom", &mParameterBag->iZoom, "{ \"min\":5.0, \"max\":0.1 }");
	sliderZoom = mParams->addToggleSlider("zoom", &mParameterBag->iZoom, "a", std::bind(&SlidersPanel::lockZoom, this, std::placeholders::_1), "{ \"clear\":false, \"width\":" + toString(mParameterBag->mPreviewWidth - 39) + ", \"min\":" + toString(minZoom) + ", \"max\":" + toString(maxZoom) + " }", "{ \"width\":9, \"stateless\":false, \"group\":\"zoom\", \"exclusive\":true, \"clear\":false }");
	mParams->addButton("t", std::bind(&SlidersPanel::tempoZoom, this, std::placeholders::_1), "{ \"width\":9, \"stateless\":false, \"group\":\"zoom\", \"exclusive\":true, \"clear\":false }");
	mParams->addButton("x", std::bind(&SlidersPanel::resetZoom, this, std::placeholders::_1), "{ \"width\":9, \"stateless\":false, \"group\":\"zoom\", \"exclusive\":true, \"pressed\":true, \"clear\":false }");
	mParams->addSlider("m", &minZoom, "{ \"min\":" + toString(minZoom) + ", \"max\":" + toString(maxZoom) + ", \"handleVisible\":false, \"width\":16, \"vertical\":true, \"clear\":false }");
	mParams->addSlider("M", &maxZoom, "{ \"min\":" + toString(minZoom) + ", \"max\":" + toString(maxZoom) + ", \"handleVisible\":false, \"width\":16, \"vertical\":true }");

	sliderSpeed = mParams->addSlider("speed", &mParameterBag->mSpeed, "{ \"min\":1.0, \"max\":255.0, \"nameColor\":\"0xFFFFFFFF\" }");

	mParams->addToggleSlider("z Position", &mParameterBag->mZPosition, "a", std::bind(&SlidersPanel::lockZ, this, std::placeholders::_1), "{ \"width\":" + toString(mParameterBag->mPreviewWidth) + ", \"clear\":false, \"min\": -1, \"max\": 1 }", "{ \"stateless\":false }");
	sliderRotationSpeed = mParams->addSlider("rotation", &mParameterBag->iRotationSpeed, "{ \"min\":-1.0, \"max\":1.0 }");
	sliderBlendmode = mParams->addSlider("blendmode", &mParameterBag->iBlendmode, "{ \"min\":0.0, \"max\":27.0 }");
	sliderSteps = mParams->addSlider("steps", &mParameterBag->iSteps, "{ \"min\":0.0, \"max\":128.0, \"nameColor\":\"0xFFFFFFFF\" }");
	sliderPixelate = mParams->addSlider("pixelate", &mParameterBag->iPixelate, "{ \"min\":1.0, \"max\":60.0 }");
	sliderCrossfade = mParams->addSlider("xFade", &mParameterBag->iCrossfade, "{ \"min\":0.0, \"max\":1.0 }");

	mParameterBag->oPanels.push_back(mParams);
}


void SlidersPanel::update()
{

	if (mVisible)
	{
		sliderExposure->setName("exposure " + toString(floor(mParameterBag->iExposure)) + " " + toString(floor(minExposure)) + "-" + toString(floor(maxExposure)));
		sliderZoom->setName("zoom " + toString(floor(mParameterBag->iZoom)) + " " + toString(floor(minZoom)) + "-" + toString(floor(maxZoom)));
		// crossfade
		(mParameterBag->iCrossfade < 0.5) ? sliderCrossfade->setBackgroundColor(mParameterBag->ColorYellow) : sliderCrossfade->setBackgroundColor(mParameterBag->ColorPurple);
		sliderRatio->setName("ratio " + toString(floor(mParameterBag->iRatio)) + " " + toString(floor(minRatio)) + "-" + toString(floor(maxRatio)));
		sliderPixelate->setName("pixelate " + toString(floor(mParameterBag->iPixelate)));
		sliderCrossfade->setName("xFade " + formatNumber(mParameterBag->iCrossfade));
		sliderRotationSpeed->setName("rotation speed " + toString(int(mParameterBag->iRotationSpeed * 100) / 100));
		sliderBlendmode->setName("blendmode " + toString(floor(mParameterBag->iBlendmode)));
		sliderSpeed->setName("speed " + toString(floor(mParameterBag->mSpeed)));
		sliderSteps->setName("steps " + toString(floor(mParameterBag->iSteps)));
		mParams->update();
	}
}
void SlidersPanel::draw()
{
	if (mVisible)
	{
		// normal alpha blending
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		mParams->draw();
		// needed because of what the ping pong fbo is doing, at least
		gl::disableAlphaBlending();
	}
}
string SlidersPanel::formatNumber(float f)
{
	f *= 100;
	f = (float)((int)f);
	return toString(f);
}
void SlidersPanel::resize()
{
	mParams->resize();
}

void SlidersPanel::mouseDown(MouseEvent &event)
{
}

void SlidersPanel::keyDown(KeyEvent &event)
{
	//switch (event.getChar())
	//{
	//	// toggle params & mouse
	//case 'h':
	//	toggleVisibility();
	//	break;
	//}
}

void SlidersPanel::show()
{
	mVisible = true;
}

void SlidersPanel::hide()
{
	mVisible = false;
}

void SlidersPanel::releaseGroup(const std::string &aGroup)
{
	mParams->releaseGroup(aGroup);
}

void SlidersPanel::tempoZoom(const bool &pressed)
{
	tZoom = pressed;
	if (!pressed) resetZoom(pressed);
}
void SlidersPanel::resetZoom(const bool &pressed)
{
	autoZoom = false;
	tZoom = false;
	mParameterBag->iZoom = defaultZoom;
}

void SlidersPanel::tempoExposure(const bool &pressed)
{
	tExposure = pressed;
	if (!pressed) resetExposure(pressed);
}
void SlidersPanel::resetExposure(const bool &pressed)
{
	autoExposure = false;
	tExposure = false;
	mParameterBag->iExposure = defaultExposure;
}
void SlidersPanel::tempoRatio(const bool &pressed)
{
	tRatio = pressed;
	if (!pressed) resetRatio(pressed);
}
void SlidersPanel::resetRatio(const bool &pressed)
{
	autoRatio = false;
	tRatio = false;
	mParameterBag->iRatio = defaultRatio;
}