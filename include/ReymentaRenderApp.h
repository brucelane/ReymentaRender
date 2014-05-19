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

#include "cinder/app/AppNative.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Camera.h"
#include <vector>

// spout
#include "spout.h"
#include "cinder/Timeline.h"
// logger
#include "Logger.h"
// json
#include "JSONWrapper.h"
// parameters
#include "ParameterBag.h"
// textures
#include "Textures.h"
// shaders
#include "Shaders.h"
// UI
#include "UI.h"
// transparent window
#include "dwmapi.h"
// window manager
#include "WindowMngr.h"
// OSC
#include "OSCWrapper.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace Spout;
using namespace reymenta;
using namespace MinimalUI;

class ReymentaRenderApp : public AppNative {
public:	
	void prepareSettings( Settings *settings );
	void setup();
	void shutdown();
	void update();
	void mouseDown( MouseEvent event );	

	void updateWindowTitle( string title );
	void save();
private:
	// windows
	WindowRef					mMainWindow;
	void						windowManagement();
	void						getWindowsResolution();
	void						drawMain();
	// render
	void						createRenderWindow( int rCount );
	void						createRenderWindows();
	void						addRenderWindow();
	void						deleteRenderWindows();
	vector<WindowMngr>			allRenderWindows;
	void						drawRender();
	// spout
	void 						initSpout();

	// modes, should be the same as in ParameterBag
	static const int			MODE_NORMAL = 0;
	static const int			MODE_CIRCLE = 1;

	// windows to create, should be the same as in ParameterBag
	static const int			NONE = 0;
	static const int			RENDER_1 = 1;
	static const int			RENDER_2 = 2;
	static const int			RENDER_3 = 3;
	static const int			RENDER_MORE = 4;
	static const int			RENDER_DELETE = 5;
	static const int			RECEIVER = 6;

	// Logger
	LoggerRef					log;	
	// json
	JSONWrapperRef				mJson;	
	// Textures
	TexturesRef					mTextures;	
	// Shaders
	ShadersRef					mShaders;
	gl::GlslProg 				mShader;
	// minimalUI
	UIRef						mUserInterface;
	// parameters
	ParameterBagRef				mParameterBag;
	// osc
	OSCRef						mOSC;

	// timeline
	Anim<float>					mTimer;

    // spout
    bool            			bInitialized;       // true if a sender initializes OK
    bool            			bTextureShare;      // tells us if texture share compatible
    unsigned int    			g_Width, g_Height;  // size of the texture being sent out
    char            			SenderName[256];    // sender name 
    gl::Texture     			spoutTexture;       // Local Cinder texture used for sharing

};
