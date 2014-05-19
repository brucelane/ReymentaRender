/*
	==========================================================================
	Copyright (C) 2014 Bruce LANE.

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
	==========================================================================

	*/
#include "ReymentaRenderApp.h"

void ReymentaRenderApp::prepareSettings(Settings *settings)
{
	// instanciate the logger class
	log = Logger::create("ReymentaRenderLog.txt");
	log->logTimedString("prepareSettings");

	// instanciate the json wrapper class
	mJson = JSONWrapper::create();

	// parameters
	mParameterBag = ParameterBag::create();

	settings->setWindowSize(mParameterBag->mMainWindowWidth, mParameterBag->mMainWindowHeight);
	settings->setFrameRate(120);
	settings->setWindowPos(Vec2i(mParameterBag->mMainWindowX, mParameterBag->mMainWindowY));
	settings->setResizable(true); // allowed for a receiver
}

void ReymentaRenderApp::setup()
{
	log->logTimedString("setup");
	// instanciate the textures class
	mTextures = Textures::create(mParameterBag);
	//mJson->parseConfigJson();
	mJson->parseAssetsJson(mTextures);

	// instanciate the Shaders class, must not be in prepareSettings
	mShaders = Shaders::create(mParameterBag);

	windowManagement();
	// Setup the user interface
	mUserInterface = UI::create(mParameterBag, mShaders, mTextures, mMainWindow);
	mUserInterface->setup();
	// instanciate the OSC class
	mOSC = OSC::create(mParameterBag, mShaders);
	mTimer = 0.0f;
	//initSpout();
	// -------- SPOUT -------------
	bInitialized = false;
	mParameterBag->mSpoutInitialized = false;
	// This is a receiver, so the initialization is a little more complex than a sender
	// Set up the texture we will use to receive from a sender
	SenderName[0] = NULL; // the name will be filled when the receiver connects to a sender

	g_Width = 320;
	g_Height = 240;
}

void ReymentaRenderApp::initSpout()
{
	unsigned int width, height;
	char tempname[256];

	if (!mParameterBag->mSpoutInitialized)
	{
		// -------- SPOUT -------------
		// This is a receiver, so the initialization is a little more complex than a sender
		// Set up the texture we will use to receive from a sender
		SenderName[0] = NULL; // the name will be filled when the receiver connects to a sender
		// You can pass a sender name to try to find and connect to 
		strcpy_s(tempname, 256, "Required sender name");
		width = g_Width; // pass the initial width and height (they will be adjusted if necessary)
		height = g_Height;
		bInitialized = InitReceiver(tempname, width, height, bTextureShare);
		mParameterBag->mSpoutInitialized = true;
		if (bInitialized) {
			// Check to see whether it has initialized texture share or memoryshare
			if (bTextureShare) {
				// Texture share is OK so we can look at sender names
				// Check if the name returned is different.
				if (strcmp(SenderName, tempname) != 0) {
					// If the sender name is different, the requested 
					// sender was not found so the active sender was used.
					// Act on this if necessary.
					strcpy_s(SenderName, 256, tempname);
				}
			}
			// else the receiver has initialized in memoryshare mode

			// Is the size of the detected sender different from the current texture size ?
			// This is detected for both texture share and memoryshare
			if (width != g_Width || height != g_Height) {
				g_Width = width;
				g_Height = height;
				// Reset the local receiving texture size
				spoutTexture = gl::Texture(g_Width, g_Height);
				// reset render window
				setWindowSize(g_Width, g_Height);

			}
		}
		else {
			// else receiver initialization failure
			MessageBoxA(NULL, "No sender running\nQuit and try again", "CINDER Spout Receiver", MB_OK);
		}
		// ----------------------------		
	}

}

void ReymentaRenderApp::update()
{
	if (mParameterBag->mGreyScale)
	{
		mParameterBag->iBackgroundColor.g = mParameterBag->iBackgroundColor.b = mParameterBag->iBackgroundColor.r;
		mParameterBag->iColor.g = mParameterBag->iColor.b = mParameterBag->iColor.r;
	}
	mParameterBag->iChannelTime[0] = getElapsedSeconds();
	mParameterBag->iChannelTime[1] = getElapsedSeconds() - 1;
	mParameterBag->iChannelTime[3] = getElapsedSeconds() - 2;
	mParameterBag->iChannelTime[4] = getElapsedSeconds() - 3;
	mParameterBag->iGlobalTime = getElapsedSeconds();

	mParameterBag->mFps = getAverageFps();
	mUserInterface->update();
	mTextures->update();
	mOSC->update();
	// nothing for now mOutput->update();
	if (mParameterBag->mWindowToCreate > 0)
	{
		// try to create the window only once
		int windowToCreate = mParameterBag->mWindowToCreate;
		mParameterBag->mWindowToCreate = NONE;
		switch (windowToCreate)
		{
		case RENDER_1:
			createRenderWindow(1);
			break;
		case RENDER_2:
			createRenderWindow(2);
			break;
		case RENDER_3:
			createRenderWindow(3);
			break;
		case RENDER_MORE:
			addRenderWindow();
			break;
		case RENDER_DELETE:
			deleteRenderWindows();
			break;
		case RECEIVER:
			initSpout();
			break;
		}
	}
}
void ReymentaRenderApp::drawMain()
{
	gl::clear();
	gl::setViewport(getWindowBounds());
	gl::setMatricesWindow(getWindowSize());
	gl::color(ColorAf(1.0f, 1.0f, 1.0f, 1.0f));

	if (mParameterBag->mPreviewEnabled)
	{

	}
	mUserInterface->draw();
	// Show the user what it is receiving
	char txt[256];

	gl::enableAlphaBlending();
	sprintf_s(txt, "Receiving from [%s]", SenderName);
	gl::drawString(txt, Vec2f(toPixels(20), getWindowHeight() - toPixels(120)), Color(1, 1, 1), Font("Verdana", toPixels(24)));
	sprintf_s(txt, "fps : %2.2d", (int)getAverageFps());
	gl::drawString(txt, Vec2f(getWindowWidth() - toPixels(100), getWindowHeight() - toPixels(80)), Color(1, 1, 1), Font("Verdana", toPixels(24)));
	gl::drawString("RH click to select a sender", Vec2f(toPixels(20), getWindowHeight() - toPixels(40)), Color(1, 1, 1), Font("Verdana", toPixels(24)));
	gl::disableAlphaBlending();


}
void ReymentaRenderApp::drawRender()
{
	unsigned int width, height;

	gl::setMatricesWindow(getWindowSize());
	gl::clear();
	gl::color(Color(1, 1, 1));

	// Save current global width and height - they will be changed
	// by receivetexture if the sender changes dimensions
	width = g_Width;
	height = g_Height;

	// Try to receive the texture at the current size 
	if (bInitialized) {

		if (!ReceiveTexture(SenderName, spoutTexture.getId(), spoutTexture.getTarget(), width, height)) {
			//
			// Receiver failure :
			//	1)	width and height are zero for read failure.
			//	2)	width and height are changed for sender change
			//		The local texture then has to be resized.
			//
			if (width == 0 || height == 0) {
				// width and height are returned zero if there has been 
				// a texture read failure which might happen if the sender
				// is closed. Spout will keep trying and if the same sender opens again
				// will use it. Otherwise the user can select another sender.
				return;
			}

			if (width != g_Width || height != g_Height) {
				// The sender dimensions have changed
				// Update the global width and height
				g_Width = width;
				g_Height = height;
				// Update the local texture to receive the new dimensions
				spoutTexture = gl::Texture(g_Width, g_Height);
				// reset render window
				setWindowSize(g_Width, g_Height);
				return; // quit for next round
			}
		}
		else
		{
			gl::draw(spoutTexture, getWindowBounds());
			if (mParameterBag->mPreviewEnabled)	mUserInterface->mPreviewPanel->setBackgroundTexture(spoutTexture);
		}
	}
	gl::disableAlphaBlending();
}
void ReymentaRenderApp::getWindowsResolution()
{
	mParameterBag->mDisplayCount = 0;
	// Display sizes
	mParameterBag->mMainDisplayWidth = Display::getMainDisplay()->getWidth();
	//mRenderMaxWidth = 0;
	mParameterBag->mRenderX = mParameterBag->mMainDisplayWidth;
	mParameterBag->mRenderY = 0;
	//mRenderMaxHeight = Display::getMainDisplay()->getHeight();
	for (auto display : Display::getDisplays())
	{
		mParameterBag->mDisplayCount++;
		mParameterBag->mRenderWidth = display->getWidth();
		//mRenderMaxWidth += mRenderWidth;
		mParameterBag->mRenderHeight = display->getHeight();
		// false: if ( mParameterBag->m16_9 ) mParameterBag->mRenderHeight = mParameterBag->mRenderWidth * 9 / 16;
		//if ( mRenderHeight > mRenderMaxHeight ) mRenderMaxHeight = mRenderHeight;
		log->logTimedString("Window " + toString(mParameterBag->mDisplayCount) + ": " + toString(mParameterBag->mRenderWidth) + "x" + toString(mParameterBag->mRenderHeight));
	}
	log->logTimedString("mRenderWindowsCount:" + toString(mParameterBag->mRenderWindowsCount) + " mRenderWidth" + toString(mParameterBag->mRenderWidth) + "mRenderHeight" + toString(mParameterBag->mRenderHeight));
	mParameterBag->mRenderResoXY = Vec2f(mParameterBag->mRenderWidth, mParameterBag->mRenderHeight);

	// in case only one screen , rebder from x = 0
	if (mParameterBag->mDisplayCount == 1) mParameterBag->mRenderX = 0;
}
void ReymentaRenderApp::windowManagement()
{
	log->logTimedString("windowManagement");
	getWindowsResolution();
	// setup the main window and associated draw function
	mMainWindow = getWindow();
	mMainWindow->setTitle("ReymentaRender");
	mMainWindow->connectDraw(&ReymentaRenderApp::drawMain, this);
	mMainWindow->connectClose(&ReymentaRenderApp::shutdown, this);

	//mWindowsManager = WindowsManager::create(false, true, "main", mParameterBag->mMainWindowWidth, mParameterBag->mMainWindowHeight, getWindow());//, this );
}
void ReymentaRenderApp::updateWindowTitle(string title)
{
	getWindow()->setTitle(" Reymenta - " + title);
}

void ReymentaRenderApp::mouseDown(MouseEvent event)
{
	if (event.isRightDown()) { // Select a sender
		SelectSenderDialog(); // TODO check if init // The Spout.dll sender selection dialog OK for Cinder
	}
	else
	{
		initSpout();
	}
}

// -------- SPOUT -------------
void ReymentaRenderApp::shutdown()
{
	log->logTimedString("shutdown");
	deleteRenderWindows();
	if(mParameterBag->mSpoutInitialized) ReleaseReceiver();// TODO  check if init
	quit();
}
void ReymentaRenderApp::addRenderWindow()
{
	log->logTimedString("addRenderWindow,mRenderWindowsCount:" + toString(mParameterBag->mRenderWindowsCount));
	mParameterBag->mRenderWindowsCount++;
	createRenderWindows();
}
void ReymentaRenderApp::createRenderWindow(int rCount)
{
	log->logTimedString("createRenderWindow,mRenderWindowsCount:" + toString(rCount));
	mParameterBag->mRenderWindowsCount = rCount;
	createRenderWindows();
}
void ReymentaRenderApp::createRenderWindows()
{
	log->logTimedString("createRenderWindows,mRenderWindowsCount:" + toString(mParameterBag->mRenderWindowsCount));

	log->logTimedString("BEFORE");
	log->logTimedString("iResolution:" + toString(mParameterBag->iResolution.x) + "x" + toString(mParameterBag->iResolution.y));
	// position
	log->logTimedString("position for each window(-2 to 2 factor):");
	log->logTimedString("mMiddleRenderXY:" + toString(mParameterBag->mRenderXY.x) + "x" + toString(mParameterBag->mRenderXY.y));
	// mouse
	log->logTimedString("iMouse for each window:");
	log->logTimedString("mMiddleRenderPosXY:" + toString(mParameterBag->mRenderPosXY.x) + "x" + toString(mParameterBag->mRenderPosXY.y));
	// iResolution
	log->logTimedString("iResolution for each window:");
	log->logTimedString("mMiddleRenderResoXY:" + toString(mParameterBag->mRenderResoXY.x) + "x" + toString(mParameterBag->mRenderResoXY.y));
	// renderwindow
	log->logTimedString("render window:");
	log->logTimedString("mRenderResolution:" + toString(mParameterBag->mRenderResolution.x) + "x" + toString(mParameterBag->mRenderResolution.y));

	deleteRenderWindows();
	getWindowsResolution();

	if (mParameterBag->mCustomRender)
	{
		log->logTimedString("CREATEWINDOW CustomRender true");
		mParameterBag->iResolution.x = mParameterBag->mSizeLeftW * 3 / mParameterBag->mRenderWindowsCount;//TODO other sizes
		mParameterBag->iResolution.y = mParameterBag->mSizeLeftH;
		mParameterBag->mRenderResolution = Vec2i(mParameterBag->mSizeLeftW * 3 / mParameterBag->mRenderWindowsCount, mParameterBag->mSizeLeftH);
	}
	else
	{
		log->logTimedString("CREATEWINDOW CustomRender false");
		mParameterBag->iResolution.x = mParameterBag->mRenderWidth / mParameterBag->mRenderWindowsCount;
		mParameterBag->iResolution.y = mParameterBag->mRenderHeight;
		mParameterBag->mRenderResolution = Vec2i(mParameterBag->mRenderWidth / mParameterBag->mRenderWindowsCount, mParameterBag->mRenderHeight);
	}
	log->logTimedString("resolution:" + toString(mParameterBag->iResolution.x) + "x" + toString(mParameterBag->iResolution.y));

	for (int i = 0; i < mParameterBag->mRenderWindowsCount; i++)
	{
		string windowName = "render";

		log->logTimedString("mRenderWindowsCount:" + toString(mParameterBag->mRenderWindowsCount) + " mRenderWidth" + toString(mParameterBag->mRenderWidth / mParameterBag->mRenderWindowsCount) + " mRenderHeight" + toString(mParameterBag->mRenderHeight));

		WindowRef   mRenderWindow;
		mRenderWindow = createWindow(Window::Format().size(mParameterBag->iResolution.x, mParameterBag->iResolution.y));

		// create instance of the window and store in vector
		WindowMngr rWin = WindowMngr(windowName, mParameterBag->mRenderWidth / mParameterBag->mRenderWindowsCount, mParameterBag->mRenderHeight, mRenderWindow);
		allRenderWindows.push_back(rWin);

		mRenderWindow->setBorderless();
		//mRenderWindow->setAlwaysOnTop();
		mParameterBag->mRenderResoXY = Vec2f(mParameterBag->mRenderWidth / mParameterBag->mRenderWindowsCount, mParameterBag->mRenderHeight);

		if (windowName == "render")
		{
			mRenderWindow->connectDraw(&ReymentaRenderApp::drawRender, this);
			//mRenderWindow->connectKeyDown(  &ReymentaApp::keyDownRender, this );
			if (i < 2) mParameterBag->mRenderPosXY = Vec2i(mParameterBag->mRenderX + (i * mParameterBag->mRenderWidth / mParameterBag->mRenderWindowsCount), 0);
		}
		if (mParameterBag->mCustomRender)
		{
			if (i == 0) mRenderWindow->setPos(mParameterBag->mWindowLeftX, mParameterBag->mWindowLeftY);
			if (i == 1) mRenderWindow->setPos(mParameterBag->mWindowMiddleX, mParameterBag->mWindowMiddleY);
			if (i == 2) mRenderWindow->setPos(mParameterBag->mWindowRightX, mParameterBag->mWindowRightY);
		}
		else
		{
			mRenderWindow->setPos(mParameterBag->mRenderX + (i * mParameterBag->mRenderWidth / mParameterBag->mRenderWindowsCount), 0);
		}
		//transparentWindow( (HWND)mRenderWindow->getNative() );
	}
	log->logTimedString("AFTER");
	log->logTimedString("iResolution:" + toString(mParameterBag->iResolution.x) + "x" + toString(mParameterBag->iResolution.y));
	// position
	log->logTimedString("position for each window(-2 to 2 factor):");
	log->logTimedString("mMiddleRenderXY:" + toString(mParameterBag->mRenderXY.x) + "x" + toString(mParameterBag->mRenderXY.y));
	// mouse
	log->logTimedString("iMouse for each window:");
	log->logTimedString("mMiddleRenderPosXY:" + toString(mParameterBag->mRenderPosXY.x) + "x" + toString(mParameterBag->mRenderPosXY.y));
	// iResolution
	log->logTimedString("iResolution for each window:");
	log->logTimedString("mMiddleRenderResoXY:" + toString(mParameterBag->mRenderResoXY.x) + "x" + toString(mParameterBag->mRenderResoXY.y));
	// renderwindow
	log->logTimedString("render window:");
	log->logTimedString("mRenderResolution:" + toString(mParameterBag->mRenderResolution.x) + "x" + toString(mParameterBag->mRenderResolution.y));

}
void ReymentaRenderApp::deleteRenderWindows()
{
	for (auto wRef : allRenderWindows) DestroyWindow((HWND)wRef.mWRef->getNative());
	allRenderWindows.clear();
}

// This line tells Cinder to actually create the application
CINDER_APP_NATIVE(ReymentaRenderApp, RendererGl)
