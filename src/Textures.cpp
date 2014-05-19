#include "Textures.h"

using namespace reymenta;

Textures::Textures(ParameterBagRef aParameterBag)
{
	mParameterBag = aParameterBag;
	// instanciate the logger class
	log = Logger::create("TexturesLog.txt");
	log->logTimedString("Textures constructor");
	playheadPosition = 0;
	playheadFrameInc = 1;
	paused = false;
	playing = true;
	looping = true;
	mTexturesCount = 8;
	mCurrentLayer = 0;
	/*for( size_t m = 0; m < mTexturesCount; m++ )
	{
	iTextures[m] = m+1;
	}*/

	for (int i = 0; i < 1024; ++i) dTexture[i] = (unsigned char)(Rand::randUint() & 0xFF);

	// store it as a 512x2 texture in the first texture
	sTextures.push_back(gl::Texture(dTexture, GL_LUMINANCE, 512, 2));
	for (int j = 0; j < mTexturesCount - 1; j++)
	{
		gl::Texture glitch(500, 500);
		sTextures.push_back(glitch);
	}
	// init the vector
	sequenceTextures.push_back(gl::Texture(dTexture, GL_LUMINANCE, 512, 2));

	fs::path path = getAssetPath("");
	createFromDir(path.string() + mParameterBag->sequenceSubFolder);
	previousTexture = getCurrentSequenceTexture();
	currentTexture = getCurrentSequenceTexture();

	/*for (int i = 0; i < mWebcam.getWidth() * mWebcam.getHeight(); i++)
	{
	pixData.push_back(Vec3f::zero());
	}
	// set up the FBO (framebuffer object)
	// think of it like an off-screen window that you can draw into that can also act as a texture
	mFbo = gl::Fbo(mParameterBag->mPreviewWidth, mParameterBag->mPreviewHeight);
	mFbo2 = gl::Fbo(mParameterBag->mPreviewWidth, mParameterBag->mPreviewHeight);

	// set up the camera to be used when rendering to the FBO
	mCamera = CameraPersp(mFbo.getWidth(), mFbo.getHeight(), 60.0f, 1, 1000);
	mCamera.setEyePoint(Vec3f(2.0f, 1.0f, 2.0f));
	mCamera.setCenterOfInterestPoint(Vec3f::zero());*/
	mFbo3 = gl::Fbo(mParameterBag->mRenderWidth, mParameterBag->mRenderHeight);


}
void Textures::initWebcam()
{
	// webcam
	try {
		mWebcam = Capture(640, 480);
		mWebcam.start();
	}
	catch (...) {
		console() << "Failed to initialize capture" << std::endl;
	}
}
void Textures::setAudioTexture(unsigned char *signal)
{
	sTextures[0] = gl::Texture(signal, GL_LUMINANCE, 512, 2);
}
void Textures::setTexture(int index, string fileName)
{
	if (index > mTexturesCount - 1) index = mTexturesCount - 1;
	if (index > 0)
	{
		try
		{
			string pathToAssetFile = (getAssetPath("") / fileName).string();

			if (!fs::exists(pathToAssetFile))
			{
				log->logTimedString("asset file not found: " + fileName);
			}
			else
			{
				log->logTimedString("asset found file: " + fileName);
				if (index < sTextures.size())
				{
					sTextures[index] = gl::Texture(loadImage(loadAsset(fileName)));
				}
				else
				{
					sTextures.push_back(gl::Texture(loadImage(loadAsset(fileName))));
				}
				log->logTimedString("asset loaded: " + fileName);
			}
		}
		catch (...)
		{
			log->logTimedString("Load asset error: " + fileName);
		}
	}
}
void Textures::setLayer(int aLayer)
{
	mCurrentLayer = aLayer;
	log->logTimedString("setLayer, mCurrentLayer: " + toString(mCurrentLayer));
}

ci::gl::Texture Textures::getTexture(int index)
{
	// fbo
	if (index == 5)
	{
		return mFbo3.getTexture();
	}
	else
	{
		return sTextures[index];
	}
}

// sequence textures
ci::gl::Texture Textures::getCurrentSequenceTexture()
{
	return sequenceTextures[playheadPosition];
}
ci::gl::Texture Textures::getCurrentSequenceTextureAtIndex(int index)
{
	if (index > totalFrames - 1) index = totalFrames - 1;
	return sequenceTextures[index];
}
void Textures::loadImageFile(int index, string aFile)
{
	try
	{
		// try loading image file
		if (index > 0) sTextures[index] = gl::Texture(loadImage(aFile));
		// TODO? sTextures[index] = gl::Texture(loadImage( aFile ) );
		//iChannelResolution = Vec3i( mTexture0.getWidth(),  mTexture0.getHeight(), 1);
		//status->setLabel("loaded image file: " + mFile);
		//mSrcArea = mImage->getBounds();
		// adjust the content size of the warps
		//Warp::setSize( mWarps, mImage->getSize() );
		log->logTimedString("Image loaded:" + aFile);
	}
	catch (...)
	{
		log->logTimedString("Error loading image:" + aFile);
	}
}
//add movie
void Textures::addMovie(int index, string path)
{
	/* TODO GSTREAMER try
	{
	// 3 video textures max
	// starts at index 4
	//if ( index < 4 ) index = 4;
	//if ( index > 6 ) index = 6;
	std::shared_ptr<GStreamerWrapper> fileToLoad = std::shared_ptr<GStreamerWrapper>(new GStreamerWrapper());
	std::string uri = "file:/" +  path;
	int movieCount = m_Players.size();
	log->logTimedString( "load movie " + path + " movie count " + toString( movieCount ) );
	if ( movieCount > 3 )
	{
	m_Players[ 3 ]->stop();
	}

	if(fileToLoad->open(uri, m_bUseVideoBuffer, m_bUseAudioBuffer))
	{
	m_Players.push_back(fileToLoad);
	m_Players.back()->play();
	movieLoaded = true;
	}
	}
	catch( const std::exception &e )
	{
	log->logTimedString( "Unable to load the movie " + path + ": " + string( e.what() ) );
	}
	catch( ... )
	{
	log->logTimedString( "Unable to load the movie, " + path );

	}*/
}

void Textures::update()
{
	// gStreamer
	/* TODO GSTREAMER int index;

	for(int i=0; i<m_Players.size(); i++)
	{
	if ( i + 4 < 7 )
	{
	index = m_Players.size() - i - 1;
	m_Players[index]->update();

	if( m_Players[index]->hasVideo() && m_Players[index]->isNewVideoFrame() )
	{
	unsigned char* pImg = m_Players[index]->getVideo();
	if( pImg != nullptr )
	{
	sTextures[i+4] = gl::Texture(ci::Surface(pImg, m_Players[index]->getWidth(), m_Players[index]->getHeight(), m_Players[index]->getWidth() * 3, ci::SurfaceChannelOrder::RGB) );

	}
	}
	}
	}*/
	// webcam
	//if ( (int)getElapsedSeconds() % 6 == 0 )
	//{
	if (mWebcam)
	{
		if (mWebcam.isCapturing())
		{
			sTextures[6] = gl::Texture(mWebcam.getSurface());
		}
	}
	//}
}

void Textures::updateSequence()
{
	// sequence
	previousTexture = getCurrentSequenceTexture();
	//timeline().apply( &mAlpha, 1.0f, 2.0f ).finishFn( [&]{ textureSequence.update(); mAlpha= 1.0f; } );

	/**
	*  -- Call on each frame to update the playhead
	*/
	if (!paused && playing)
	{
		int newPosition = playheadPosition + playheadFrameInc;
		if (newPosition > totalFrames - 1)
		{
			if (looping)
			{
				complete = false;
				playheadPosition = newPosition - totalFrames;
			}
			else {
				complete = true;
			}

		}
		else if (newPosition < 0) {
			if (looping)
			{
				complete = false;
				playheadPosition = totalFrames - abs(newPosition);
			}
			else {
				complete = true;
			}

		}
		else {
			complete = false;
			playheadPosition = newPosition;
		}
	}
	sTextures[7] = getCurrentSequenceTexture();
	currentTexture = getCurrentSequenceTexture();
}
Textures::~Textures()
{
	log->logTimedString("Textures destructor");
	sTextures.clear();
	sequenceTextures.clear();
}

// sequence

/**
*  -- Begins playback of sequence
*/
void Textures::play()
{
	paused = false;
	playing = true;
}

/**
*  -- Pauses playback
*/
void Textures::pause()
{
	paused = true;
	playing = false;
}

/**
*  -- Stops playback and resets the playhead to zero
*/
void Textures::stop()
{
	playheadPosition = 0;
	playing = false;
	paused = false;
}

/**
*  -- Seek to a new position in the sequence
*/
void Textures::setPlayheadPosition(int newPosition)
{
	playheadPosition = max(0, min(newPosition, totalFrames));
}

/**
*  -- Loads all files contained in the supplied folder and creates Textures from them
*/
void Textures::createFromDir(string filePath)
{
	bool noValidFile = true; // if no valid files in the folder, we keep existing vector
	int i = 0;
	string ext = "";
	fs::path p(filePath);
	for (fs::directory_iterator it(p); it != fs::directory_iterator(); ++it)
	{
		if (fs::is_regular_file(*it))
		{
			string fileName = it->path().filename().string();
			if (fileName.find_last_of(".") != std::string::npos) ext = fileName.substr(fileName.find_last_of(".") + 1);
			if (ext == "png" || ext == "jpg")
			{
				if (noValidFile)
				{
					// we found a valid file
					noValidFile = false;
					sequenceTextures.clear();
					// reset playhead to the start
					playheadPosition = 0;
				}
				sequenceTextures.push_back(ci::gl::Texture(loadImage(filePath + fileName)));
			}
		}
	}

	totalFrames = sequenceTextures.size();
}

/**
*  -- Loads all of the images in the supplied list of file paths
*/
void Textures::createFromPathList(vector<string> paths)
{
	sequenceTextures.clear();
	for (int i = 0; i < paths.size(); ++i)
	{
		sequenceTextures.push_back(ci::gl::Texture(loadImage(paths[i])));
	}
	totalFrames = sequenceTextures.size();
}

void Textures::createFromTextureList(vector<ci::gl::Texture> textureList)
{
	sequenceTextures.clear();
	sequenceTextures = textureList;
	totalFrames = sequenceTextures.size();
}