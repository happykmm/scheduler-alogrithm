#ifndef _XMLWRAP_H
#define _XMLWRAP_H

#include <libxml/tree.h>
#include <libxml/parser.h>

#include <string>

xmlDocPtr readXmlDoc(const char *buf);
xmlDocPtr newXmlDoc(const char *rootTag, xmlNodePtr *root);
int writeXmlDoc(xmlDocPtr doc, const char *filename, const char *dir = NULL);
char *cGetNodeProp(xmlNodePtr node, const char *propName);
void freeXmlVars(int count, ...);
void freeXmlDocs(int count, ...);
xmlNodePtr newXmlNode(const char *name, int count, ...);
xmlNodePtr getFirstchildOfRoot(xmlDocPtr doc);
xmlNodePtr getFirstElementChild(xmlNodePtr cur);
xmlNodePtr prevElementSibling(xmlNodePtr cur);
xmlNodePtr nextElementSibling(xmlNodePtr cur);
void debugNode(xmlNodePtr node);

std::string getNodeProp(xmlNodePtr node, const char *propName, 
    bool required = false, std::string def = "");
int writeXmlDocToStr(xmlDocPtr doc, std::string &str);

#endif
