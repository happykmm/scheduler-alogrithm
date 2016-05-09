
/* ÎÄ¼þ×¢ÊÍ */
/*
 * @file xmlprocess.c processes the xml data including 
 *  functions: read xml data from xml and create new xml file. 
 * @created by jianhaichen
 * @modified by jianhaichen at 2013.5.13
 */

#pragma   once  
#ifndef _DRS_XMLPROCESS_H
#define _DRS_XMLPROCESS_H

#include "../lib/drsdata.h"
#include "../lib/drslib.h"


#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlreader.h> //for process large data


/**************************************************/
typedef struct{
	
	sharedataPtr sd;/*Point to the shared data*/

	/*Create new xml file for drs*/
	void (*xml_create_by_pms_vms)(char *xmlfilename);
	void (*xml_create_by_vm2vm)(char *xmlfilename);	
	void (*xml_create_by_pm2pm)(char *);	
	void (*xml_create_by_actions)(char *);
	void (*xml_create_by_pm2pm_treetopo)(char *xmlfilename);

	/*Read data from xml*/
	
	int (*get_request_from_xml1)(char *filename);
	int (*get_vmpm_from_xml2)(char *filename);
	int (*get_pm2pm_from_xml21)(char *filename);
	int (*get_vm2vm_from_xml22)(char *filename);
	
}cls_st_xmlprocess;

/*A pointer for class xmlprocess*/
typedef cls_st_xmlprocess * cls_xmlprocessPtr;

static cls_xmlprocessPtr _xpthis;

/**************************************************/
/* initialize class parameters*/
void init_xmlprocess(cls_xmlprocessPtr THIS);

/* declarations of functions for this class*/
/*------------------------------------------------*/
/*create xml functions declaration*/
static void xml_create_by_pms_vms(char *xmlfilename);
static void xml_create_by_vm2vm(char *xmlfilename);
static void xml_create_by_pm2pm(char *xmlfilename);
static void xml_create_by_actions(char *xmlfilename);
static void xml_create_by_pm2pm_treetopo(char *xmlfilename);
/*------------------------------------------------*/
static int get_request_from_xml1(char *filename);
static int get_vmpm_from_xml2(char *filename);
static int get_pm2pm_from_xml21(char *filename);
static int get_vm2vm_from_xml22(char *filename);

/**************************************************/


#endif

