#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Timeline.h"
#include "cinder/Json.h"
#include "cinder/Xml.h"
// mesh
#include "cinder/TriMesh.h"
#include "cinder/gl/Vbo.h"
#include "cinder/Camera.h"

// webcam
#include "cinder/Capture.h"
#include "cinder/Surface.h"
#include "cinder/MayaCamUI.h"
// fonts
#include "Resources.h"
#include "UIController.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace MinimalUI;

namespace reymenta {

	typedef std::shared_ptr<class ParameterBag> ParameterBagRef;

	class ParameterBag
	{
	public:
		ParameterBag();
		static ParameterBagRef create();

		bool save();
		bool restore();
		void reset();

		vector<MinimalUI::UIControllerRef> oPanels;

		std::string mParamString;
		//fonts
		ci::Font mLabelFont, mSmallLabelFont, mIconFont, mHeaderFont, mBodyFont, mFooterFont;

		// params
		int							mPreviewWidth, mPreviewHeight;
		int							mMainWindowX, mMainWindowY, mMarginSmall, mMainWindowWidth, mMainWindowHeight;
		std::string					OSCMsg;
		float						mFps;
		std::string					InfoMsg;
		// render windows
		int							mRenderWidth;
		int							mRenderHeight;
		Vec2f						mRenderXY;
		Vec2f						mRenderPosXY;
		Vec2f						mRenderResoXY;
		// override render windows
		bool						mCustomRender;
		bool						mPreviewFullScreen;
		int							mWindowLeftX;
		int							mWindowLeftY;
		int							mWindowMiddleX;
		int							mWindowMiddleY;
		int							mWindowRightX;
		int							mWindowRightY;
		int							mSizeLeftW;
		int							mSizeLeftH;
		int							mSizeMiddleW;
		int							mSizeMiddleH;
		int							mSizeRightW;
		int							mSizeRightH;

		bool						isUIDirty;
		bool						isShaderDirty;
		int							mCount;
		float						mZPosition;
		bool						mLockZ;
		bool						mLockF0, mLockF1, mLockF2, mLockF3;
		bool						mLockFR, mLockFG, mLockFB, mLockFA, mLockBR, mLockBG, mLockBB, mLockBA;
		bool						autoRenderX, autoRenderY, autoResoX, autoResoY, autoPosX, autoPosY, autoChannel1Speed, autoChannel2Speed, autoChannel7Speed;
		int							mRenderWindowsCount;
		string						mImageFile;
		//shaders
		ci::Color					iColor;				// color   
		ci::Color					iBackgroundColor;	// background color
		float						iAlpha;				// alpha		
		float						iBackgroundAlpha;	// background alpha		
		float						iBlendmode;			// blend modes		
		float						iRatio;
		float						iRotationSpeed;
		float						iPixelate;
		bool						iFade;
		bool						iToggle;
		bool						iLight;
		bool						iLightAuto;
		float						iSteps;
		float						iCrossfade;

		// transition
		int							iTransition;
		Anim<float>					iAnim;
		float						mTransitionDuration;
		//int							iPreview;
		int							multFactor;
		float						iFreqs[4];
		float						iChannelTime[4];
		Vec3f						iResolution;        // viewport resolution (in pixels)
		Vec3f						iChannelResolution[4];	// channel resolution (in pixels)
		float						iGlobalTime;        // shader playback time (in seconds)
		Vec4f						iMouse;             // mouse pixel coords. xy: current (if MLB down), zw: click
		float						mSpeed, mXSpeed, mYSpeed, mChannel1Speed, mChannel2Speed, mChannel7Speed;
		// windows and params
		int							mMainDisplayWidth;
		int							mRenderX;
		int							mRenderY;
		int							mDisplayCount;
		bool						mSpoutInitialized;
		bool						mPreviewEnabled;
		int							mCurrentPreview;
		string						mCurrentFilePath;
		// Textures
		bool						mOriginUpperLeft;

		int							currentSelectedIndex;
		int							iChannel0;
		int							iChannel1;
		int							iChannel2;
		int							iChannel3;
		int							iChannel4;
		int							iChannel5;
		int							iChannel6;
		int							iChannel7;
		// OSC controlled UI
		map<int,int>				controlValues;

		// modes, should be the same in App
		static const int			MODE_NORMAL = 0;
		static const int			MODE_CIRCLE = 1;

		// windows to create, should be the same in App and UI
		int							mMode, mPreviousMode;
		static const int			NONE = 0;
		static const int			RENDER_1 = 1;
		static const int			RENDER_2 = 2;
		static const int			RENDER_3 = 3;
		static const int			RENDER_MORE = 4;
		static const int			RENDER_DELETE = 5;
		static const int			RECEIVER = 6;

		int							mWindowToCreate;
		ColorA						FPSColor;
		ColorA						ColorGreen;
		ColorA						ColorRed;
		ColorA						ColorYellow;
		ColorA						ColorPurple;
		bool						mUseBlendmodes;
		// audio
		float						*mData;
		float						maxVolume;
		bool						mUseAudioFrequencies;
		float						mAudioMultFactor;

		// zoom
		float						iZoom;
		// exposure
		float						iExposure;
		// freq0
		float						iF0;
		float						defaultF0;
		float						minF0;
		float						maxF0;
		bool						tF0;
		bool						autoF0;
		// freq1
		float						iF1;
		float						defaultF1;
		float						minF1;
		float						maxF1;
		bool						tF1;
		bool						autoF1;
		// freq2
		float						iF2;
		float						defaultF2;
		float						minF2;
		float						maxF2;
		bool						tF2;
		bool						autoF2;
		// freq3
		float						iF3;
		float						defaultF3;
		float						minF3;
		float						maxF3;
		bool						tF3;
		bool						autoF3;

		ci::Anim<float> mStateTransition;

		//enum { STATE_DEFAULT, STATE_MENU, STATE_EDIT_CAMERA, STATE_EDIT_ATTRACTOR, STATE_TRANSITION_DEFAULT_TO_EDIT_CAMERA, STATE_TRANSITION_EDIT_CAMERA_TO_DEFAULT, STATE_TRANSITION_DEFAULT_TO_EDIT_ATTRACTOR, STATE_TRANSITION_EDIT_ATTRACTOR_TO_DEFAULT, STATE_DISCONNECTED };
		enum paramNames { ISPEED = 1, IZOOM = 2, IRATIO = 3, ISTEPS = 4, IPIXELATE = 5, ITOGGLE = 6 };

		Vec2i						mRenderResolution;        // render resolution (replaces iResolution which increments magically)
		bool						mGreyScale;

		// tap tempo
		float						mTempo;
		float						iDeltaTime;
		float						iTempoTime;
		// shader
		string						mShaderToLoad;
		// sequence
		std::string					sequenceSubFolder;

		int mFboResolution;

		// spout
		std::string					mOutputPlugin;
		bool						mOutputVideoTexture;
		int							mOutputVideoResolution;

		bool						mSendToOutput;
		bool						mRenderUI;

	};

}
