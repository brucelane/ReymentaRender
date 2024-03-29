#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/AppNative.h"
#include "cinder/Json.h"
#include "Logger.h"
// textures
#include "Textures.h"
// shaders
#include "Shaders.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace reymenta
{
	// stores the pointer to the JSONWrapper instance
	typedef std::shared_ptr<class JSONWrapper> JSONWrapperRef;

	class JSONWrapper {
	public:		
		JSONWrapper();
		virtual					~JSONWrapper();
		string					parseUIJson();
		//bool					parseConfigJson();
		bool					parseAssetsJson( TexturesRef aTexturesRef );
		bool					parseFragGlobalJson();
		//string					getJsonStringValue( string fileName, string element );

		static JSONWrapperRef	create()
		{
			return shared_ptr<JSONWrapper>( new JSONWrapper() );
		}

	private:
		// Logger
		LoggerRef					log;	
		// Textures
		TexturesRef					mTextures;	
		// shaders
		ShadersRef					mShaders;

	};
}