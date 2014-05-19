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
		void toggleOriginUpperLeft(const bool &pressed);

		// panels
		void useTransition(const bool &pressed);
		void useOutput(const bool &pressed);
		void setMode(const int &aMode, const bool &pressed);
		void setLayer(const int &aLayer, const bool &pressed);
		void createRenderWindow(const int &aIndex, const bool &pressed) { mParameterBag->mWindowToCreate = aIndex; }
		void addRenderWindow(const bool &pressed) { mParameterBag->mWindowToCreate = RENDER_MORE; };
		void deleteRenderWindows(const bool &pressed) { mParameterBag->mWindowToCreate = RENDER_DELETE; };

		MinimalUI::UIElementRef				labelXY, labelPosXY, labelResoXY;
		MinimalUI::UIElementRef				sliderRenderXY, sliderRenderPosXY, sliderResoXY;

		MinimalUI::UIElementRef				labelOSC, labelError, labelLayer, labelInfo;
		MinimalUI::UIElementRef				buttonOutput;
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

		// windows mgmt
		MinimalUI::UIControllerRef mMiniControl, gParams, mSliders;
		// panels
		SlidersPanelRef				mSlidersPanel;
		vector<MinimalUI::UIControllerRef> mPanels;

		ci::app::WindowRef mWindow;
		ci::signals::scoped_connection mCbMouseDown, mCbKeyDown;
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

		// settings
		void saveSettings(const bool &pressed = true);
		void restoreSettings(const bool &pressed = true);
		void resetSettings(const bool &pressed = true);

	};
}