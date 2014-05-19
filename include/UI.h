#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Timeline.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"

// Parameters
#include "ParameterBag.h"
#include "UIController.h"
#include "UIElement.h"
// shaders
#include "Shaders.h"
// textures
#include "Textures.h"
// panels
#include "SlidersPanel.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace MinimalUI;

namespace reymenta
{

	typedef std::shared_ptr<class UI> UIRef;

	class UI
	{

	public:
		UI(ParameterBagRef aParameterBag, ShadersRef aShadersRef, TexturesRef aTexturesRef, ci::app::WindowRef aWindow);
		static UIRef create(ParameterBagRef aParameterBag, ShadersRef aShadersRef, TexturesRef aTexturesRef, ci::app::WindowRef aWindow);

		void setup();
		void draw();
		void update();
		void resize();
		void fullscreen(const bool &pressed);
		void initReceiver(const bool &pressed);
		void toggleCustomRender(const bool &pressed);
		void togglePreviewRender(const bool &pressed);
		void toggleAudioReactive(const bool &pressed);
		void toggleLight(const bool &pressed);
		void toggleLightAuto(const bool &pressed);
		// aether
		void toggleSendToOutput(const bool &pressed);
		void toggleFade(const bool &pressed);
		void toggleRenderUI(const bool &pressed);
		void toggleOriginUpperLeft(const bool &pressed);

		void channelsSelector(const bool &pressed);
		// panels
		void useTransition(const bool &pressed);
		void useOutput(const bool &pressed);

		void useBlendmodes(const bool &pressed);
		void setMode(const int &aMode, const bool &pressed);
		void setLayer(const int &aLayer, const bool &pressed);
		void createRenderWindow(const int &aIndex, const bool &pressed) { mParameterBag->mWindowToCreate = aIndex; }
		void selectPreviewWindow(const int &aIndex, const bool &pressed) { mParameterBag->mCurrentPreview = aIndex; }
		void selectPreviewSize(const bool &pressed) { mParameterBag->mPreviewFullScreen = pressed; };
		void addRenderWindow(const bool &pressed) { mParameterBag->mWindowToCreate = RENDER_MORE; };
		void deleteRenderWindows(const bool &pressed) { mParameterBag->mWindowToCreate = RENDER_DELETE; };

		void lockFR(const bool &pressed) { mParameterBag->mLockFR = pressed; };
		void lockFG(const bool &pressed) { mParameterBag->mLockFG = pressed; };
		void lockFB(const bool &pressed) { mParameterBag->mLockFB = pressed; };
		void lockFA(const bool &pressed) { mParameterBag->mLockFA = pressed; };
		void lockBR(const bool &pressed) { mParameterBag->mLockBR = pressed; };
		void lockBG(const bool &pressed) { mParameterBag->mLockBG = pressed; };
		void lockBB(const bool &pressed) { mParameterBag->mLockBB = pressed; };
		void lockBA(const bool &pressed) { mParameterBag->mLockBA = pressed; };
		void lockRenderX(const bool &pressed) { mParameterBag->autoRenderX = pressed; };
		void lockRenderY(const bool &pressed) { mParameterBag->autoRenderY = pressed; };
		void lockPosX(const bool &pressed) { mParameterBag->autoPosX = pressed; };
		void lockPosY(const bool &pressed) { mParameterBag->autoPosY = pressed; };
		void lockResoX(const bool &pressed) { mParameterBag->autoResoX = pressed; };
		void lockResoY(const bool &pressed) { mParameterBag->autoResoY = pressed; };
		// tempo
		void tapTempo(const bool &pressed);
		void calculateTempo();

		MinimalUI::UIElementRef				sliderRed, sliderGreen, sliderBlue, sliderAlpha;
		MinimalUI::UIElementRef				sliderBackgroundRed, sliderBackgroundGreen, sliderBackgroundBlue, sliderBackgroundAlpha;
		MinimalUI::UIElementRef				labelXY, labelPosXY, labelResoXY;
		MinimalUI::UIElementRef				sliderRenderXY, sliderRenderPosXY, sliderResoXY, sliderXSpeed, sliderYSpeed;

		MinimalUI::UIElementRef				labelOSC, labelError, labelLayer, labelInfo;
		MinimalUI::UIElementRef				buttonOutput, buttonBlendmodes;
		MinimalUI::UIElementRef				buttonLayer[8];
		MinimalUI::UIElementRef				fpsMvg;
		MinimalUI::UIControllerRef			mPreviewPanel;
	private:
		void setupGlobal();
		void setupMiniControl();
		void setupSliders();
		void setupPreview();

		void show();
		void hide();
		void toggleVisibility() { mVisible ? hide() : show(); }

		void mouseDown(ci::app::MouseEvent &event);
		void keyDown(ci::app::KeyEvent &event);
		//void releaseGroup( const std::string &aGroup );

		// windows mgmt
		MinimalUI::UIControllerRef mMiniControl, gParams, mSliders;
		// panels
		SlidersPanelRef				mSlidersPanel;
		vector<MinimalUI::UIControllerRef> mPanels;

		ci::app::WindowRef mWindow;
		ci::signals::scoped_connection mCbMouseDown, mCbKeyDown, mCbLeapConnect, mCbLeapDisconnect;
		int mPrevState;
		bool mVisible;
		bool mSetupComplete;

		// Parameters
		ParameterBagRef				mParameterBag;
		// Shaders
		ShadersRef					mShaders;
		// Textures
		TexturesRef					mTextures;

		Anim<float>					mTimer;
		//float						mSpeed;
		// windows to create, should be the same as in ParameterBag
		static const int			NONE = 0;
		static const int			RENDER_1 = 1;
		static const int			RENDER_2 = 2;
		static const int			RENDER_3 = 3;
		static const int			RENDER_MORE = 4;
		static const int			RENDER_DELETE = 5;
		static const int			RECEIVER = 6;
		// math
		string						formatNumber(float f);
		// tempo
		std::deque <double> buffer;
		ci::Timer timer;
		int counter;
		double averageTime;
		double currentTime;
		double startTime;
		float previousTime;
		int beatIndex; //0 to 3

		// settings
		void saveSettings(const bool &pressed = true);
		void restoreSettings(const bool &pressed = true);
		void resetSettings(const bool &pressed = true);

	};
}