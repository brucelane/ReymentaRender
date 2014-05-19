#pragma once

#include <string>
#include <vector>

#include "cinder/Cinder.h"
#include "cinder/app/AppNative.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rand.h"
#include "Logger.h"
#include "cinder/Utilities.h"
#include "cinder/Filesystem.h"
#include "cinder/Capture.h"

#include "cinder/gl/Fbo.h"
#include "cinder/Camera.h"
// parameters
#include "ParameterBag.h"

// gstreamer
//#include "_2RealGStreamerWrapper.h"

using namespace ci;
using namespace ci::app;
using namespace std;
//using namespace _2RealGStreamerWrapper;

namespace reymenta
{
	// stores the pointer to the Textures instance
	typedef std::shared_ptr<class Textures> TexturesRef;

	class Textures {
	public:		
		Textures(ParameterBagRef aParameterBag);
		virtual					~Textures();
		static TexturesRef	create(ParameterBagRef aParameterBag)
		{
			return shared_ptr<Textures>(new Textures( aParameterBag));
		}
		void						setTexture( int index, string fileName );
		ci::gl::Texture				getTexture( int index );
		int							getTextureCount() { return mTexturesCount; };
		int							getSequenceTextureCount() { return totalFrames; };
		//void						setTextureChannel( int channel );
		//int							getTextureChannel( int channel );
		void						setLayer( int aLayer );
		int							getCurrentLayer() { return mCurrentLayer; };
		// from audio
		void						setAudioTexture( unsigned char *signal );
		ci::gl::Texture				getAudioTexture() { return aTexture; };
		// image
		void						loadImageFile( int index, string aFile );
		// video
		void						addMovie( int index, string path );
		// webcam
		void						setWebcamTexture( Surface8u *surface );
		void						update();
		
		// sequence textures
		ci::gl::Texture getCurrentSequenceTexture();
		ci::gl::Texture Textures::getCurrentSequenceTextureAtIndex( int index );
		void stop();
		void play();
		void pause();
		void updateSequence();
		void createFromDir( string path );
		void createFromPathList( vector<string> paths );
		void createFromTextureList( vector<ci::gl::Texture> textureList );

		int getPlayheadFrameInc() { return playheadFrameInc; }
		void setPlayheadFrameInc( int frames ) { playheadFrameInc = frames; }

		int getPlayheadPosition() { return playheadPosition; }
		void setPlayheadPosition( int newPosition );

		void setLooping( bool doLoop ) { looping = doLoop; }

		bool isPlaying() { return playing; }
		bool isPaused() { return paused; }  
		// webcam/ruttetra put in private
		void initWebcam();
		Capture						mWebcam;
		gl::Fbo mFbo3;

	protected:
		int playheadPosition;
		int playheadFrameInc;

		int totalFrames;
		bool looping;
		bool paused;
		bool playing;
		bool complete;
	private:
		// Logger
		LoggerRef					log;	
		int							mTexturesCount;
		//int							iTextures[9];
		ci::gl::Texture				previousTexture;
		ci::gl::Texture				currentTexture;
		ci::gl::Texture				aTexture;
		unsigned char				dTexture[1024];
		int							mCurrentLayer;
		vector<ci::gl::Texture>		sTextures;
		vector<ci::gl::Texture>		sequenceTextures;
		// gStreamer
		//vector<shared_ptr<GStreamerWrapper> >	m_Players;
		double						m_dLastTime;
		float						m_fSeekPos;
		float						m_fOldSeekPos;
		float						m_fSpeed;
		float						m_fVolume;
		int							m_iLoopMode;
		int							m_iTilesDivisor;
		int							m_iTileWidth;
		int							m_iTileHeight;
		bool						m_bUseVideoBuffer;
		bool						m_bUseAudioBuffer;
		bool						movieLoaded;
		// fbo
		//gl::Fbo mFbo, mFbo2;

		//CameraPersp mCamera;
		// parameters
		ParameterBagRef				mParameterBag;

	};
}