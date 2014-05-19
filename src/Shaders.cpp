#include "Shaders.h"

using namespace reymenta;

Shaders::Shaders(ParameterBagRef aParameterBag)
{
	mParameterBag = aParameterBag;
	// instanciate the logger class
	log = Logger::create("ShadersLog.txt");
	log->logTimedString("Shaders constructor");
	mFragBegin = getFragStringFromFile("begin.fragpart");
	mFragFunctionsLeft = getFragStringFromFile("leftfunctions.fragpart");
	mFragMainHeaderLeft = getFragStringFromFile("leftmainheader.fragpart");
	mFragMainLinesLeft = getFragStringFromFile("leftmainlines.fragpart");
	mFragFunctionsRight = getFragStringFromFile("rightfunctions.fragpart");
	mFragMainHeaderRight = getFragStringFromFile("rightmainheader.fragpart");
	mFragMainLinesRight = getFragStringFromFile("rightmainlines.fragpart");
	mFragGlobalFunctions = getFragStringFromFile("globalfunctions.fragpart");
	mFragBlendFunctions = getFragStringFromFile("blendfunctions.fragpart");
	mFragEnd = getFragStringFromFile("end.fragpart");

	fileName = "default.frag";
	fs::path localFile = getAssetPath("") / "shaders" / fileName;
	ofstream mDefaultFrag(localFile.string());
	mDefaultFrag << buildFragString();
	mDefaultFrag.close();
	// init if something goes wrong	
	mShaderLeft = gl::GlslProg(loadResource(PASSTHROUGH2_VERT), loadResource(PASSTHROUGH_FRAG));
	mShaderRight = gl::GlslProg(loadResource(PASSTHROUGH2_VERT), loadResource(PASSTHROUGH_FRAG));
	mShader = gl::GlslProg(loadResource(PASSTHROUGH2_VERT), loadResource(PASSTHROUGH_FRAG));

	//mPassthruPixelFrag = "uniform vec3 iResolution;uniform sampler2D iChannel0;void main(){vec2 uv = gl_TexCoord[0].st* vec2(iResolution.x,iResolution.y);gl_FragColor = texture2D(iChannel0, uv);}";
	validMiddleFrag = false;
	validLeftFrag = false;
	validRightFrag = false;
	validVert = true;

	mLeftFragFileName = mRightFragFileName = mFragFileName = fileName;

	mLeftFragFile = mRightFragFile = mFragFile = localFile.string();
	loadPixelFrag(localFile.string(), 1);
	loadPixelFrag(localFile.string(), 0);
	loadPixelFrag(localFile.string(), 2);
}
string Shaders::buildFragString()
{
	string s;
	if (mParameterBag->mUseBlendmodes)
	{
		s = mFragBegin + mFragFunctionsLeft + mFragMainHeaderLeft + mFragMainLinesLeft + mFragFunctionsRight + mFragMainHeaderRight + mFragMainLinesRight + mFragBlendFunctions + mFragEnd;
	}
	else
	{
		s = mFragBegin + mFragFunctionsLeft + mFragMainHeaderLeft + mFragMainLinesLeft + mFragFunctionsRight + mFragMainHeaderRight + mFragMainLinesRight + mFragGlobalFunctions + mFragEnd;
	}
	return s;
}

string Shaders::getFragStringFromFile(string fileName)
{
	string rtn = "";
	try
	{
		rtn = loadString(loadAsset("shaders/templates/" + fileName));
	}
	catch (const std::exception &e)
	{
		log->logTimedString(fileName + " unable to load string from file:" + string(e.what()));
	}
	return rtn;
}

string Shaders::getFragError()
{
	return mError;
}
Shaders::~Shaders()
{
	log->logTimedString("Shaders destructor");
}
string Shaders::getFileName(string aFilePath)
{
	string fName;
	if (aFilePath.find_last_of("\\") != std::string::npos)
	{
		fName = aFilePath.substr(aFilePath.find_last_of("\\") + 1);
	}
	else
	{
		fName = aFilePath;
	}
	return fName;
}
string Shaders::getNewFragFileName(string aFilePath)
{
	string fName;
	if (aFilePath.find_last_of("\\") != std::string::npos)
	{
		fName = aFilePath.substr(aFilePath.find_last_of("\\") + 1);
	}
	else
	{
		fName = aFilePath;
	}
	return fName + ".frag";
}
void Shaders::loadFragJson(string aFilePath)
{
	try
	{
		JsonTree json;
		if (!fs::exists(aFilePath))
		{
			log->logTimedString("file not found: " + aFilePath);

		}
		else
		{
			log->logTimedString("found file: " + aFilePath);
		}
		try
		{
			json = JsonTree(loadFile(aFilePath));
			mParameterBag->mCurrentFilePath = aFilePath;
		}
		catch (cinder::JsonTree::Exception exception)
		{
			log->logTimedString("fragjsonparser exception " + aFilePath + ": " + exception.what());

		}
		// frag
		try
		{
			if (json.hasChild("frag"))
			{
				JsonTree jsonfrags = json.getChild("frag");
				for (JsonTree::ConstIter jsonElement = jsonfrags.begin(); jsonElement != jsonfrags.end(); ++jsonElement)
				{
					if (mParameterBag->iCrossfade < 0.5)
					{
						//Right
						//mParameterBag->iPreview = 1;
						setFragFunctions(jsonElement->getChild("fragfunctions").getValue<string>(), 1);
						setFragMainLines(jsonElement->getChild("fragmain").getValue<string>(), 1);
					}
					else
					{
						//Left
						//mParameterBag->iPreview = 0;
						setFragFunctions(jsonElement->getChild("fragfunctions").getValue<string>(), 0);
						setFragMainLines(jsonElement->getChild("fragmain").getValue<string>(), 0);
					}
				}
			}
		}
		catch (cinder::JsonTree::Exception exception)
		{
			log->logTimedString("fragjsonparser exception parser " + aFilePath + ": " + exception.what());

		}
		//params

		try
		{
			if (json.hasChild("params"))
			{
				JsonTree jsonParams = json.getChild("params");
				for (JsonTree::ConstIter jsonElement = jsonParams.begin(); jsonElement != jsonParams.end(); ++jsonElement)
				{
					int id = jsonElement->getChild("paramid").getValue<int>();
					string name = jsonElement->getChild("name").getValue<string>();
					//TODO colors of the controls: control[id]=
					switch (id)
					{
					case ParameterBag::ISPEED://1
						mParameterBag->mSpeed = jsonElement->getChild("value").getValue<float>();
						//(mParameterBag->iCrossfade < 0.5) ? sliderCrossfade->setBackgroundColor(mParameterBag->ColorYellow) : sliderCrossfade->setBackgroundColor(mParameterBag->ColorPurple);

						break;
					case ParameterBag::IZOOM://2
						mParameterBag->iZoom = jsonElement->getChild("value").getValue<float>();
						break;
					case ParameterBag::IRATIO://3
						mParameterBag->iRatio = jsonElement->getChild("value").getValue<float>();
						break;
					case ParameterBag::ISTEPS://4
						mParameterBag->iSteps = jsonElement->getChild("value").getValue<float>();
						break;
					case ParameterBag::IPIXELATE://5
						mParameterBag->iPixelate = jsonElement->getChild("value").getValue<float>();
						break;
					case ParameterBag::ITOGGLE://6
						mParameterBag->iToggle = jsonElement->getChild("value").getValue<float>();
						break;
					default:
						break;
					}
				}
			}
		}
		catch (cinder::JsonTree::Exception exception)
		{
			log->logTimedString("fragjsonparser exception parser " + aFilePath + ": " + exception.what());

		}

	}
	catch (...)
	{
		log->logTimedString("Fragjson parsing error: " + aFilePath);

	}

}
bool Shaders::loadPixelFrag(string aFilePath, int screenPosition)
{
	bool rtn = false;
	// reset 
	mParameterBag->iFade = false;
	mParameterBag->iZoom = 1.0f;
	try
	{
		fs::path fr = aFilePath;
		if (fs::exists(fr))
		{
			switch (screenPosition)
			{
			case 0:
				validLeftFrag = false;
				mShaderLeft = gl::GlslProg(loadResource(PASSTHROUGH2_VERT), loadFile(aFilePath));
				mLeftFragFile = aFilePath;
				mLeftFragFileName = getFileName(aFilePath);
				validLeftFrag = true;
				rtn = true;
				mError = "";
				break;
			case 2:
				validRightFrag = false;
				mShaderRight = gl::GlslProg(loadResource(PASSTHROUGH2_VERT), loadFile(aFilePath));
				mRightFragFile = aFilePath;
				mRightFragFileName = getFileName(aFilePath);
				validRightFrag = true;
				rtn = true;
				mError = "";
				break;
			default:
				validMiddleFrag = false;
				mShader = gl::GlslProg(loadResource(PASSTHROUGH2_VERT), loadFile(aFilePath));
				mFragFile = aFilePath;
				mFragFileName = getFileName(aFilePath);
				validMiddleFrag = true;
				rtn = true;
				mError = "";
				break;
			}
			log->logTimedString("Loaded and compiled: " + aFilePath);
		}
		else
		{
			//aShader = gl::GlslProg( loadFile( mVertFile ), loadFile( mFragFile ) );
			log->logTimedString(mFragFile + " loaded and compiled, does not exist:" + aFilePath);
		}
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		mError = string(exc.what());
		log->logTimedString(aFilePath + " unable to load/compile shader:" + string(exc.what()));
	}
	catch (const std::exception &e)
	{
		mError = string(e.what());
		log->logTimedString(aFilePath + " unable to load shader:" + string(e.what()));
	}
	// if failure, load default passthru frag string
	/*if ( !rtn )
	{
	try
	{
	switch ( screenPosition )
	{
	case 0:
	if ( !validLeftFrag )
	{
	mShaderLeft = gl::GlslProg( loadResource( PASSTHROUGH2_VERT ), loadResource( PASSTHROUGH_FRAG ) );
	log->logTimedString("loadPixelFrag left PASSTHROUGH success");
	validRightFrag = true;
	}

	break;
	case 2:
	if ( !validRightFrag )
	{
	mShaderRight = gl::GlslProg( loadResource( PASSTHROUGH2_VERT ), loadResource( PASSTHROUGH_FRAG ) );
	log->logTimedString("loadPixelFrag right PASSTHROUGH success");
	validRightFrag = true;
	}
	break;
	default:
	if ( !validMiddleFrag )
	{
	mShader = gl::GlslProg( loadResource( PASSTHROUGH2_VERT ), loadResource( PASSTHROUGH_FRAG ) );
	log->logTimedString("loadPixelFrag middle PASSTHROUGH success");
	validMiddleFrag = true;
	}
	break;
	}
	}
	catch( gl::GlslProgCompileExc &exc )
	{
	mError = string( exc.what() );
	log->logTimedString( "loadPixelFrag PASSTHROUGH error:" + string( exc.what() ) );
	}
	catch( const std::exception &e )
	{
	log->logTimedString( "loadPixelFrag PASSTHROUGH error:" + string( e.what() ) );
	}
	}*/
	return rtn;
}
void Shaders::setFragFunctions(string aFragString, int leftOrRight)
{
	if (leftOrRight == 0)
	{
		//left
		mFragFunctionsLeft = aFragString;
	}
	else
	{
		//right
		mFragFunctionsRight = aFragString;
	}
}
void Shaders::setFragMainLines(string aFragString, int leftOrRight)
{
	if (leftOrRight == 0)
	{
		//left
		mFragMainLinesLeft = aFragString;
	}
	else
	{
		//right
		mFragMainLinesRight = aFragString;
	}
	setFragString(buildFragString());
}

void Shaders::loadCurrentFrag()
{
	mParameterBag->iAlpha = 0.0;
	try
	{
		string dbg = currentFrag.c_str();
		mShader = gl::GlslProg(NULL, currentFrag.c_str());
		log->logTimedString("loadCurrentFrag success");
		mError = "";
		validMiddleFrag = true;
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		validMiddleFrag = false;
		mError = string(exc.what());
		log->logTimedString("loadCurrentFrag error: " + mError);
	}
	// reset to no transition
	//mParameterBag->iTransition = 0;
	// avoid looping or transition to run
	//mParameterBag->iAnim = 1.0;
	if (mParameterBag->iCrossfade < 0.5)
	{
		//Right
		mParameterBag->iCrossfade = 1.0;
	}
	else
	{
		//Left
		mParameterBag->iCrossfade = 0.0;
		
	}
	timeline().apply(&mParameterBag->iAnim, 1.0f, 0.1f, EaseOutCubic()).finishFn([&]{
		mParameterBag->iAlpha = 1.0;
	});
	
}

void Shaders::doTransition()
{
	if (mParameterBag->iTransition > 0)
	{
		mParameterBag->iAnim = 128.0;
		timeline().apply(&mParameterBag->iAnim, 1.0f, mParameterBag->mTransitionDuration, EaseOutCubic()).startFn([&]{ mParameterBag->iSteps = 16.0; }).finishFn([&]{
			loadCurrentFrag();
		});
	}
}

bool Shaders::setFragString(string pixelFrag)
{
	currentFrag = pixelFrag;
	try
	{
		if (mParameterBag->iTransition > 0)
		{
			doTransition();
		}
		else
		{
			mShader = gl::GlslProg(NULL, currentFrag.c_str());
			log->logTimedString("setFragString success");
			mError = "";
			validMiddleFrag = true;
		}
		// if codeeditor mCodeEditor->setValue( pixelFrag );// CHECK 
		// save current mix
		fs::path localFile = getAssetPath("") / "shaders" / "generated" / getNewFragFileName(mParameterBag->mCurrentFilePath);
		ofstream mMixFrag(localFile.string());
		mMixFrag << pixelFrag;
		mMixFrag.close();
		log->logTimedString("current mix saved:" + localFile.string());
		// save as current.frag for code editor
		/* not refreshed fs::path currentFile = getAssetPath("") / "shaders" / "current.frag";
		ofstream mCurrentFrag(currentFile.string());
		mCurrentFrag << pixelFrag;
		mCurrentFrag.close();
		log->logTimedString("current live editor mix saved:" + currentFile.string());*/

	}
	catch (gl::GlslProgCompileExc &exc)
	{
		validMiddleFrag = false;
		mError = string(exc.what());
		log->logTimedString("setFragString error: " + mError);
		/*fs::path errorFile = getAssetPath("") / "shaders"  / "error" / getNewFragFileName( mParameterBag->mCurrentFilePath );
		ofstream mErrorFrag( errorFile.string() );
		mErrorFrag << pixelFrag;
		mErrorFrag.close();
		log->logTimedString("error saved:" + errorFile.string() );*/
	}

	return validMiddleFrag;
}

bool Shaders::loadTextFile(string aFilePath)
{
	bool success = false;
	string rtn = "";
	try
	{
		fs::path txtFile = aFilePath;
		if (fs::exists(txtFile))
		{
			DataSourceRef dsr = loadFile(txtFile);
			rtn = loadString(loadFile(txtFile));
			log->logTimedString(aFilePath + " content:" + rtn);
		}
		success = true;
	}
	catch (const std::exception &e)
	{
		log->logTimedString(fileName + " unable to load string from text file:" + string(e.what()));
	}
	return success;
	/* TODO
	// parse json
	if ( parseFragJson( mFile ) )
	{
	if ( mUserInterface->mCodeEditor )
	{
	string s = fragBegin + fragGlobalFunctions + fragFunctions + fragMainHeader + fragMainLines + fragEnd;
	mUserInterface->mCodeEditor->setValue( s );
	mUserInterface->mCodeEditor->write( mFile + "-" + ci::toString( (int)getAverageFps() ) + ".frag" );
	}

	// left
	loadPixelFrag( mParameterBag->mFragFile, mParameterBag->mShaderLeft, 0);
	// right
	loadPixelFrag( mParameterBag->mFragFile, mParameterBag->mShaderRight, 2);
	}
	*/
}