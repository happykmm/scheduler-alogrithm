
/* �ļ�ע�� */
/*
 * @file ganglia.c parse ganglia xml
 * @created by tuantuan <dangoakachan@foxmail.com>
 * @modified by tuantuan at 2011.7.19
 */


#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xmlreader.h> //for process large data
#include <libxml/xmlstring.h>

#include "xmlprocess.h"

//Initialize the struct functions
void init_xmlprocess(cls_xmlprocessPtr THIS)
{		
	_xpthis=THIS;

	/*Functions for creating xml files*/
	THIS->xml_create_by_pms_vms=xml_create_by_pms_vms;
	THIS->xml_create_by_vm2vm=xml_create_by_vm2vm;
	THIS->xml_create_by_actions=xml_create_by_actions;
	THIS->xml_create_by_pm2pm=xml_create_by_pm2pm;
	THIS->xml_create_by_pm2pm_treetopo=xml_create_by_pm2pm_treetopo;
	

	/*Functions for get data from xml files*/
	THIS->get_request_from_xml1=get_request_from_xml1;
	THIS->get_vmpm_from_xml2=get_vmpm_from_xml2;
	THIS->get_vm2vm_from_xml22=get_vm2vm_from_xml22;
	THIS->get_pm2pm_from_xml21=get_pm2pm_from_xml21;

}

/*-----------------------------------------------------*/
/*-------Create xml functions--------------------------*/
/*******************************************************/
/*
 * ������ : void xml_create_by_pms_vms()
 * ���� : ����������������Ϣ����xml�ĵ���xml2.xml
 * ������� : xmlfilename Ϊ���ɵ��ļ���
 * ����ֵ :��
 * @created by �ֿ�ΰ in 2013.5.12
 * @Modified by Jianhai in 2013-05-13
 */
void xml_create_by_pms_vms(char *xmlfilename)
{ 
	int i,j;
	
	char strBuf[30];
	char ret[400]; /*һ��VMG������*/

	xmlDocPtr doc = NULL;       
	xmlNodePtr root_node = NULL, node = NULL, node1 = NULL;

	/*�������ڵ�*/
	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "DATA");
	xmlNewProp(root_node,BAD_CAST"VERSION",BAD_CAST"0.3");

	xmlDocSetRootElement(doc, root_node);

	
	/////////////////////////////////////////////////////////////////
	/*���ɿ�FRAME��Ϣ*/

	for(i=0;i<_xpthis->sd->fr_total;i++){
	
		/*����FRAMEԪ��*/
		node = xmlNewNode(NULL, BAD_CAST "FRAME");
		
		/*����FRAME��PMIDS����*/
		xmlNewProp(node,BAD_CAST"PMIDS",BAD_CAST _xpthis->sd->framelist[i].pmids);

		/*����FRAME��ID����*/
		xmlNewProp(node,BAD_CAST"ID",BAD_CAST _xpthis->sd->framelist[i].sid);
		
		
		xmlAddChild(root_node, node);
	}
	
	/*��GROUPxin*/
	for(i=0;i<_xpthis->sd->gr_total;i++){
	
		/*����GROUPԪ��*/
		node = xmlNewNode(NULL, BAD_CAST "GROUP");
		
		/*����GROUP��PMIDS����*/
		xmlNewProp(node,BAD_CAST"PMIDS",BAD_CAST _xpthis->sd->pmglist[i].pmids);

		/*����GROUP��ID����*/
		xmlNewProp(node,BAD_CAST"ID",BAD_CAST _xpthis->sd->pmglist[i].sid);
		
		xmlAddChild(root_node, node);
	}

	/////////////////////////////////////////////////////////////////
	/*��������*/
	for(i=0;i<_xpthis->sd->pm_total;i++){
		
		/*����PMԪ��*/
		node = xmlNewNode(NULL, BAD_CAST "PM");
		
		/*����PM��STAT����*/
 		sprintf(strBuf,"%d",(int)_xpthis->sd->pmlist[i].host.stat);
		xmlNewProp(node,BAD_CAST"STAT",BAD_CAST strBuf); 


		/*����GROUP*/
		//sprintf(strBuf,"%d",(int)_xpthis->sd->pmlist[i].group_id);

		xmlNewProp(node,BAD_CAST"GR",BAD_CAST _xpthis->sd->pmlist[i].at_gr_ids);
		


		/*����FRAME*/
		//sprintf(strBuf,"%d",(int)_xpthis->sd->pmlist[i].frame_id);
		if(_xpthis->sd->pmlist[i].at_p_fr!=NULL)
			xmlNewProp(node,BAD_CAST"FR",BAD_CAST _xpthis->sd->pmlist[i].at_p_fr->sid);
		else
			xmlNewProp(node,BAD_CAST"FR",BAD_CAST "");

		/*����PM��IP����*/
		xmlNewProp(node,BAD_CAST"IP",BAD_CAST _xpthis->sd->pmlist[i].host.ip);
		
		
		
		/*����PM��ID����*/
		//itoa(_xpthis->sd->pmlist[i].host.id,strBuf,10);
		//sprintf(strBuf,"%d",(int)_xpthis->sd->pmlist[i].host.id);
		xmlNewProp(node,BAD_CAST"ID",BAD_CAST _xpthis->sd->pmlist[i].host.sid);
		

		xmlAddChild(root_node, node);

		/*����METRICԪ��*/
		for(j=0;j<DIM;j++){
			node1 = xmlNewNode(NULL,BAD_CAST"METRIC");
			
			/*ָ��ֵ Ԥ��*/
			xmlNewProp (node1,BAD_CAST"RESERVED",BAD_CAST DoubleToChar(_xpthis->sd->pmlist[i].host.load[j].reserved,2));
			/*ָ��ֵ ����*/
			xmlNewProp (node1,BAD_CAST"TOTAL",BAD_CAST DoubleToChar(_xpthis->sd->pmlist[i].host.load[j].total,2));
			/*ָ��ֵ ������*/
			xmlNewProp (node1,BAD_CAST"USED",BAD_CAST DoubleToChar(_xpthis->sd->pmlist[i].host.load[j].used,2));
			
			/*����METRIC����enum MetricName {CPU = 0, MEM, DISKIO, FPGA, ETH0, ETH1, ETH2, ETH3, SUM, MAX, MIN};*/
			switch(j){
			case CPU:
				xmlNewProp(node1,BAD_CAST"NAME",BAD_CAST"cpu");
				break;

			case MEM:
				xmlNewProp(node1,BAD_CAST"NAME",BAD_CAST"mem");
				break;

			case DISKIO:
				xmlNewProp(node1,BAD_CAST"NAME",BAD_CAST"diskio");
				break;

			case FPGA:
				xmlNewProp(node1,BAD_CAST"NAME",BAD_CAST"fpga");
				break;

			case ETH0:
				xmlNewProp(node1,BAD_CAST"NAME",BAD_CAST"eth0");
				break;
			case ETH1:
				xmlNewProp(node1,BAD_CAST"NAME",BAD_CAST"eth1");
				break;

			case ETH2:
				xmlNewProp(node1,BAD_CAST"NAME",BAD_CAST"eth2");
				break;

			case ETH3:
				xmlNewProp(node1,BAD_CAST"NAME",BAD_CAST"eth3");
				break;
			}

			xmlAddChild(node,node1);
		}
	}

	/////////////////////////////////////////////////////////////////
	/*��������*/
	for(i=0;i<_xpthis->sd->vm_total;i++){

		node = xmlNewNode(NULL, BAD_CAST "VM");
		//		xmlNewProp(node,BAD_CAST"STAT",BAD_CAST NumToChar(g_vmlist[i].stat));    û��stat��ֵ
		/*����PM*/		
		xmlNewProp(node,BAD_CAST"AT",BAD_CAST _xpthis->sd->pmlist[_xpthis->sd->vmlist[i].at_pmid].host.sid);

		
		/*����VM��STAT����*/
 		xmlNewProp(node,BAD_CAST"STAT",BAD_CAST NumToChar(_xpthis->sd->vmlist[i].host.stat));

		/*IP*/	
		xmlNewProp(node,BAD_CAST"IP",BAD_CAST _xpthis->sd->vmlist[i].host.ip);
	
		/*����ID����*/
		xmlNewProp(node,BAD_CAST"ID",BAD_CAST _xpthis->sd->vmlist[i].host.sid);
		

		xmlAddChild(root_node, node);

		for(j=0;j<DIM;j++){

			/*METRIC*/
			node1=xmlNewNode(NULL,BAD_CAST"METRIC");
			
			/*TOTAL����*/
			xmlNewProp(node1,BAD_CAST"TOTAL",BAD_CAST DoubleToChar(_xpthis->sd->vmlist[i].host.load[j].total,2));
			
			/*USED����*/
			xmlNewProp(node1,BAD_CAST"USED",BAD_CAST DoubleToChar(_xpthis->sd->vmlist[i].host.load[j].used,2));


			/*����METRIC����enum MetricName {CPU = 0, MEM, DISKIO, FPGA, ETH0, ETH1, ETH2, ETH3, SUM, MAX, MIN};*/
			switch(j){
			case CPU:
				xmlNewProp(node1,BAD_CAST"NAME",BAD_CAST"cpu");
				break;

			case MEM:
				xmlNewProp(node1,BAD_CAST"NAME",BAD_CAST"mem");
				break;

			case DISKIO:
				xmlNewProp(node1,BAD_CAST"NAME",BAD_CAST"diskio");
				break;

			case FPGA:
				xmlNewProp(node1,BAD_CAST"NAME",BAD_CAST"fpga");
				break;

			case ETH0:
				xmlNewProp(node1,BAD_CAST"NAME",BAD_CAST"eth0");
				break;
			case ETH1:
				xmlNewProp(node1,BAD_CAST"NAME",BAD_CAST"eth1");
				break;

			case ETH2:
				xmlNewProp(node1,BAD_CAST"NAME",BAD_CAST"eth2");
				break;

			case ETH3:
				xmlNewProp(node1,BAD_CAST"NAME",BAD_CAST"eth3");
				break;
			}

	
			xmlAddChild(node,node1);
		}
	}

	/*VM��vmGROUP*/
	for(i=0;i<_xpthis->sd->vmg_total;i++){
		char tvmids[300];
		cls_st_vm_group vmg;
		vmgPtr p_vg;
		p_vg=&_xpthis->sd->vmglist[i];
		
		/*��ȡ*/
		init_vmgroup(&vmg);
		vmg.sd=_xpthis->sd;
		vmg.getVMidstrByvmg(p_vg->groupid,tvmids);

		
		/*����GROUPԪ��*/
		node = xmlNewNode(NULL, BAD_CAST "VMG");
		
		/*����VMGROUP��VMIDS����*/
		xmlNewProp(node,BAD_CAST"VMIDS",BAD_CAST tvmids);

		/*����VMGROUP��ID����*/
		xmlNewProp(node,BAD_CAST"ID",BAD_CAST p_vg->sid);
		
		xmlAddChild(root_node, node);
	}



	/*����������ļ�*/
	xmlSaveFormatFileEnc(xmlfilename, doc, "UTF-8", 1);
	
	/*�ͷ�*/
	xmlFreeDoc(doc);
	
	xmlCleanupParser();
	xmlMemoryDump();
}


/*
 * ������ : void xml_create_by_vm2vm()
 * ���� : �������֮���������Ϣ����xml�ĵ�
 * ������� : urlfile- xmlfilename
 * ����ֵ :��
 * @created by �ֿ�ΰ in 2013.5.12
 * @Modified by Jianhai in 2013-05-13
 */

void xml_create_by_vm2vm(char *xmlfilename)
{
	int i;
	xmlDocPtr doc = NULL;       
	xmlNodePtr root_node = NULL, node = NULL, node1 = NULL;
	//�������ڵ�
	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "DATA");
	xmlNewProp(root_node,BAD_CAST"VERSION",BAD_CAST"0.1");

	xmlDocSetRootElement(doc, root_node);

	/*��ûһ���׺��Թ�ϵ�����XML*/
	for(i=0;i<_xpthis->sd->v2v_total;i++){

		/*�������ڵ�*/
		node = xmlNewNode(NULL, BAD_CAST "AFFINITY");

		/*����AT����*/
		xmlNewProp(node,BAD_CAST"AT",BAD_CAST _xpthis->sd->v2vlist[i].at);
		/*����TYPE����*/
		xmlNewProp(node,BAD_CAST"TYPE",BAD_CAST NumToChar (_xpthis->sd->v2vlist[i].affinity));

		/*����TRAFFIC����*/
		if(_xpthis->sd->v2vlist[i].traffic>0)
			xmlNewProp(node,BAD_CAST"TRAFFIC",BAD_CAST DoubleToChar (_xpthis->sd->v2vlist[i].traffic,0));
		else
			xmlNewProp(node,BAD_CAST"TRAFFIC",BAD_CAST " ");

		/*����DELAY����*/
		xmlNewProp(node,BAD_CAST"DELAY",BAD_CAST DoubleToChar (_xpthis->sd->v2vlist[i].delay,0));

		/*����VMIDS����*/
		xmlNewProp(node,BAD_CAST"VMIDS",BAD_CAST _xpthis->sd->v2vlist[i].vmids);

		/*
		xmlNewProp(node,BAD_CAST"DSTID",BAD_CAST NumToChar(_xpthis->sd->v2vlist[i].p_dst_vm->host.id));
		xmlNewProp(node,BAD_CAST"SRCID",BAD_CAST NumToChar(_xpthis->sd->v2vlist[i].p_src_vm->host.id));
		*/

		xmlAddChild(root_node, node);
	}

	/*�����ļ�*/
	xmlSaveFormatFileEnc(xmlfilename, doc, "UTF-8", 1);
	
	/*�ͷ�*/
	xmlFreeDoc(doc);
	xmlCleanupParser();

	xmlMemoryDump();
}


/*������������XML21 cjh add 7.28*/
void xml_create_by_pm2pm_treetopo(char *xmlfilename)
{
	int i;
	pm2swPtr p_ps;
	sw2swPtr p_ss;
	swPtr p_sw;
	xmlDocPtr doc = NULL;       
	xmlNodePtr root_node = NULL, node = NULL, node1 = NULL;
	//�������ڵ�
	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "DATA");
	xmlNewProp(root_node,BAD_CAST"VERSION",BAD_CAST"0.1");

	xmlDocSetRootElement(doc, root_node);

	/*---1.����SWITCH�б�---------------*/
	

	/*��ÿһ��SWITHC�����XML21*/
	for(i=0;i<_xpthis->sd->sw_total;i++){
		
		p_sw=&_xpthis->sd->swlist[i];
		
		/*�������ڵ�*/
		node = xmlNewNode(NULL, BAD_CAST "SW");

		/*����id����*/
		xmlNewProp(node,BAD_CAST"ID",BAD_CAST p_sw->sid);

		//printf(" ---%s ---\n",p_sw->sid);

		/*����LEVEL����*/
		xmlNewProp(node,BAD_CAST"LEV",BAD_CAST NumToChar(p_sw->level));

		/*�˿ڸ�������*/
		xmlNewProp(node,BAD_CAST"PORTNUM",BAD_CAST "24");
		
		xmlAddChild(root_node, node);
	}

	/*��ÿһ��SW2SW���ӹ�ϵ�����XML21*/
	for(i=0;i<_xpthis->sd->s2s_total;i++){
		p_ss=&_xpthis->sd->s2slist[i];
		/*�������ڵ�*/
		node = xmlNewNode(NULL, BAD_CAST "S2S");
		
		/*����delay����*/
		xmlNewProp(node,BAD_CAST"DELAY",BAD_CAST DoubleToChar(p_ss->delay,0));

		/*����BANDWIDTH����*/
		if(_xpthis->sd->s2slist[i].bandwidth>0)
			xmlNewProp(node,BAD_CAST"BW",BAD_CAST DoubleToChar (p_ss->bandwidth,0));
		else
			xmlNewProp(node,BAD_CAST"BW",BAD_CAST "");

		/*����DST: SWITCH SID����*/
		xmlNewProp(node,BAD_CAST"DST",BAD_CAST p_ss->p_dst_sw->sid);

		/*����SRC: PMSID����*/
		xmlNewProp(node,BAD_CAST"SRC",BAD_CAST p_ss->sw_src_sid);

		/*����ID����:PS(pm2sw), SS(sw2sw)*/
		xmlNewProp(node,BAD_CAST"ID",BAD_CAST NumToChar(p_ss->id));
		
		xmlAddChild(root_node, node);
	}

	
	/*��ÿһ��PM2SW���ӹ�ϵ�����XML21*/
	
	for(i=0;i<_xpthis->sd->p2s_total;i++){
		p_ps=&_xpthis->sd->p2slist[i];
		/*�������ڵ�*/
		node = xmlNewNode(NULL, BAD_CAST "P2S");

		/*����delay����*/
		xmlNewProp(node,BAD_CAST"DELAY",BAD_CAST DoubleToChar(p_ps->delay,0));

		/*����BANDWIDTH����*/
		if(_xpthis->sd->p2slist[i].bandwidth>0)
			xmlNewProp(node,BAD_CAST"BW",BAD_CAST DoubleToChar (p_ps->bandwidth,0));
		else
			xmlNewProp(node,BAD_CAST"BW",BAD_CAST "");
		
		
		/*����DST: SWITCH SID����*/
		xmlNewProp(node,BAD_CAST"DPORT",BAD_CAST NumToChar(p_ps->swithch_port));


		/*����DST: SWITCH SID����*/
		xmlNewProp(node,BAD_CAST"DST",BAD_CAST p_ps->switch_sid);

		/*����SRC: PMSID����*/
		xmlNewProp(node,BAD_CAST"SPORT",BAD_CAST NumToChar( p_ps->port_id));

		/*����SRC: PMSID����*/
		xmlNewProp(node,BAD_CAST"SRC",BAD_CAST p_ps->pm_sid);


		/*����ID����:PS(pm2sw), SS(sw2sw)*/
		xmlNewProp(node,BAD_CAST"ID",BAD_CAST NumToChar( p_ps->id));
		

		xmlAddChild(root_node, node);
	}


	/*�����ļ�*/
	xmlSaveFormatFileEnc(xmlfilename, doc, "UTF-8", 1);
	
	/*�ͷ�*/
	xmlFreeDoc(doc);
	xmlCleanupParser();

	xmlMemoryDump();

}



void xml_create_by_pm2pm(char *filename)
{
	return;

}

/*����ACTIONS XML�б�*/
void xml_create_by_actions(char *filename)
{
	int i,j,action;
	char idstr[30],ast[30],msg[200];
	xmlDocPtr doc = NULL;       
	xmlNodePtr root_node = NULL, node = NULL, node1 = NULL;
	//�������ڵ�
	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "ACTION");
	xmlNewProp(root_node,BAD_CAST"VERSION",BAD_CAST"0.1");

	xmlDocSetRootElement(doc, root_node);

	for(i=0;i<_xpthis->sd->action_total;i++){

		action=_xpthis->sd->actionlist[i].action;
		switch (action) /*0-PLACE,1-MIGRATE,2-DOWN,3-UP,4-MSG_PLACE*/
		{
		case 0:
			node = xmlNewNode(NULL, BAD_CAST "PLACE");
			strcpy(idstr,"");
			strcat(idstr,"ID_");
                        sprintf(ast,"%d",_xpthis->sd->actionlist[i].id);
			//itoa(_xpthis->sd->actionlist[i].id,ast,10);
			strcat(idstr,ast);

			xmlNewProp(node,BAD_CAST"ID",BAD_CAST idstr);

			strcpy(idstr,"");
			//itoa(_xpthis->sd->actionlist[i].place_pm->host.id,ast,10);
			strcat(idstr,_xpthis->sd->actionlist[i].place_pm->host.sid);
			xmlNewProp(node,BAD_CAST"TRAGET",BAD_CAST idstr);

			strcpy(idstr,"");
			sprintf(ast,"%s",_xpthis->sd->actionlist[i].place_vm->host.sid); //for linux
			//itoa(_xpthis->sd->actionlist[i].place_vm->host.id,ast,10); for windows
			strcat(idstr,ast);
			strcat(ast,_xpthis->sd->actionlist[i].place_vm->host.sid);
			xmlNewProp(node,BAD_CAST"VMID",BAD_CAST idstr);


			break;
		case 1:
			node = xmlNewNode(NULL, BAD_CAST "MIGRATE");
			strcpy(idstr,"");
			strcat(idstr,"ID_");
                        sprintf(ast,"%d",_xpthis->sd->actionlist[i].id);
			//itoa(_xpthis->sd->actionlist[i].id,ast,10);
			strcat(idstr,ast);

			xmlNewProp(node,BAD_CAST"ID",BAD_CAST idstr);

			strcpy(idstr,"");
			//itoa(_xpthis->sd->actionlist[i].place_pm->host.id,ast,10);
			strcat(idstr,_xpthis->sd->actionlist[i].mig_src->host.sid);
			xmlNewProp(node,BAD_CAST"SRC",BAD_CAST idstr);

			strcpy(idstr,"");
			sprintf(ast,"%s",_xpthis->sd->actionlist[i].mig_vm->host.sid); //for linux
			//itoa(_xpthis->sd->actionlist[i].place_vm->host.id,ast,10); for windows
			strcat(idstr,ast);
			strcat(ast,_xpthis->sd->actionlist[i].mig_vm->host.sid);
			xmlNewProp(node,BAD_CAST"VMID",BAD_CAST idstr);
			
			strcpy(idstr,"");
			//itoa(_xpthis->sd->actionlist[i].place_pm->host.id,ast,10);
			strcat(idstr,_xpthis->sd->actionlist[i].mig_dst->host.sid);
			xmlNewProp(node,BAD_CAST"DST",BAD_CAST idstr);

			break;
		case 2:
			node = xmlNewNode(NULL, BAD_CAST "DOWN");
			strcpy(idstr,"");
			strcat(idstr,"ID_");
                        sprintf(ast,"%d",_xpthis->sd->actionlist[i].id);
			//itoa(_xpthis->sd->actionlist[i].id,ast,10);
			strcat(idstr,ast);

			xmlNewProp(node,BAD_CAST"ID",BAD_CAST idstr);

			strcpy(idstr,"");
			//itoa(_xpthis->sd->actionlist[i].place_pm->host.id,ast,10);
			strcat(idstr,_xpthis->sd->actionlist[i].down_pm->host.sid);
			xmlNewProp(node,BAD_CAST"TRAGET",BAD_CAST idstr);
			break;
		case 4:
			node = xmlNewNode(NULL, BAD_CAST "MSG");
			strcpy(idstr,"ID_");
			sprintf(ast,"%d",_xpthis->sd->actionlist[i].id);
			strcat(idstr,ast);
			xmlNewProp(node,BAD_CAST"ID",BAD_CAST idstr);
			strcpy(msg,_xpthis->sd->actionlist[i].mesg);
			xmlNewProp(node,BAD_CAST"CONT",BAD_CAST msg);
			strcpy(msg,_xpthis->sd->actionlist[i].unpackvm[0]->host.sid);
			for(j=1;j<_xpthis->sd->actionlist[i].unpack_vm_total;j++)
			{
				strcat(msg,",");
				strcat(msg,_xpthis->sd->actionlist[i].unpackvm[j]->host.sid);
			}
			xmlNewProp(node,BAD_CAST"VMLIST",BAD_CAST msg);
			switch (_xpthis->sd->actionlist[i].msgtype)
			{
			case 0:
				strcpy(ast,"PLACE");
				break;
			case 1:
				strcpy(ast,"LOADBALANCE");
				break;
			case 2:
				strcpy(ast,"HOTSOLVE");
				break;
			case 3:
				strcpy(ast,"COLDSOLVE");
				break;
			default:
				break;
			}
			xmlNewProp(node,BAD_CAST"TYPE",BAD_CAST ast);
			break;
		default:
			break;
		}
		
		xmlAddChild(root_node, node);
	}

	/*�����ļ�*/
	xmlSaveFormatFileEnc(filename, doc, "UTF-8", 1);
	
	/*�ͷ�*/
	xmlFreeDoc(doc);
	xmlCleanupParser();

	xmlMemoryDump();
}

/******************************************************************************************/

	
/****2.��ȡXML����ͨ�ú���******************************************************/

/*��ȡINT���͵�����ֵ CJH add 7.30*/
int getint_attrib(xmlTextReaderPtr reader, char *fldname,int disp)
{
	/* handling of a node in the tree */
	xmlChar *att;
	char *endptr;
	int aValue=0;
	
	att=xmlTextReaderGetAttribute(reader,(xmlChar *)fldname);
	if(att!=NULL) {
		aValue = (int)strtod(att,&endptr);
			
		if(DEBUG && disp) printf("%s: %d ",fldname,aValue);
	}
	xmlFree(att);

	return aValue;
}


			
/*��ȡDOUBLE���͵�����ֵ CJH add 7.30*/
double getdbl_attrib(xmlTextReaderPtr reader,char *fldname,int disp)
{
	/* handling of a node in the tree */
	xmlChar *att;
	char *endptr;
	double aValue=0;
	
	att=xmlTextReaderGetAttribute(reader,(xmlChar *)fldname);
	if(att!=NULL) {
		aValue = strtod(att,&endptr);
		
		if(DEBUG && disp) printf("%s: %.2f ",fldname,aValue);
	}
	xmlFree(att);

	return aValue;
}

//
/*��ȡDOUBLE���͵�����ֵ CJH add 7.30*/
void getstr_attrib(xmlTextReaderPtr reader,char *fldname, char *ret,int disp)
{
	/* handling of a node in the tree */
	xmlChar *att;
	
	att=xmlTextReaderGetAttribute(reader,(xmlChar *)fldname);
	if(att!=NULL) {
		strcpy(ret,(char *)att);
		
		if(DEBUG && disp) printf("%s: %s \n",fldname,att);
	}
	xmlFree(att);

}

/******************************************************************************************/

//Process every node of XML1 file to get values 
//����XML1ÿ���ڵ��ȡ���ֵ
static void process_XML1_Node(xmlTextReaderPtr reader, 
							  int *reqcount,
							  int *algparacount
							  ) 
{
    /* handling of a node in the tree */
	xmlChar *name, *att, *att1, *att2;
	char *endptr;
	int nt;	
	double aValue;
	requestPtr p_req;
	char *str_arr[DIM];
	int i;
	int kk=-1;//��kk��¼���������ֵ
	int iPara=-1;

	kk=*reqcount;
	iPara=*algparacount;

    name = xmlTextReaderName(reader);
    if (name == NULL)
        name = xmlStrdup(BAD_CAST "--");
	nt=xmlTextReaderNodeType(reader);

	/*���Ԫ��������REQUEST�����ȡ����ID,TYPE��ENABLE��ֵ*/
	if (xmlStrcmp(name, (xmlChar *) "REQUEST")==0) {
		if(nt==XML_READER_TYPE_ELEMENT) 
		{
			kk++;iPara=0;
			p_req=&_xpthis->sd->reqlist[kk];
			p_req->id=kk;
			
			if(DEBUG) printf("Request %d ",kk);
			
			getstr_attrib(reader,"TYPE",p_req->type,1);

			p_req->enable=getint_attrib(reader,"ENABLE",1);
		}
	}

	/*���Ԫ��������ALGORITHM�����ȡ����name��ֵ*/
	if (xmlStrcmp(name, (const xmlChar *) "ALGORITHM")==0) {
		if(nt==XML_READER_TYPE_ELEMENT) 
		{
			iPara=0;
			p_req=&_xpthis->sd->reqlist[kk];
			getstr_attrib(reader,"NAME",p_req->alg.algName,1);

		}
		else if(nt==XML_READER_TYPE_END_ELEMENT) /*</ALGORITHM>*/
		{
			_xpthis->sd->reqlist[kk].alg.paracount=iPara; /*��������*/
		}

	}

	/*���Ԫ��������ALGORITHM�����ȡ����name��ֵ*/
	if (xmlStrcmp(name, (const xmlChar *) "PARAMETER")==0) {
		if(nt==XML_READER_TYPE_ELEMENT) 
		{
			int cmp;
			p_req=&_xpthis->sd->reqlist[kk];
			getstr_attrib(reader,"NAME",p_req->alg.para[iPara].para_name,1);
			getstr_attrib(reader,"TYPE",p_req->alg.para[iPara].para_type,1);

			/*��ȡ����*/
			cmp=strcmp(p_req->alg.para[iPara].para_type,"DOUBLE_ARRAY");
			getstr_attrib(reader,"VALUE",p_req->alg.para[iPara].para_str_val,1);
			
			
			/*��ȡ��������*/
			cmp=strcmp(p_req->alg.para[iPara].para_type,"DOUBLE");
			if(cmp==0)
				p_req->alg.para[iPara].para_value=getdbl_attrib(reader,"VALUE",1);

			if (strcmp(p_req->alg.para[iPara].para_name,"balance")==0)
				_xpthis->sd->balance = p_req->alg.para[iPara].para_value;

			if (strcmp(p_req->alg.para[iPara].para_name,"maxcount")==0)
				_xpthis->sd->MaxcountOfBalance = p_req->alg.para[iPara].para_value;

			iPara++; /*������������1*/
		}
	}
	
	*algparacount=iPara;

	*reqcount=kk;//set request count
    xmlFree(name);  
}


/*----------------------------------------------------------------------*/

/*Read values from XML2 file ��ȡXML2�ļ�*/
static int get_request_from_xml1(char *filename) 
{
    xmlTextReaderPtr reader;
	xmlChar *name;
    
	int reqc=-1,parac=-1;
	int ret=0;
	
    reader = xmlNewTextReaderFilename(filename);
	
    if (reader != NULL) {
        ret = xmlTextReaderRead(reader);
		
		name = xmlTextReaderName(reader);/*��ȡreader��Ԫ������*/
		if (name == NULL)
			name = xmlStrdup(BAD_CAST "--");
		
		/*����������У�������Ҫȷ���ĵ�����ȷ�����͡���DATA���������ʾ����ʹ���ĵ��ĸ����͡�*/
		if (xmlStrcmp(name, (const xmlChar *) "CONTROLOR")) {
			fprintf(stderr,"document of the wrong type, root node != CONTROLOR");
			xmlFree(name);
			xmlFreeTextReader(reader);
			return ret;
		}
		
		
        while (ret == 1) {/*����ÿ���ڵ�*/
            process_XML1_Node(reader, &reqc, &parac);
			
            ret = xmlTextReaderRead(reader);
	    }
        
		xmlFreeTextReader(reader);

        if (ret != 0) {
            printf("%s : failed to parse\n", filename);
        }
    
	} 
	else 
	    printf("Unable to open %s\n", filename);
    
	
	/*���¹�������*/
	_xpthis->sd->req_total=reqc+1;
	
	

	return ret;
}



/******************************************************************************************/

/******************************************************************************************/
/***********************************************************************/
/*
* ������ : process_XML2_Node
* ���� : ����XML2ÿ���ڵ��ȡ���ֵ
* ������� : xmlTextReaderPtr reader,int *pmcount,int *vmcount,int *grcount,int *frcount
* ����reader : XML�������������
* ����pmcount : ���������
* ����vmcount : �����������
* ����grcount : �������ĸ���
* ����frcount : ��ĸ���
* ����ֵ : ��
* @created by �½��� in 2013.5.12
*/
void process_XML2_Node(xmlTextReaderPtr reader, 
					   int *pmcount,int *vmcount,
					   int *grcount,int *frcount,int *vmgcout) 
{
 
	xmlChar *name, *att;
	char *endptr,iput[20];
	int rr,nt;	
	double aValue;
	framePtr p_fr;
	pmPtr p_pm; vmPtr p_vm;
	pmgPtr p_pmg;
	vmgPtr p_vg;
	cls_st_host hs;


	int countofvm;

	int kk=-1,ll=-1,ff=-1,gg=-1,vv=-1;//��kk��¼�����������ֵ

	kk=*pmcount;
	ll=*vmcount;
	ff=*frcount;
	gg=*grcount;
	vv=*vmgcout;

    name = xmlTextReaderName(reader);
    if (name == NULL)
        name = xmlStrdup(BAD_CAST "--");
	nt=xmlTextReaderNodeType(reader);

	
	/*���Ԫ��������FRAME�����ȡ����ID��NAME, PMIDS��ֵ*/
	if (xmlStrcmp(name,  (const xmlChar *) "FRAME")==0) {
		if(nt==XML_READER_TYPE_ELEMENT) 
		{
			ff++;

			p_fr=&_xpthis->sd->framelist[ff];
			p_fr->id=ff;//�����

			//��ȡ���SID
			getstr_attrib(reader,"ID",p_fr->sid, ff<0);
			
			//��ȡ������������ID
			getstr_attrib(reader,"PMIDS",p_fr->pmids,ff<0);

		}
	}

	
	/*���Ԫ��������GROUP�����ȡ����ID��NAME, PMIDS��ֵ*/
	if (xmlStrcmp(name, (const xmlChar *) "GROUP")==0) {
		if(nt==XML_READER_TYPE_ELEMENT) 
		{
			gg++;
			p_pmg =&_xpthis->sd->pmglist[gg];
			p_pmg->id=gg;//�����
			

			/*��ȡ���SID*/
			getstr_attrib(reader,"ID",p_pmg->sid,gg<0);

			//��ȡzu�����������ID
			p_pmg->pmids = ++_xpthis->sd->shareStrPtr;
			getstr_attrib(reader,"PMIDS",p_pmg->pmids,gg<0);
			_xpthis->sd->shareStrPtr += strlen(p_pmg->pmids);

		}
	}


	/*���Ԫ��������PM�����ȡ����ID��IP,STAT��ֵ*/
	if (xmlStrcmp(name, (const xmlChar *) "PM")==0) {
		if(nt==XML_READER_TYPE_ELEMENT) 
		{
			cls_st_frame clsfr;
			char *strs[100],*endptr;
			int i,cc;



			kk++;
			p_pm=&_xpthis->sd->pmlist[kk];
			p_pm->host.id=kk;

			/*��ȡPM��SID*/
			getstr_attrib(reader,"ID",p_pm->host.sid,kk<0);

			/*��ȡPM��IP*/
			getstr_attrib(reader,"IP",p_pm->host.ip,kk<0);
			
			//��ȡPM״̬
			p_pm->host.stat=getint_attrib(reader,"STAT",kk<0);

			/*��ȡPM��������Ϣ*/
			getstr_attrib(reader,"GR",p_pm->at_gr_ids,kk<0);
			
			/*���µ�p_pm��ǰ��������
			p_pm->at_p_pmgrps;
			p_pm->countofpmgrp;*/
			/*��(gr1,gr2,gr3,gr5)�ָ��1 2 3 4����*/
			cc=split_str(p_pm->at_gr_ids,",",strs);
			p_pm->countofpmgrp=cc;

			for(i=0;i<cc;i++)
			{
				cls_st_pmgroup clspmg;
				pmgPtr ppg;
				initPmgroup(&clspmg);
				clspmg.sd=_xpthis->sd;

				/*��ȡstrs[i]: gr0*/
				/*����gr0��ȡpmg*/
				p_pm->at_p_pmgrps[i]=clspmg.getpmgbysid(strs[i]);
			}



			/*��ȡPM���ڿ���Ϣ*/
			getstr_attrib(reader,"FR",p_pm->at_fr_sid,kk<0);
			
			initframe(&clsfr);
			clsfr.sd=_xpthis->sd;
			p_pm->at_p_fr=clsfr.getfrbysid(p_pm->at_fr_sid,kk<0);


			if(DEBUG && kk<0) printf("\n----\n");
		}
		//return;
	}
	
	/*���Ԫ��������VM�����ȡ����ID��IP,STAT��ֵ*/
	if (xmlStrcmp(name, (const xmlChar *) "VM")==0) {
		
		if(nt==XML_READER_TYPE_ELEMENT) 
		{
			ll++;
			
			p_vm=&_xpthis->sd->vmlist[ll];
			p_vm->host.id=ll;
			
			//��ȡ����Id
			getstr_attrib(reader,"ID",p_vm->host.sid,ll<0);
			
			//��ȡ����IP
			getstr_attrib(reader,"IP",p_vm->host.ip,ll<0);
			
			//��ȡ������״̬
			p_vm->host.stat=getint_attrib(reader,"STAT",ll<0);


			//��ȡVM����PM
			
			att=xmlTextReaderGetAttribute(reader,"AT");
			_xpthis->sd->vmlist[ll].at = _xpthis->sd->vmlist[ll].at_pmid = -1;
			if(att!=NULL) {
				/*��õ���pm0����Ҫ��ȡ��Ӧ��pmid*/
				initHost(&hs);
				hs.sd = _xpthis->sd;
				
				p_pm=hs.getpmbysid(att);

				if(p_pm!=NULL){
					_xpthis->sd->vmlist[ll].at=p_pm->host.id;
					_xpthis->sd->vmlist[ll].at_pmid = INVI;//���at_pmid�ĳ�ʼ����by sun in 8-15
					countofvm=_xpthis->sd->pmlist[p_pm->host.id].countofvm++;
					if (countofvm>=100){
						printf("There's more than 100 VMs in PM %d. Please check the data.\n",
							p_pm->host.id);
						_xpthis->sd->readFailed = 1;
						return ;

					}
					_xpthis->sd->pmlist[p_pm->host.id].p_vms[countofvm]=&_xpthis->sd->vmlist[ll];
				}
				else _xpthis->sd->vmlist[ll].at = -1;
				if(DEBUG && ll<0) printf("atpm: %s ",att);
			}
			xmlFree(att);
			

			if(DEBUG && ll<0) printf("\n-----\n");	
		}
	}

	/*���Ԫ��������METRIC�����ȡ����NAME��USED,TOTAL, RESERVED��ֵ*/
	if (xmlStrcmp(name, (const xmlChar *) "METRIC")==0) {
		
		att=xmlTextReaderGetAttribute(reader,"NAME");		
		//{ CPU = 0, MEM, DISKIO, FPGA, ETH0, ETH1, ETH2, ETH3 };
		if (ll==-1){
			rr = _xpthis->sd->pmlist[kk].host.dimRead++;
		}else 
			rr = _xpthis->sd->vmlist[ll].host.dimRead++;
		/*rr=0;
		if (1) {
			int first = 1;
			if(xmlStrcmp(att, (const xmlChar *) "cpu")==0) 
				rr=CPU;
			else if(xmlStrcmp(att, (const xmlChar *) "mem")==0) 
				rr=MEM;
			else if(xmlStrcmp(att, (const xmlChar *) "diskio")==0) 
				rr=DISKIO;
			else if(xmlStrcmp(att, (const xmlChar *) "fpga")==0) 
				rr=FPGA;
			else if(xmlStrcmp(att, (const xmlChar *) "eth0")==0) 
				rr=ETH0;
			else if(xmlStrcmp(att, (const xmlChar *) "eth1")==0) 
				rr=ETH1;
			else if(xmlStrcmp(att, (const xmlChar *) "eth2")==0) 
				rr=ETH2;
			else if(xmlStrcmp(att, (const xmlChar *) "eth3")==0) 
				rr=ETH3;
			
		}*/
		
		//get the attribute value metricname
		if(ll==-1)
			strcpy(_xpthis->sd->pmlist[kk].host.load[rr].metric_name,att);
		else
			strcpy(_xpthis->sd->vmlist[ll].host.load[rr].metric_name,att);

		//if(DEBUG && ll<0 && kk<0) printf(" %s ",att); 

		xmlFree(att);
		
		//get the attribute value used
		att=xmlTextReaderGetAttribute(reader,"USED");
		if(att!=NULL) {
			//strcpy(iput,att);
			aValue = strtod(att,&endptr);

			if(ll==-1)//PM
			{
				_xpthis->sd->pmlist[kk].host.load[rr].used=aValue;
				if(DEBUG && kk<0) printf("used: %.3f ",_xpthis->sd->pmlist[kk].host.load[rr].used);
			}
			else
			{
				_xpthis->sd->vmlist[ll].host.load[rr].used=aValue;
				if(DEBUG && ll<0) printf("used: %.3f ",_xpthis->sd->vmlist[ll].host.load[rr].used);
			}
			
		}
		xmlFree(att);
		
		//get the attribute value total
		att=xmlTextReaderGetAttribute(reader,"TOTAL");
		if(att!=NULL) {
			strcpy(iput,att);
			aValue = strtod(iput,&endptr);
			if(ll==-1)//PM
			{
				_xpthis->sd->pmlist[kk].host.load[rr].total=aValue;
				if(DEBUG && kk<0) printf("total: %.3f ",_xpthis->sd->pmlist[kk].host.load[rr].total);
			}
			else
			{
				_xpthis->sd->vmlist[ll].host.load[rr].total=aValue;
				if(DEBUG && ll<0) printf("total: %.3f ",_xpthis->sd->vmlist[ll].host.load[rr].total);
			}
			
		}
		
		xmlFree(att);	
		//get the attribute value reserved
		att=xmlTextReaderGetAttribute(reader,"RESERVED");
		if(att!=NULL) {
			//strcpy(iput,att);
			aValue = strtod(att,&endptr);
			
			if(ll==-1)//PM
			{
				_xpthis->sd->pmlist[kk].host.load[rr].reserved=aValue;
				if(DEBUG && kk<0) printf("reserved: %.3f ",_xpthis->sd->pmlist[kk].host.load[rr].reserved);
			}
			else
			{
				_xpthis->sd->vmlist[ll].host.load[rr].reserved=aValue;
				if(DEBUG && ll<0) printf("reserved: %.3f ",_xpthis->sd->vmlist[ll].host.load[rr].reserved);
			}			
		}
		xmlFree(att);



	/*���Ԫ��������PM�����ȡ����ID��IP,STAT��ֵ*/
	if (xmlStrcmp(name, (const xmlChar *) "VMG")==0) {
		if(nt==XML_READER_TYPE_ELEMENT) 
		{
			char tvmids[200];
			char* strdt[200],*endptr;
			int i,cc;

			vv++;
			p_vg=&_xpthis->sd->vmglist[vv];
			p_vg->groupid=vv;

			/*��ȡPM��SID*/
			getstr_attrib(reader,"ID",p_vg->sid,vv<0);
			
			
			/*��ȡVMG��VMIDS*/
			getstr_attrib(reader,"VMIDS",tvmids,vv<0);
			/*�ֽ�tvmids�� vm_id_list*/
			
			/*��(1,2,3,5)�ָ��1 2 3 4����*/
			cc=split_str(tvmids,",",strdt);
			for(i=0;i<cc;i++)
			{
				p_vg->vm_id_list[i]=(int)strtod(strdt[i],&endptr);
			}

			//if(DEBUG && vv<100) printf("\n----\n");
		   }
		//return;
		}
	
		//if(DEBUG && kk<0) printf("\n");
	}
	

	*pmcount=kk;//set PM count
	*vmcount=ll; //set VM count
	*frcount=ff;
	*grcount=gg;
	*vmgcout=vv;

    xmlFree(name);  
}

/*Read values from XML2 file ��ȡXML2�ļ�*/
static int get_vmpm_from_xml2(char *filename) 
{
    xmlTextReaderPtr reader;
	xmlChar *name;
    
	int pc=-1,vc=-1,gc=-1,fc=-1,vv=-1;

	int ret=0;

	cls_st_frame cls_fr;
	cls_st_pmgroup cls_pmg;

	
	
    reader = xmlNewTextReaderFilename(filename);
	
    if (reader != NULL) {
        ret = xmlTextReaderRead(reader);
		
		name = xmlTextReaderName(reader);/*��ȡreader��Ԫ������*/
		if (name == NULL)
			name = xmlStrdup(BAD_CAST "--");
		
		/*����������У�������Ҫȷ���ĵ�����ȷ�����͡���DATA���������ʾ����ʹ���ĵ��ĸ����͡�*/
		if (xmlStrcmp(name,  (const xmlChar *) "DATA")) {
			fprintf(stderr,"document of the wrong type, root node != DATA");
			xmlFree(name);
			xmlFreeTextReader(reader);
			return ret;
		}
		
		 

        while (ret == 1) {
           process_XML2_Node(reader, &pc,&vc,&gc,&fc,&vv);
		   if (_xpthis->sd->readFailed ){
			   return ;
		   }
			_xpthis->sd->pm_total=pc+1;
			_xpthis->sd->vm_total=vc+1;
			_xpthis->sd->gr_total=gc+1;
			_xpthis->sd->fr_total=fc+1;
			_xpthis->sd->vmg_total=vv+1;

            ret = xmlTextReaderRead(reader);
	    }
        
		xmlFreeTextReader(reader);

        if (ret != 0) {
            printf("%s : failed to parse\n", filename);
        }
    
	} 
	else 
	    printf("Unable to open %s\n", filename);
    
	
	/*���¹�������*/
	_xpthis->sd->pm_total=pc+1;
	_xpthis->sd->vm_total=vc+1;
	_xpthis->sd->gr_total=gc+1;
	_xpthis->sd->fr_total=fc+1;
	_xpthis->sd->vmg_total=vv+1;

	initframe(&cls_fr);
	cls_fr.sd = _xpthis->sd;
	cls_fr.get_frame_pmids();

	initPmgroup(&cls_pmg);
	cls_pmg.sd = _xpthis->sd;
	cls_pmg.get_group_pmids();

	return ret;
}



/*************************************************************************/
//Process every node of XML21 file to get values 
//����XML21ÿ���ڵ��ȡ���ֵ pm2pm
void process_XML21_Node(xmlTextReaderPtr reader, int *swcount,int *pscount,int *sscount) 
{
    /* handling of a node in the tree */
	xmlChar *name;
	//char *endptr;
	int nt;	

	int swc=-1;//��vd��¼������ϵ������ֵ
	int psc=-1;
	int ssc=-1;

	swPtr p_sw;
	sw2swPtr p_ss;
	pm2swPtr p_ps;

	swc=*swcount;
	psc=*pscount;
	ssc=*sscount;


	p_sw=&_xpthis->sd->swlist[swc];
	p_ss=&_xpthis->sd->s2slist[ssc];
	p_ps=&_xpthis->sd->p2slist[psc];

    name = xmlTextReaderName(reader);
    if (name == NULL)
        name = xmlStrdup(BAD_CAST "--");
	nt=xmlTextReaderNodeType(reader);

	/*****************************************************/
	/*1.��ȡ��������Ϣ�����Ԫ��������SW�����ȡ����ID��PORTNUM,LEV��ֵ*/
	if (xmlStrcmp(name, (xmlChar *) "SW")==0) {
		
		if(nt==XML_READER_TYPE_ELEMENT) swc++;

		/*��ȡID*/
		getstr_attrib(reader,(xmlChar*)"ID",p_sw->sid,swc<0);
		
		p_sw->portnum=getint_attrib(reader,"PORTNUM",swc<0);

		p_sw->level=getint_attrib(reader,"LEV",swc<0);

	}

	/*****************************************************/
	/*2.��ȡ���������������������Ϣ�����Ԫ��������SW�����ȡ����ID��PORTNUM,LEV��ֵ*/
	if (xmlStrcmp(name, (xmlChar *) "P2S")==0) {
		
		if(nt==XML_READER_TYPE_ELEMENT) psc++;

		/*��ȡID*/
		p_ps->id=getint_attrib(reader,"ID",psc<0);
		/*��ȡSRCΪPMID*/
		getstr_attrib(reader,"SRC",p_ps->pm_sid,psc<0);
		/*��ȡ�˿ں�*/
		p_ps->port_id=getint_attrib(reader,"SPORT",psc<0);
		/*��ȡĿ�꽻����SID*/
		getstr_attrib(reader,"DST",p_ps->switch_sid,psc<0);
		/*��ȡ�˿ں�*/
		p_ps->swithch_port=getint_attrib(reader,"DPORT",psc<0);

		p_ps->bandwidth=(float)getdbl_attrib(reader,"BW",psc<0); 
		p_ps->delay=(float)getdbl_attrib(reader,"DELAY",psc<0);

	}


	
	/*****************************************************/
	/*3.��ȡ�������뽻������������Ϣ�����Ԫ��������SW�����ȡ����ID��PORTNUM,LEV��ֵ*/
	if (xmlStrcmp(name, (xmlChar *) "S2S")==0) {
		
		if(nt==XML_READER_TYPE_ELEMENT) ssc++;

		/*��ȡID*/
		p_ss->id=getint_attrib(reader,"ID",ssc<0);/*ԭ��p_ps �֣�p_ss modified by lyq 2013.8.18 */
		/*��ȡSRCΪPMID*/
		getstr_attrib(reader,"SRC",p_ss->sw_src_sid,ssc<0);
		/*��ȡĿ�꽻����SID*/
		getstr_attrib(reader,"DST",p_ss->sw_dst_sid,ssc<0);
		/*��ȡ����*/
		p_ss->bandwidth=(float)getdbl_attrib(reader,"BW",ssc<0);/*ԭ��p_ps �֣�p_ss modified by lyq 2013.8.18 */
		/*��ȡʱ��*/
		p_ss->delay=(float)getdbl_attrib(reader,"DELAY",ssc<0);/*ԭ��p_ps �֣�p_ss modified by lyq 2013.8.18 */
	}


	//if(DEBUG && ssc<0 && swc<10 && psc<0) printf("\n");

	
	*swcount=swc;//set p2p count
	*pscount=psc;
	*sscount=ssc;
	//
	///*p2p����*/
	_xpthis->sd->sw_total=*swcount;
	_xpthis->sd->p2s_total=*pscount;
	_xpthis->sd->s2s_total=*sscount;

	xmlFree(name);  

	return;
}

/*��ȡXML ��������˽ṹ CJH ADD 7.29*/
/*Read values from XML21 file ��ȡXML21��pm2pm�����֮���ʱ�Ӵ�����Ϣ�ļ�*/
int get_pm2pm_from_xml21(char *filename)
{

    xmlTextReaderPtr reader;
	xmlChar *name;
    
	int swc=0,psc=0,ssc=0;

	int ret=0;
	
    reader = xmlNewTextReaderFilename(filename);
	
    if (reader != NULL) {
        ret = xmlTextReaderRead(reader);
		
		name = xmlTextReaderName(reader);/*��ȡreader��Ԫ������*/
		if (name == NULL)
			name = xmlStrdup(BAD_CAST "--");
		
		/*����������У�������Ҫȷ���ĵ�����ȷ�����͡���DATA���������ʾ����ʹ���ĵ��ĸ����͡�*/
		if (xmlStrcmp(name, (xmlChar *) "DATA")) {
			fprintf(stderr,"document of the wrong type, root node != DATA");
			xmlFree(name);
			xmlFreeTextReader(reader);
		}	
        
		while (ret == 1) {
            process_XML21_Node(reader, &swc,&psc,&ssc);
			
            ret = xmlTextReaderRead(reader);
	    }
        
		xmlFreeTextReader(reader);

        if (ret != 0) {
            printf("%s : failed to parse\n", filename);
        }
    
	} 
	else 
	    printf("Unable to open %s\n", filename);
    
	_xpthis->sd->sw_total=swc; /*���¹������ݵ�ȫ�ֱ���*/
	_xpthis->sd->p2s_total=psc;
	_xpthis->sd->s2s_total=ssc;

	ret=swc+1;

	return ret;

}


/***********************************************************************/
/*
* ������ : process_XML22_Node
* ���� : ����XML22ÿ���ڵ��ȡ���ֵ
* Process every node of XML22 file to get values  to v2vlist
* ����XML22ÿ���ڵ��ȡ���ֵ

* ������� : xmlTextReaderPtr reader,int *VDcount
* ����reader : XML�������������
* ����vdcount : ���������
* ����ֵ : ��
* @created by �½��� in 2013.5.12
*/
/*----------------------------------------------------------------------*/

void process_XML22_Node(xmlTextReaderPtr reader, int *vdcount) 
{
    /* handling of a node in the tree */
	xmlChar *name, *att;
	char *endptr;
	char *strvm[MAX_VM_COUNT_PER_GROUP];
	char t_vmids[PMIDS_SIZE_GR];
	int nt,k;	
	int i;
	double aValue;
	vm2vmPtr p_v2v;
	int tmpSum;
	cls_st_pmgroup clspmg;
	cls_st_frame clsfr;
#define isNum(x) (x>='0'&&x<='9')
	
	int vd=-1;//��vd��¼������ϵ������ֵ
	initPmgroup(&clspmg);
	clspmg.sd=_xpthis->sd;
	vd=*vdcount;
	name = xmlTextReaderName(reader);
    if (name == NULL)
        name = xmlStrdup(BAD_CAST "--");
	nt=xmlTextReaderNodeType(reader);

	/*���Ԫ��������AFFINITY�����ȡ����VMIDS��TRAFFIC,DELAY,TYPE,AT��ֵ*/

	if (xmlStrcmp(name, (xmlChar *) "AFFINITY")==0) {
		if(nt==XML_READER_TYPE_ELEMENT) vd++;

		p_v2v=&_xpthis->sd->v2vlist[vd];

		//AFFTYPE
		p_v2v->affinity=(enum affinity_type)getint_attrib(reader,"TYPE",vd<0);

		//TRAFFIC
		p_v2v->traffic=getdbl_attrib(reader,"TRAFFIC",vd<0);

		//DELAY ʱ��Ҫ��
		p_v2v->delay=getdbl_attrib(reader,"DELAY",vd<0);

		//AT
		getstr_attrib(reader,"AT",p_v2v->at,vd<0);

		{
			//��ȡ�̶�ֵat��affinity_at by CC 10.17
			
				
			p_v2v->affinity_at = -1;
			if (p_v2v->at[0]!=0){

				cls_st_host hs;
				initHost(&hs);
				hs.sd = _xpthis->sd;
				if (p_v2v->affinity==1)
					p_v2v->affinity_at = hs.getpmbysid(p_v2v->at)->host.id;
				else if (p_v2v->affinity==3){
					p_v2v->affinity_at = clspmg.getpmgbysid(p_v2v->at)->id;
				}else if (p_v2v->affinity==2){
					int kk = 0;
					initframe(&clsfr);
					clsfr.sd=_xpthis->sd;
					p_v2v->affinity_at =clsfr.getfrbysid(p_v2v->at,kk<0)->id;
				}
			}
				
		
		}


		/*VMIDS*/
		getstr_attrib(reader,"VMIDS",p_v2v->vmids,vd<0);

		/*��vmids����õ�p_vms*/
		{
			cls_st_host hh;
			initHost(&hh);
			hh.sd = _xpthis->sd;
			strcpy(t_vmids,_xpthis->sd->v2vlist[vd].vmids);
				p_v2v->countofvm=split_str(t_vmids,",",strvm);
			
			for(k=0;k<p_v2v->countofvm;k++)
				p_v2v->p_vms[k]=hh.getvmbysid(strvm[k]); 
			
			p_v2v->p_dst_vm=_xpthis->sd->v2vlist[vd].p_vms[1];
			p_v2v->p_src_vm=_xpthis->sd->v2vlist[vd].p_vms[0];
		}	
		
		if(DEBUG && vd<0) printf("\n");
		//return;
	}
	
	*vdcount=vd;//set v2v count
	xmlFree(name);  
}



/*Read values from XML22 file ��ȡXML22�������֮���������Ϣ�ļ�*/
int get_vm2vm_from_xml22(char *filename) 
{
    xmlTextReaderPtr reader;
	xmlChar *name;
    
	int vd=-1,ret=0;
	
    reader = xmlNewTextReaderFilename(filename);
	
    if (reader != NULL) {
        ret = xmlTextReaderRead(reader);
		
		name = xmlTextReaderName(reader);/*��ȡreader��Ԫ������*/
		if (name == NULL)
			name = xmlStrdup(BAD_CAST "--");
		
		/*����������У�������Ҫȷ���ĵ�����ȷ�����͡���DATA���������ʾ����ʹ���ĵ��ĸ����͡�*/
		if (xmlStrcmp(name, (xmlChar *) "DATA")) {
			fprintf(stderr,"document of the wrong type, root node != DATA");
			xmlFree(name);
			xmlFreeTextReader(reader);
			return ret;
		}		
		
        while (ret == 1) {
            process_XML22_Node(reader,&vd);
			if (vd == 12){
				vd = 12;
			}
            ret = xmlTextReaderRead(reader);
	    }
        
		xmlFreeTextReader(reader);

        if (ret != 0) {
            printf("%s : failed to parse\n", filename);
        }
    
	} 
	else 
	    printf("Unable to open %s\n", filename);
    
	_xpthis->sd->v2v_total=vd+1;/*��ȫ�ֹ�����������¼v2v������*/
	
	return ret;
}
