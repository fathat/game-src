// I N C L U D E S ////////////////////////////////////////////
#pragma once
#include "..\tinyxml\tinyxml.h"
#include <string>

// F U N C T I O N S /////////////////////////////////////////
TiXmlElement* xmlgetnode( TiXmlNode	* node, const char* name );

const char* xmlgetvalue( TiXmlNode* node, const char* name );

const char* xmlreads( TiXmlElement* node, const char* name );
bool	    xmlreadb ( TiXmlElement* node, const char* name );
int			xmlreadi( TiXmlElement* node, const char* name );
double		xmlreadf( TiXmlElement* node, const char* name );

bool xmlnodeexists( TiXmlElement* node, const char* name );

double xmlreadattributef( TiXmlElement* parentnode, const char* element, const char* attribute );
int xmlreadattributei( TiXmlElement* parentnode, const char* element, const char* attribute );
std::string xmlreadattributes( TiXmlElement* parentnode, const char* element, const char* attribute );