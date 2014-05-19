#include "JSONWrapper.h"

using namespace reymenta;

JSONWrapper::JSONWrapper() 
{
	// instanciate the logger class
	log = Logger::create( "JsonLog.txt" );
	log->logTimedString("JSONWrapper constructor");
}

JSONWrapper::~JSONWrapper() 
{
	log->logTimedString("JSONWrapper destructor");
}


string JSONWrapper::parseUIJson()
{
	// params
	stringstream params;
	params << "{ \"ok\":1";
	string pathToFile = ( getAssetPath("") / "json" / "ui.json" ).string();
	//dual screen string pathToFile = ( getAssetPath("") / "json" / "config.json" ).string();
	try 
	{
		if ( !fs::exists( pathToFile ) ) 
		{
			log->logTimedString( "file not found: " +  pathToFile ); 
			return "{ \"ok\":0 }";
		}
		else
		{
			log->logTimedString( "found file: " +  pathToFile ); 
		}
		JsonTree json;
		try
		{
			json = JsonTree( loadFile( pathToFile ) );
		} 
		catch(cinder::JsonTree::Exception exception) 
		{ 
			log->logTimedString("jsonparser exception " + pathToFile +": " + exception.what());
			return  "{ \"ok\":0 }";
		}
		JsonTree jsonParams = json.getChild( "params" );   
		for( JsonTree::ConstIter jsonElement = jsonParams.begin(); jsonElement != jsonParams.end(); ++jsonElement ) 
		{			
			string name = jsonElement->getChild( "name" ).getValue<string>();
			string value = jsonElement->getChild( "value" ).getValue<string>();
			params << ", \"";
			params << name;
			params << "\":\"";
			params << value;
			params << "\"";
		} 
	} 
	catch ( ... ) 
	{
		log->logTimedString(  "Fragjson parsing error: " +  pathToFile );
		return  "{ \"ok\":0 }";
	}
	params << "}";

	return params.str();
}

/*bool JSONWrapper::parseConfigJson()
{

	string pathToFile = ( getAssetPath("") / "json" / "config.json" ).string();
	try 
	{
		if ( !fs::exists( pathToFile ) ) 
		{
			log->logTimedString( "file not found: " +  pathToFile ); 
			return false;
		}
		else
		{
			log->logTimedString( "found file: " +  pathToFile ); 
		}
		JsonTree json;
		try
		{
			json = JsonTree( loadFile( pathToFile ) );
		} 
		catch(cinder::JsonTree::Exception exception) 
		{ 
			log->logTimedString("jsonparser exception " + pathToFile +": " + exception.what());
			return false;
		}

		//// frag
		//JsonTree jsonfrags = json.getChild( "frag" );   
		//for( JsonTree::ConstIter jsonElement = jsonfrags.begin(); jsonElement != jsonfrags.end(); ++jsonElement ) 
		//{			
		//	mShaders->setFragBegin( jsonElement->getChild( "begin" ).getValue<string>() );
		//	mShaders->setFragGlobalFunctions( jsonElement->getChild( "globalfunctions" ).getValue<string>() );
		//	mShaders->setFragMainHeader( jsonElement->getChild( "mainheader" ).getValue<string>() );
		//	mShaders->setFragEnd( jsonElement->getChild( "end" ).getValue<string>() );
		//} 

	} 
	catch ( ... ) 
	{
		log->logTimedString(  "Parsing error: " +  pathToFile );
		return false;
	}
	return true;
}*/
bool JSONWrapper::parseAssetsJson( TexturesRef aTexturesRef )
{
	mTextures = aTexturesRef;

	string pathToFile = ( getAssetPath("") / "json" / "assets.json" ).string();
	try 
	{
		if ( !fs::exists( pathToFile ) ) 
		{
			log->logTimedString( "file not found: " +  pathToFile ); 
			return false;
		}
		else
		{
			log->logTimedString( "found file: " +  pathToFile ); 
		}
		JsonTree json;
		try
		{
			json = JsonTree( loadFile( pathToFile ) );
		} 
		catch(cinder::JsonTree::Exception exception) 
		{ 
			log->logTimedString("jsonparser exception " + pathToFile +": " + exception.what());
			return false;
		}

		//Assets
		int i = 1; // 0 is audio
		JsonTree jsons = json.getChild( "assets" );   
		for( JsonTree::ConstIter jsonElement = jsons.begin(); jsonElement != jsons.end(); ++jsonElement ) 
		{			
			if ( i < mTextures->getTextureCount() )
			{
				string jsonFileName = jsonElement->getChild( "filename" ).getValue<string>();
				log->logTimedString("asset filename: " + jsonFileName );
				mTextures->setTexture( i, jsonFileName );

				i++;
			}

		} 
	} 
	catch ( ... ) 
	{
		log->logTimedString(  "Parsing error: " +  pathToFile );
		return false;
	}
	return true;
}
/*string JSONWrapper::getJsonStringValue( string fileName, string element )
{
	try 
	{
		if ( !fs::exists( fileName ) ) 
		{
			log->logTimedString( "fragjson file not found: " +  fileName ); 
			return false;
		}
		else
		{
			log->logTimedString( "fragjson found file: " +  fileName ); 
		}
		JsonTree json;
		try
		{
			json = JsonTree( loadFile( fileName ) );
		} 
		catch(cinder::JsonTree::Exception exception) 
		{ 
			log->logTimedString("fragjsonparser exception " + fileName +": " + exception.what());
			return false;
		}

		// frag
		return json.getChild( element ).getValue<string>();   
		//// frag
		//JsonTree jsonfrags = json.getChild( "frag" );   
		//for( JsonTree::ConstIter jsonElement = jsonfrags.begin(); jsonElement != jsonfrags.end(); ++jsonElement ) 
		//{			
		//	mShaders->setFragBegin( jsonElement->getChild( "begin" ).getValue<string>() );
		//	mShaders->setFragGlobalFunctions( jsonElement->getChild( "globalfunctions" ).getValue<string>() );
		//	mShaders->setFragMainHeader( jsonElement->getChild( "mainheader" ).getValue<string>() );
		//	mShaders->setFragEnd( jsonElement->getChild( "end" ).getValue<string>() );
		//} 
	
			//for( JsonTree::ConstIter jsonElement = jsonfrags.begin(); jsonElement != jsonfrags.end(); ++jsonElement ) 
			//{			
				//mShaders->setFragFunctions( jsonElement->getChild( "fragfunctions" ).getValue<string>() );
				//mShaders->setFragMainLines( jsonElement->getChild( "fragmain" ).getValue<string>() );
			//} 
	} 
	catch ( ... ) 
	{
		log->logTimedString(  "Fragjson parsing error: " +  fileName );
		return false;
	}
	return true;
}*/