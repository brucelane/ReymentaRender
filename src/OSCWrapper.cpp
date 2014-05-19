#include "OSCWrapper.h"

using namespace reymenta;

OSC::OSC( ParameterBagRef aParameterBag, ShadersRef aShadersRef )//, WindowsManagerRef aWindowsManager )
{
	mParameterBag = aParameterBag;
	mShaders = aShadersRef;
	//mWindowsManager = aWindowsManager;
	// OSC receiver
	mOSCReceiver.setup( 9001 );
}

OSCRef OSC::create( ParameterBagRef aParameterBag, ShadersRef aShadersRef )//, WindowsManagerRef aWindowsManager )
{
	return shared_ptr<OSC>( new OSC( aParameterBag, aShadersRef ) );//, aWindowsManager ) );
}

void OSC::update()
{
	// osc
	while( mOSCReceiver.hasWaitingMessages() ) 
	{
		osc::Message message;
		mOSCReceiver.getNextMessage( &message );
		int arg0 = 0;
		int arg1 = 0;
		string oscAddress = message.getAddress();
		string oscArg0 = "";
		string oscArg1 = "";
		int numArgs = message.getNumArgs();
		if(oscAddress == "/quit")
		{
			//shutdown();
		}
		else if( oscAddress == "/pixelfrag")
		{
			oscArg0 = message.getArgAsString(0);
			mShaders->setFragString( oscArg0 );
		}		
		else if( oscAddress == "/renderwindows")//	sendOSCIntMessage("/renderwindows", mRenderWindowsCount, 0, 0 );
		{
			mParameterBag->mWindowToCreate = message.getArgAsInt32(1);
			oscArg0 = toString( message.getArgAsInt32(0) );
			oscArg1 = toString( message.getArgAsInt32(1) );

		}		
		else if(oscAddress == "/warp/loadimage")
		{
			oscArg0 = message.getArgAsString(0);
			//fs::path imagePath = oscArg0;
			//mImage = loadImage( imagePath );//CHECK
			//mSrcArea = mImage->getBounds();
			// adjust the content size of the warps
			//Warp::setSize( mWarps, mImage->getSize() );
		}		
		else if(oscAddress == "/warp/loadmovie")
		{
			oscArg0 = message.getArgAsString(0);
			//fs::path moviePath = oscArg0;
			//loadMovieFile( moviePath );
		}		

		else if(oscAddress == "/quit")
		{
			//shutdown();
		}	
		else if(oscAddress == "/activeclip/video/positionx/values")
		{
			mParameterBag->iMouse.x = message.getArgAsFloat(0);
			oscArg0 = toString( message.getArgAsFloat(0) );
			//pad->setValue( Vec2f(iMouse.x, iMouse.y ) );
		}
		else if(oscAddress == "/slider")
		{
			int name = message.getArgAsInt32(0);
			int newValue = message.getArgAsInt32(1);
			oscArg0 = toString( name );
			oscArg1 = toString( newValue );

			stringstream ssName; 
			ssName << name;
			string controlName = ssName.str(); 

			// slider
			if ( name > 0 || name < 9 )
			{
				sliderChange( name, newValue );

			}

		}
		else if(oscAddress == "/rotary")
		{
			int name = message.getArgAsInt32(0);
			int newValue = message.getArgAsInt32(1);
			oscArg0 = toString( name );
			oscArg1 = toString( newValue );

			stringstream ssName; 
			ssName << name;
			string controlName = ssName.str(); 

			// rotary
			if ( name > 10 && name < 19 )
			{
				rotaryChange( name, newValue );	

			}

		}
		else if(oscAddress == "/toggle")
		{			
			int name = message.getArgAsInt32(0);
			int newValue = message.getArgAsInt32(1);
			oscArg0 = toString( name );
			oscArg1 = toString( newValue );

			stringstream ssName; 
			ssName << name;
			string controlName = ssName.str(); 

			// toggle
			if ( name > 20 && name < 49 )
			{
				toggleChange( name, newValue );
			}

		}
		else if(oscAddress == "/composition/video/rotatex/values")
		{
			mParameterBag->iMouse.x = (message.getArgAsFloat(0)+1)  * mParameterBag->multFactor;
			oscArg0 = toString( message.getArgAsFloat(0) );
			//console() << "x: " << iMouse.x  << " y: " << iMouse.y << std::endl;
			//status->setLabel("x: "+toString(iMouse.x)+" y: "+toString(iMouse.y)+" volume: "+toString(maxVolume));

			//pad->setValue( Vec2f(iMouse.x, iMouse.y ) );
		}
		else if(oscAddress == "/composition/video/rotatey/values")
		{
			mParameterBag->iMouse.y = (message.getArgAsFloat(0)+1) * mParameterBag->multFactor;
			oscArg0 = toString( message.getArgAsFloat(0) );
			//pad->setValue( Vec2f(iMouse.x, iMouse.y ) );
		}
		else if(oscAddress == "/activeclip/video/positiony/values")
		{
			mParameterBag->iMouse.y = message.getArgAsFloat(0);
			oscArg0 = toString( message.getArgAsFloat(0) );
			//pad->setValue( Vec2f(iMouse.x, iMouse.y ) );
		}
		else
		{
			console() << "OSC message received: " << oscAddress << std::endl;
			for (int i = 0; i < message.getNumArgs(); i++) {
				cout << "-- Argument " << i << std::endl;
				cout << "---- type: " << message.getArgTypeName(i) << std::endl;
				if( message.getArgType(i) == osc::TYPE_INT32 ) {
					try {
						//cout << "------ int value: "<< message.getArgAsInt32(i) << std::endl;
						if ( i == 0 )
						{
							arg0 = message.getArgAsInt32(i);
							oscArg0 = toString( message.getArgAsInt32(i) );
						}
						if ( i == 1 )
						{
							arg1 = message.getArgAsInt32(i);
							oscArg1 = toString( message.getArgAsInt32(i) );
						}
					}
					catch (...) {
						cout << "Exception reading argument as int32" << std::endl;
					}
				}
				else if( message.getArgType(i) == osc::TYPE_FLOAT ) {
					try {
						cout << "------ float value: " << message.getArgAsFloat(i) << std::endl;
						if ( i == 0 )
						{
							oscArg0 = toString( message.getArgAsFloat(i) );
						}
						if ( i == 1 )
						{

							oscArg1 = toString( message.getArgAsFloat(i) );
						}
					}
					catch (...) {
						cout << "Exception reading argument as float" << std::endl;
					}
				}
				else if( message.getArgType(i) == osc::TYPE_STRING) {
					try {
						cout << "------ string value: " << message.getArgAsString(i).c_str() << std::endl;
						if ( i == 0 )
						{
							oscArg0 = message.getArgAsString(i).c_str();
						}
						if ( i == 1 )
						{

							oscArg1 = message.getArgAsString(i).c_str();
						}
					}
					catch (...) {
						cout << "Exception reading argument as string" << std::endl;
					}
				}
			}
			// is it a layer msg?
			int layer = 0;
			unsigned layerFound = oscAddress.find( "layer");
			if ( layerFound == 1 )
			{
				unsigned clipFound = oscAddress.find("/clip");
				if ( clipFound == 7 ) // layer must be < 10
				{
					cout << "clipFound " << clipFound;
					layer = atoi( oscAddress.substr( 6, 1 ).c_str() );
					int clip = atoi( oscAddress.substr( 12, 1 ).c_str() );
					string fileName = toString( ( layer * 10 ) + clip ) + ".fragjson";
					fs::path fragFile = getAssetPath("") / "shaders" / "fragjson" / fileName;
					if( fs::exists( fragFile ) )
					{
						mShaders->loadFragJson( fragFile.string() );
					}
				}
				else
				{
					if ( clipFound == 8 )
					{
						layer = atoi( oscAddress.substr( 6, 2 ).c_str() );
					}
				}
				// connect or preview
				unsigned connectFound = oscAddress.find("connect");
				if ( connectFound != string::npos ) cout << "connectFound " << connectFound;
			}
			//if ( layerFound != string::npos ) cout << "layerFound " << layerFound;

			unsigned found = oscAddress.find_last_of("/");
			int name = atoi( oscAddress.substr( found + 1 ).c_str() );
			if ( name > 0 )	oscProcessMessage( name, arg0, arg1 );
		}
		string oscString = "osc: " + oscAddress + " 0: " + oscArg0  + " 1: " + oscArg1;
		//mUserInterface->labelOSC->setName( oscString );
		mParameterBag->OSCMsg = oscString;
		//stringstream oscString; 
		//oscString << "osc address: " << oscAddress << " oscArg0: " << oscArg0  << " oscArg1: " << oscArg1;
		//oscStatus->setLabel( oscString );
	}	
}void OSC::oscProcessMessage( int controlName, int arg0, int arg1 )

{
	if ( arg0 < 0 ) arg0 = 0;
	if ( arg1 < 0 ) arg1 = 0;
	if ( arg0 > 4096 ) arg0 = 4096;
	if ( arg1 > 4096 ) arg1 = 4096;

	switch (controlName)
	{
		//pad
	case 0:
		mParameterBag->mRenderResoXY.x = lmap<float>(arg0, 0, 127, 0.0, 4096.0);
		mParameterBag->mRenderResoXY.y = lmap<float>(arg1, 0, 127, 0.0, 4096.0);
		break;
		//sliders
	case 2:
		break;
		//rotary
	case 12:
		break;
		//toggles
	case 21:
		break;
		//volume
	case 256:
		break;	
	default:
		break;
	}
}
void OSC::sliderChange( int name, float newValue )
{
	if ( newValue != mParameterBag->controlValues[name] )
	{
		mParameterBag->controlValues[name] = newValue;
		newValue /= 127.0;
		cout << newValue << endl;
		switch (name)
		{
		case 1:
			mParameterBag->iColor.r = newValue;
			console() << "mParameterBag->iColor.r: " << mParameterBag->iColor.r << std::endl;
			break;
		case 2:
			mParameterBag->iColor.g = newValue;
			console() << "mParameterBag->iColor.g: " << mParameterBag->iColor.g << std::endl;
			break;
		case 3:
			mParameterBag->iColor.b = newValue;
			console() << "mParameterBag->iColor.b: " << mParameterBag->iColor.b << std::endl;
			break;
		case 4:
			mParameterBag->iAlpha = newValue;
			console() << "mParameterBag->iAlpha: " << mParameterBag->iAlpha << std::endl;
			break;
		case 5:
			mParameterBag->iBackgroundColor.r = newValue;
			console() << "mParameterBag->iBackgroundColor.r: " << mParameterBag->iBackgroundColor.r << std::endl;
			break;
		case 6:
			mParameterBag->iBackgroundColor.g = newValue;
			console() << "mParameterBag->iBackgroundColor.g: " << mParameterBag->iBackgroundColor.g << std::endl;
			break;
		case 7:
			mParameterBag->iBackgroundColor.b = newValue;
			console() << "mParameterBag->iBackgroundColor.b: " << mParameterBag->iBackgroundColor.b << std::endl;
			break;
		case 8:
			mParameterBag->iBackgroundAlpha = newValue;
			console() << "mParameterBag->iBackgroundAlpha: " << mParameterBag->iBackgroundAlpha << std::endl;
			break;

		}
	}
}

void OSC::rotaryChange( int name, float newValue )
{

	mParameterBag->controlValues[name] = newValue;
	//cout << newValue << endl;
	switch (name)
	{
	case 11:
		mParameterBag->iRatio = newValue + 1;
		console() << "iRatio: " << mParameterBag->iRatio << std::endl;
		break;
	case 12:
		mParameterBag->mSpeed = newValue * 2.0;
		break;
	case 13:
		mParameterBag->iZoom = ( newValue + 0.1 ) /25.0;
		break;
	case 14:
		mParameterBag->mZPosition = ( newValue - 63.0 ) / 63.0;
		mParameterBag->iRotationSpeed = ( newValue - 63.0 ) / 63.0;
		break;
	case 15:
		mParameterBag->iBlendmode = newValue / 5.0;
		break;
	case 16:
		mParameterBag->iSteps = newValue + 1;
		break;
	case 17:
		mParameterBag->iCrossfade = lmap<float>(newValue, 0.0, 127.0, 0.0, 1.0);
		break;
	case 18:
		mParameterBag->iPixelate = lmap<float>(newValue, 0.0, 127.0, 1.0, 60.0);
		break;
	}

}
void OSC::toggleChange( int name, int newValue )
{
	mParameterBag->currentSelectedIndex = (name % 10) - 1;
	switch (name)
	{
	case 21:
		mParameterBag->mMode = mParameterBag->MODE_NORMAL;
		mParameterBag->iAlpha = 1.0f;
		mParameterBag->iBackgroundAlpha = 1.0f;
		break;
	case 22:
		mParameterBag->mMode = mParameterBag->MODE_CIRCLE;
		mParameterBag->iAlpha = 1.0f;
		mParameterBag->iBackgroundAlpha = 1.0f;
		break;

	case 34:
		mParameterBag->iAlpha = 0.5f;
		mParameterBag->iBackgroundAlpha = 0.4f;
		mParameterBag->iExposure = 1.0f;
		break;
	case 35:
		mParameterBag->iAlpha = 1.0f;
		mParameterBag->iBackgroundAlpha = 0.04f;
		mParameterBag->iExposure = 0.27f;
		break;
	case 36:
		mParameterBag->iAlpha = 1.0f;
		mParameterBag->iBackgroundAlpha = 0.0f;
		mParameterBag->iExposure = 0.09f;
		break;
	case 37:
		break;
	case 38:
		mParameterBag->iToggle = !mParameterBag->iToggle;
		break;
	case 42:
		break;
	case 43:
		break;
	case 46:

		break;
	case 47:
		break;
	case 48:
		mParameterBag->iFade = !mParameterBag->iFade;
		break;
	}
}