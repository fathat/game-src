#include "xml.h"

using namespace std;

TiXmlElement* xmlgetnode( TiXmlNode	* node, const char* name )
{
	return node->FirstChildElement( name );
}

const char* xmlgetvalue( TiXmlNode* node, const char* name )
{
	//TiXmlNode* spacingNode = spacing->FirstChild();
	//TiXmlText* text = spacingNode->ToText();
	//t = (char*)text->Value();

	TiXmlNode* e = xmlgetnode( node, name )->FirstChild();
	if( e )
	{
		TiXmlText* t = e->ToText();
		return t->Value();
	}
	return NULL;
}

const char* xmlreads( TiXmlElement* node, const char* name )
{
	return xmlgetvalue(node, name ) ;
}

bool xmlreadb ( TiXmlElement* node, const char* name )
{
	if( stricmp( xmlgetvalue( node, name ), "true" ) == 0 )
		return true;

	return false;

}

int xmlreadi( TiXmlElement* node, const char* name )
{
	return atoi( xmlgetvalue( node, name ));
}

double xmlreadf( TiXmlElement* node, const char* name )
{
	return atof( xmlgetvalue( node, name ) );
}

bool xmlnodeexists( TiXmlElement* node, const char* name )
{
	if(  node->FirstChildElement( name ) )
		return true;
	return false;

}

double xmlreadattributef( TiXmlElement* parentnode, const char* element, const char* attribute )
{
	double d=0;
	TiXmlElement* xmlelement = parentnode->FirstChildElement( element );
	if ( xmlelement )
	{
		xmlelement->Attribute(attribute, &d ); 
	}
	return d;
}

int xmlreadattributei( TiXmlElement* parentnode, const char* element, const char* attribute )
{
	int d=0;
	TiXmlElement* xmlelement = parentnode->FirstChildElement( element );
	if ( xmlelement )
	{
		xmlelement->Attribute(attribute, &d ); 
	}
	return d;
}

std::string xmlreadattributes( TiXmlElement* parentnode, const char* element, const char* attribute )
{
	string returnstring;
	TiXmlElement* xmlelement = parentnode->FirstChildElement( element );
	if ( xmlelement )
	{

		returnstring = xmlelement->Attribute(attribute ); 

	}

	return returnstring;
}