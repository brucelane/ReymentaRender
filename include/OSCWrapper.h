#pragma once

#include "cinder/app/AppNative.h"
#include "OscListener.h"
#include "OSCSender.h"
#include "Resources.h"
#include "ParameterBag.h"
//#include "WindowsManager.h"
#include "Shaders.h"
// UI
#include "UI.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace reymenta 
{

	typedef std::shared_ptr<class OSC> OSCRef;

	class OSC 
	{

	public:
		OSC( ParameterBagRef aParameterBag, ShadersRef aShadersRef );
		static	OSCRef create( ParameterBagRef aParameterBag, ShadersRef aShadersRef );

		void	update();

		void	sendOSCIntMessage( string controlType, int controlName, int controlValue0, int controlValue1 );
		void	sendOSCStringMessage( string controlType, string controlString );
		void	toggleChange( int name, int newValue );
		void	rotaryChange( int name, float newValue );
		void	sliderChange( int name, float newValue );

	private:

		void oscProcessMessage( int controlName, int arg0, int arg1 );
		// parameters
		ParameterBagRef mParameterBag;
		// UI
		//UIRef mUserInterface;
		osc::Listener 				mOSCReceiver;
		//osc::Sender					mOSCSender;
		// windowrefs
		//WindowsManagerRef			mWindowsManager;
		// Shaders
		ShadersRef					mShaders;
		// minimalUI
		UIRef						mUserInterface;

	};
}