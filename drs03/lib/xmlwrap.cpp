/*
 * @file xmlwrap.c wrapper functions for libxml2
 * @created by tuantuan <dangoakachan@foxmail.com>
 * @modified by tuantuan at 2011.7.19
 */
#include "xmlwrap.h"
//#include "util.h"
//#include "drs_def.h"

#include <stdarg.h>
#include <string.h>

/*
 * @fn xmlDocPtr readXmlDoc(const char *buf, int from)
 * Parse a XML file from filesystem or a XML in-memory document and build a tree
 * @param buf the XML filename or the buffer that stores a XML document
 * @return a pointer to the result xml document, or NULL if failed
 * @created by tuantuan at 2011.7.01
 * @modified by tuantuan at 2011.8.26
 */
xmlDocPtr readXmlDoc(const char *buf)
{
    // The resulting document tree
    xmlDocPtr doc = NULL;

    // Check if buf is null or empty
    if (isNullOrEmpty(buf)) {
        alertMsg("Parameter buf can't be null or empty in %s.\n", __FUNCTION__);
        return doc;
    }

    if (buf[0] == '<') // Buf is a XML in-memory document
        doc = xmlReadMemory(buf, xmlStrlen(BAD_CAST buf), "noname.xml", 
            "UTF-8", XML_PARSE_RECOVER | XML_PARSE_NOBLANKS);
    else // Buf is XML filename
        doc = xmlReadFile(buf, "UTF-8", XML_PARSE_RECOVER | XML_PARSE_NOBLANKS);

    return doc;
}

/*
 * @fn writeXmlDoc(xmlDocPtr doc, const char *filename, const char *dir)
 * Dump an XML document to a file or stdout
 * @param doc the pointer to XML document being saved
 * @param filename the filename or stdout (filename = "-")
 * @param dir the output directory
 * @return the number of bytes written or -1 in case of error. 
 * @created by tuantuan at 2011.7.01
 */
int writeXmlDoc(xmlDocPtr doc, const char *filename, const char *dir)
{
    std::string path;

    if (filename == NULL || strcmp(filename, "-") == 0)
        path = "-";
    else if (dir == NULL)
        path = mkPath(getOutputDir(), filename);
    else
        path = mkPath(dir, filename);

    return xmlSaveFormatFileEnc(path.c_str(), doc, "UTF-8", 1);
}

/* 
 * @fn int writeXmlDocToStr(xmlDocPtr doc, std::string &str)
 * Write/dump xml document to string
 * @param doc pointer to xml document
 * @param str string that receive the xml document in string
 * @return string length of xml document
 * @created by tuantuan at 2011.8.29
 */
int writeXmlDocToStr(xmlDocPtr doc, std::string &str)
{
    /* Check if doc is null */
    if (doc == NULL)
        return 0;

    int siz = 0;                // Buffer size
    xmlChar *buf = NULL;        // Buffer to store xml document

    /* Dump xml document to buffer */
    xmlDocDumpFormatMemoryEnc(doc, &buf, &siz, "UTF-8", 1);
    str = (char *)buf;

    /* Free the buffer memory */
    if (buf != NULL)
        xmlFree(buf);

    return siz;
}

/*
 * @fn xmlNodePtr nextElementSibling(xmlNodePtr cur)
 * Get the next element sibling node of current one
 * @param cur current node
 * @return the next element sibling node if find, otherwise return NULL
 * @created by tuantuan at 2011.8.26
 */
xmlNodePtr nextElementSibling(xmlNodePtr cur)
{
    /* If cur is null, do nothing */
    if (cur == NULL)
        return NULL;

    /* Get next node of current one */
    xmlNodePtr nextNode = cur->next;

    /* Iterate next node to get the first element sibling node */
    while ((nextNode != NULL) && (nextNode->type != XML_ELEMENT_NODE))
        nextNode = nextNode->next;

    return nextNode;
}

/*
 * @fn xmlNodePtr prevElementSibling(xmlNodePtr cur)
 * Get the previous element sibling node of current one
 * @param cur current node
 * @return the previous element sibling node if find, otherwise return NULL
 * @created by tuantuan at 2011.8.26
 */
xmlNodePtr prevElementSibling(xmlNodePtr cur)
{
    /* If cur is null, do nothing */
    if (cur == NULL)
        return NULL;

    /* Get previous node of current one */
    xmlNodePtr prevNode = cur->prev;

    /* Iterate previous node to get the first element sibling node */
    while ((prevNode != NULL) && (prevNode->type != XML_ELEMENT_NODE))
        prevNode = prevNode->prev;

    return prevNode;
}

/*
 * @fn xmlNodePtr getFirstElementChild(xmlNodePtr cur)
 * Get the first element child of current one
 * @param cur current node
 * @return the first element child if find, otherwise return NULL
 * @created by tuantuan at 2011.8.26
 */
xmlNodePtr getFirstElementChild(xmlNodePtr cur)
{
    /* If cur is null, do nothing */
    if (cur == NULL)
        return NULL;

    xmlNodePtr child = cur->xmlChildrenNode;

    if ((child == NULL) || (child->type == XML_ELEMENT_NODE))
        return child;
    else
        return nextElementSibling(child);
}

/*
 * @fn xmlNodePtr getFirstchildOfRoot(xmlDocPtr doc)
 * Get the first child of root element in the XML document
 * @param doc the pointer to XML document
 * @return the first child of root element
 * @created by tuantuan at 2011.7.01
 */
xmlNodePtr getFirstchildOfRoot(xmlDocPtr doc)
{
    /* Make sure doc is not a null pointer */
    if (doc == NULL)
        return NULL;
    else
        return getFirstElementChild(xmlDocGetRootElement(doc));
}

/*
 * @fn freeXmlVars(int count, ...)
 * Wrap xmlFree functions to free more than one allocated memory at one time.
 * @param count count of arguments
 * @param ... variable arguments
 * @created by tuantuan at 2011.7.01
 */
void freeXmlVars(int count, ...)
{
    va_list ap;
    int i = 0;
    char *argv;

    va_start(ap, count);

    while (i++ < count) {
        argv = va_arg(ap, char *);

        if (!isNullOrEmpty(argv))
            xmlFree((xmlChar *)argv);
    }

    va_end(ap);
}

/*
 * @fn freeXmlDocs(int count, ...)
 * Wrap xmlFreeDoc functions to free more than one xmlDoc at one time.
 * @param count count of arguments
 * @param ... variable arguments
 * @created by tuantuan at 2011.7.01
 */
void freeXmlDocs(int count, ...)
{
    va_list ap;
    int i = 0;
    xmlDocPtr argv;

    va_start(ap, count);

    while (i++ < count) {
        argv = va_arg(ap, xmlDocPtr);

        if (argv != NULL)
            xmlFreeDoc(argv);
    }

    va_end(ap);
}

/*
 * @fn xmlDocPtr newXmlDoc(const char *rootTag, xmlNodePtr *root)
 * Create a new XML document and set its root element's name as @rootTag
 * @param rootTag the name of root element
 * @param root return the root element of new created XML document
 * @return a pointer to the new created XML document
 * @created by tuantuan at 2011.7.01
 */
xmlDocPtr newXmlDoc(const char *rootTag, xmlNodePtr *root)
{
    /* If no root tag name is provided */
    if (isNullOrEmpty(rootTag))
        return NULL;

    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");

    *root = newXmlNode(rootTag, 1, "VERSION", VERSION);
    xmlDocSetRootElement(doc, *root);

    return doc;
}

/*
 * @fn xmlNodePtr newXmlNode(const char *name, int count, ...)
 * @created by tuantuan at 2011.7.01
 */
xmlNodePtr newXmlNode(const char *name, int count, ...)
{
    /* If no node name is provided */
    if (isNullOrEmpty(name))
        return NULL;

    va_list ap;
    const char *attr, *val;
    int i = 0;

    xmlNodePtr node;

    va_start(ap, count);

    // Create a new node
    node = xmlNewNode(0, BAD_CAST name);

    // Get attr and val from the variable arguments
    while (i++ < count) {
        attr = va_arg(ap, char *);
        val = va_arg(ap, char *);

        // Create a new attribute
        if (!isNullOrEmpty(attr) && !isNullOrEmpty(val))
            xmlNewProp(node, BAD_CAST attr, BAD_CAST val);
    }

    va_end(ap);

    return node;
}

/*
 * @fn std::string *getNodeProp(xmlNodePtr node, const char *propName, bool required,
 *     std::string def)
 * Get the property value of the specified node
 * @param node pointer to xml node
 * @param propName property name
 * @param required specify if a property is required
 * @param def default value, empty string by default
 * @return property value if found, otherwise return an empty string
 * @created by tuantuan at 2011.7.01
 */
std::string getNodeProp(xmlNodePtr node, const char *propName, bool required,
    std::string def)
{
    if (node == NULL)
        return def;

    char *val = (char *)xmlGetProp(node, BAD_CAST propName);

    /* If the property is provided (even is a empty string) */
    if (val != NULL) {
        def = val;
        xmlFree(val);
    }

    /* If the property is required (empty string is not valid) */
    if (required && def.empty())
        errorMsg("%s is a required property in node %s.\n", propName, node->name);

    return def;
}

/*
 * @fn void debugNode(xmlNodePtr node);
 * Debug node information
 * @param node node to be debugged
 * @created by tuantuan at 2011.8.26
 */
void debugNode(xmlNodePtr node)
{
    if (node == NULL)
        debugMsg("The node to be debugged is null.\n");
    else
        debugMsg("The node %s's type is %d(1: element, 3: text).\n",
            node->name, node->type);

    return;
}

/*
 * @fn char *cGetNodeProp(xmlNodePtr node, char *propName)
 * Get the property value of the specified node, c compatiable
 * @param node pointer to xml node
 * @return propName property name
 * @created by tuantuan at 2011.7.01
 */
char *cGetNodeProp(xmlNodePtr node, const char *propName)
{
    if (node == NULL)
        return NULL;

    xmlChar *name = BAD_CAST propName;

    return (char *)xmlGetProp(node, name);
}
