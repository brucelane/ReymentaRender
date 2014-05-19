#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/AppNative.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Utilities.h"
#include "cinder/Timeline.h"

#include "Resources.h"
// log
#include "Logger.h"

// parameters
#include "ParameterBag.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace reymenta
{
	// stores the pointer to the Shaders instance
	typedef std::shared_ptr<class Shaders> ShadersRef;

	class Shaders {
	public:		
		Shaders( ParameterBagRef aParameterBag );
		virtual					~Shaders();
		static ShadersRef	create( ParameterBagRef aParameterBag )
		{
			return shared_ptr<Shaders>( new Shaders( aParameterBag ) );
		}
		void setFragBegin( string aFragString ) { mFragBegin = aFragString; };
		void setFragGlobalFunctions( string aFragString ) { mFragGlobalFunctions = aFragString; };
		//void setFragMainHeader( string aFragString ) { mFragMainHeader = aFragString; };
		void setFragFunctions( string aFragString, int leftOrRight );
		void setFragMainLines( string aFragString, int leftOrRight );
		void setFragEnd( string aFragString ) { mFragEnd = aFragString; };

		string getLeftFragFileName() { return mLeftFragFileName; };
		string getMiddleFragFileName() { return mFragFileName; };
		string getRightFragFileName() { return mRightFragFileName; };
		string getLeftFragFullPath() { return mLeftFragFile; };
		string getMiddleFragFullPath() { return mFragFile; };
		string getRightFragFullPath() { return mRightFragFile; };

		void doTransition();
		bool setFragString(string pixelFrag);
		bool loadTextFile( string aFilePath );
		void loadCurrentFrag();

		string getFragError();

		bool isValidMiddleFrag() { return validMiddleFrag; };
		bool isValidLeftFrag() { return validLeftFrag; };
		bool isValidRightFrag() { return validRightFrag; };
		bool isValidVert() { return validVert; };

		gl::GlslProg getShaderLeft() { return mShaderLeft; };
		gl::GlslProg getShader() { return mShader; };
		gl::GlslProg getShaderRight() { return mShaderRight; };

		void loadFragJson( string aFilePath );
		bool loadPixelFrag( string aFilePath, int screenPosition = 1);
		string getFileName( string aFilePath);
		string getNewFragFileName( string aFilePath);
		string getFragStringFromFile( string fileName );
		// code editor
		void createCodeEditor( WindowRef aCodeEditorWindow );
		void showCodeEditor();
		void hideCodeEditor();
		void readFile( string fileName );
		void fileDrop(FileDropEvent event);

	private:
		// Logger
		LoggerRef					log;	
		string						mFragFile;
		string						mLeftFragFile;
		string						mRightFragFile;
		string						mFragFileName;
		string						mLeftFragFileName;
		string						mRightFragFileName;
		bool						validMiddleFrag;
		bool						validLeftFrag;
		bool						validRightFrag;
		bool						validVert;
		gl::GlslProg				mShader;
		gl::GlslProg				mShaderLeft;
		gl::GlslProg				mShaderRight;
		string						fileName;
		// live coding
		string						mFragBegin;
		string						mFragGlobalFunctions, mFragBlendFunctions;
		string						mFragFunctionsLeft;
		string						mFragMainHeaderLeft;
		string						mFragMainLinesLeft;
		string						mFragFunctionsRight;
		string						mFragMainHeaderRight;
		string						mFragMainLinesRight;
		//string						mFragMainHeader;
		string						mFragEnd;
		string						mError;
		string						buildFragString();
		// parameters
		ParameterBagRef				mParameterBag;
		// current frag string
		string						currentFrag;
	};
}