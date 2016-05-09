
/* 文件注释 */
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
 * 函数名 : void xml_create_by_pms_vms()
 * 功能 : 将物理机及虚拟机信息生成xml文档：xml2.xml
 * 传入参数 : xmlfilename 为生成的文件名
 * 返回值 :无
 * @created by 贾俊伟 in 2013.5.12
 * @Modified by Jianhai in 2013-05-13
 */
void xml1_create(char *xmlfilename)
{ 
	int i,j;
	
	char strBuf[30];

	xmlDocPtr doc = NULL;       
	xmlNodePtr root_node = NULL, node = NULL, node1 = NULL;

	/*建立根节点*/
	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "DATA");
	xmlNewProp(root_node,BAD_CAST"VERSION",BAD_CAST"0.3");

	xmlDocSetRootElement(doc, root_node);

	
	/////////////////////////////////////////////////////////////////
	/*生成框FRAME信息*/

	for(i=0;i<_xpthis->sd->fr_total;i++){
	
		/*生成FRAME元素*/
		node = xmlNewNode(NULL, BAD_CAST "FRAME");
		
		/*加入FRAME的PMIDS属性*/
		xmlNewProp(node,BAD_CAST"PMIDS",BAD_CAST _xpthis->sd->framelist[i].pmids);

		/*加入FRAME的ID属性*/
		xmlNewProp(node,BAD_CAST"ID",BAD_CAST _xpthis->sd->framelist[i].sid);
		
		
		xmlAddChild(root_node, node);
	}
	
	/*组GROUPxin*/
	for(i=0;i<_xpthis->sd->gr_total;i++){
	
		/*生成GROUP元素*/
		node = xmlNewNode(NULL, BAD_CAST "GROUP");
		
		/*加入GROUP的PMIDS属性*/
		xmlNewProp(node,BAD_CAST"PMIDS",BAD_CAST _xpthis->sd->pmglist[i].pmids);

		/*加入GROUP的ID属性*/
		xmlNewProp(node,BAD_CAST"ID",BAD_CAST _xpthis->sd->pmglist[i].sid);
		
		xmlAddChild(root_node, node);
	}

	/////////////////////////////////////////////////////////////////
	/*物理机输出*/
	for(i=0;i<_xpthis->sd->pm_total;i++){
		
		/*生成PM元素*/
		node = xmlNewNode(NULL, BAD_CAST "PM");
		
		/*加入PM的STAT属性*/
 		sprintf(strBuf,"%d",(int)_xpthis->sd->pmlist[i].host.stat);
		xmlNewProp(node,BAD_CAST"STAT",BAD_CAST strBuf); 


		/*所在GROUP*/
		//sprintf(strBuf,"%d",(int)_xpthis->sd->pmlist[i].group_id);
		xmlNewProp(node,BAD_CAST"GR",BAD_CAST _xpthis->sd->pmlist[i].at_gr_ids);
		
		/*所在FRAME*/
		//sprintf(strBuf,"%d",(int)_xpthis->sd->pmlist[i].frame_id);
		xmlNewProp(node,BAD_CAST"FR",BAD_CAST _xpthis->sd->pmlist[i].at_fr_sid);
		
		/*加入PM的IP属性*/
		xmlNewProp(node,BAD_CAST"IP",BAD_CAST _xpthis->sd->pmlist[i].host.ip);
		
		
		
		/*加入PM的ID属性*/
		//itoa(_xpthis->sd->pmlist[i].host.id,strBuf,10);
		//sprintf(strBuf,"%d",(int)_xpthis->sd->pmlist[i].host.id);
		xmlNewProp(node,BAD_CAST"ID",BAD_CAST _xpthis->sd->pmlist[i].host.sid);
		

		xmlAddChild(root_node, node);

		/*生成METRIC元素*/
		for(j=0;j<DIM;j++){
			node1 = xmlNewNode(NULL,BAD_CAST"METRIC");
			
			/*指标值 预留*/
			xmlNewProp (node1,BAD_CAST"RESERVED",BAD_CAST DoubleToChar(_xpthis->sd->pmlist[i].host.load[j].reserved,2));
			/*指标值 总量*/
			xmlNewProp (node1,BAD_CAST"TOTAL",BAD_CAST DoubleToChar(_xpthis->sd->pmlist[i].host.load[j].total,2));
			/*指标值 已用量*/
			xmlNewProp (node1,BAD_CAST"USED",BAD_CAST DoubleToChar(_xpthis->sd->pmlist[i].host.load[j].used,2));
			
			/*新增METRIC属性enum MetricName {CPU = 0, MEM, DISKIO, FPGA, ETH0, ETH1, ETH2, ETH3, SUM, MAX, MIN};*/
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
	/*虚拟机输出*/
	for(i=0;i<_xpthis->sd->vm_total;i++){

		node = xmlNewNode(NULL, BAD_CAST "VM");
		//		xmlNewProp(node,BAD_CAST"STAT",BAD_CAST NumToChar(g_vmlist[i].stat));    没有stat的值
		/*所在PM*/		
		xmlNewProp(node,BAD_CAST"AT",BAD_CAST _xpthis->sd->pmlist[_xpthis->sd->vmlist[i].at].host.sid);

		
		/*加入VM的STAT属性*/
 		xmlNewProp(node,BAD_CAST"STAT",BAD_CAST NumToChar(_xpthis->sd->vmlist[i].host.stat));

		/*IP*/	
		xmlNewProp(node,BAD_CAST"IP",BAD_CAST _xpthis->sd->vmlist[i].host.ip);
	
		/*加入ID属性*/
		xmlNewProp(node,BAD_CAST"ID",BAD_CAST _xpthis->sd->vmlist[i].host.sid);
		

		xmlAddChild(root_node, node);

		for(j=0;j<DIM;j++){

			/*METRIC*/
			node1=xmlNewNode(NULL,BAD_CAST"METRIC");
			
			/*TOTAL属性*/
			xmlNewProp(node1,BAD_CAST"TOTAL",BAD_CAST DoubleToChar(_xpthis->sd->vmlist[i].host.load[j].total,2));
			
			/*USED属性*/
			xmlNewProp(node1,BAD_CAST"USED",BAD_CAST DoubleToChar(_xpthis->sd->vmlist[i].host.load[j].used,2));


			/*新增METRIC属性enum MetricName {CPU = 0, MEM, DISKIO, FPGA, ETH0, ETH1, ETH2, ETH3, SUM, MAX, MIN};*/
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

	/*保存输出到文件*/
	xmlSaveFormatFileEnc(xmlfilename, doc, "UTF-8", 1);
	
	/*释放*/
	xmlFreeDoc(doc);
	
	xmlCleanupParser();
	xmlMemoryDump();
}


/*
 * 函数名 : void xml_create_by_vm2vm()
 * 功能 : 将虚拟机之间的流量信息生成xml文档
 * 传入参数 : urlfile- xmlfilename
 * 返回值 :无
 * @created by 贾俊伟 in 2013.5.12
 * @Modified by Jianhai in 2013-05-13
 */

void xml_create_by_vm2vm(char *xmlfilename)
{
	int i;
	xmlDocPtr doc = NULL;       
	xmlNodePtr root_node = NULL, node = NULL, node1 = NULL;
	//建立根节点
	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "DATA");
	xmlNewProp(root_node,BAD_CAST"VERSION",BAD_CAST"0.1");

	xmlDocSetRootElement(doc, root_node);

	/*对没一条亲和性关系输出到XML*/
	for(i=0;i<_xpthis->sd->v2v_total;i++){

		/*生成主节点*/
		node = xmlNewNode(NULL, BAD_CAST "AFFINITY");

		/*生成AT属性*/
		xmlNewProp(node,BAD_CAST"AT",BAD_CAST _xpthis->sd->v2vlist[i].at);
		/*生成TYPE属性*/
		xmlNewProp(node,BAD_CAST"TYPE",BAD_CAST NumToChar (_xpthis->sd->v2vlist[i].affinity));

		/*生成TRAFFIC属性*/
		if(_xpthis->sd->v2vlist[i].traffic>0)
			xmlNewProp(node,BAD_CAST"TRAFFIC",BAD_CAST DoubleToChar (_xpthis->sd->v2vlist[i].traffic,0));
		else
			xmlNewProp(node,BAD_CAST"TRAFFIC",BAD_CAST " ");

		/*生成DELAY属性*/
		xmlNewProp(node,BAD_CAST"DELAY",BAD_CAST DoubleToChar (_xpthis->sd->v2vlist[i].delay,0));

		/*生成VMIDS属性*/
		xmlNewProp(node,BAD_CAST"VMIDS",BAD_CAST _xpthis->sd->v2vlist[i].vmids);

		/*
		xmlNewProp(node,BAD_CAST"DSTID",BAD_CAST NumToChar(_xpthis->sd->v2vlist[i].p_dst_vm->host.id));
		xmlNewProp(node,BAD_CAST"SRCID",BAD_CAST NumToChar(_xpthis->sd->v2vlist[i].p_src_vm->host.id));
		*/

		xmlAddChild(root_node, node);
	}

	/*生成文件*/
	xmlSaveFormatFileEnc(xmlfilename, doc, "UTF-8", 1);
	
	/*释放*/
	xmlFreeDoc(doc);
	xmlCleanupParser();

	xmlMemoryDump();
}


/*生成网络拓扑XML21 cjh add 7.28*/
void xml_create_by_pm2pm_treetopo(char *xmlfilename)
{
	int i;
	pm2swPtr p_ps;
	sw2swPtr p_ss;
	swPtr p_sw;
	xmlDocPtr doc = NULL;       
	xmlNodePtr root_node = NULL, node = NULL, node1 = NULL;
	//建立根节点
	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "DATA");
	xmlNewProp(root_node,BAD_CAST"VERSION",BAD_CAST"0.1");

	xmlDocSetRootElement(doc, root_node);

	/*---1.生成SWITCH列表---------------*/
	

	/*对每一个SWITHC输出到XML21*/
	for(i=0;i<_xpthis->sd->sw_total;i++){
		
		p_sw=&_xpthis->sd->swlist[i];
		
		/*生成id属性*/
		xmlNewProp(node,BAD_CAST"ID",BAD_CAST p_sw->sid);

		//printf(" ---%s ---\n",p_sw->sid);

		/*生成主节点*/
		node = xmlNewNode(NULL, BAD_CAST "SW");

		/*生成LEVEL属性*/
		xmlNewProp(node,BAD_CAST"LEV",BAD_CAST NumToChar(p_sw->level));

		/*端口个数属性*/
		xmlNewProp(node,BAD_CAST"PORTNUM",BAD_CAST "24");
		
		xmlAddChild(root_node, node);
	
	}


	/*对每一个PM2SW连接关系输出到XML21*/
	
	for(i=0;i<_xpthis->sd->p2s_total;i++){
		p_ps=&_xpthis->sd->p2slist[i];
		/*生成主节点*/
		node = xmlNewNode(NULL, BAD_CAST "P2S");

		
	
		
		/*生成delay属性*/
		xmlNewProp(node,BAD_CAST"DELAY",BAD_CAST DoubleToChar(p_ps->delay,0));

		/*生成BANDWIDTH属性*/
		if(_xpthis->sd->p2slist[i].bandwidth>0)
			xmlNewProp(node,BAD_CAST"BW",BAD_CAST DoubleToChar (p_ps->bandwidth,0));
		else
			xmlNewProp(node,BAD_CAST"BW",BAD_CAST "");
		
		
		/*生成DST: SWITCH SID属性*/
		xmlNewProp(node,BAD_CAST"DPORT",BAD_CAST NumToChar(p_ps->swithch_port));


		/*生成DST: SWITCH SID属性*/
		xmlNewProp(node,BAD_CAST"DST",BAD_CAST p_ps->switch_sid);

		/*生成SRC: PMSID属性*/
		xmlNewProp(node,BAD_CAST"SPORT",BAD_CAST NumToChar( p_ps->port_id));

		/*生成SRC: PMSID属性*/
		xmlNewProp(node,BAD_CAST"SRC",BAD_CAST p_ps->pm_sid);


		/*生成ID属性:PS(pm2sw), SS(sw2sw)*/
		xmlNewProp(node,BAD_CAST"ID",BAD_CAST NumToChar( p_ps->id));
		

		xmlAddChild(root_node, node);
	}


	/*对每一个SW2SW连接关系输出到XML21*/
	for(i=0;i<_xpthis->sd->s2s_total;i++){
		p_ss=&_xpthis->sd->s2slist[i];
		/*生成主节点*/
		node = xmlNewNode(NULL, BAD_CAST "S2S");

		
		/*生成delay属性*/
		xmlNewProp(node,BAD_CAST"DELAY",BAD_CAST DoubleToChar(p_ss->delay,0));

		/*生成BANDWIDTH属性*/
		if(_xpthis->sd->s2slist[i].bandwidth>0)
			xmlNewProp(node,BAD_CAST"BW",BAD_CAST DoubleToChar (p_ss->bandwidth,0));
		else
			xmlNewProp(node,BAD_CAST"BW",BAD_CAST "");


		/*生成DST: SWITCH SID属性*/
		xmlNewProp(node,BAD_CAST"DST",BAD_CAST p_ss->p_dst_sw->sid);

		/*生成SRC: PMSID属性*/
		xmlNewProp(node,BAD_CAST"SRC",BAD_CAST p_ss->sw_src_sid);

		/*生成ID属性:PS(pm2sw), SS(sw2sw)*/
		xmlNewProp(node,BAD_CAST"ID",BAD_CAST NumToChar(p_ss->id));
		
		xmlAddChild(root_node, node);
	}



	/*生成文件*/
	xmlSaveFormatFileEnc(xmlfilename, doc, "UTF-8", 1);
	
	/*释放*/
	xmlFreeDoc(doc);
	xmlCleanupParser();

	xmlMemoryDump();

}



void xml_create_by_pm2pm(char *filename)
{
	return;

}

/*生成ACTIONS XML列表*/
void xml_create_by_actions(char *filename)
{
	int i,action;
	char idstr[30],ast[30];
	xmlDocPtr doc = NULL;       
	xmlNodePtr root_node = NULL, node = NULL, node1 = NULL;
	//建立根节点
	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "ACTION");
	xmlNewProp(root_node,BAD_CAST"VERSION",BAD_CAST"0.1");

	xmlDocSetRootElement(doc, root_node);

	for(i=0;i<_xpthis->sd->action_total;i++){

		action=_xpthis->sd->actionlist->action;
		switch (action) /*0-PLACE,1-MIGRATE,2-DOWN,3-UP*/
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
			break;
		case 2:
			break;
		default:
			break;
		}
		
		xmlAddChild(root_node, node);
	}

	/*生成文件*/
	xmlSaveFormatFileEnc(filename, doc, "UTF-8", 1);
	
	/*释放*/
	xmlFreeDoc(doc);
	xmlCleanupParser();

	xmlMemoryDump();
}

/******************************************************************************************/

	
/****2.读取XML数据通用函数******************************************************/

/*读取INT类型的属性值 CJH add 7.30*/
int getint_attrib(xmlTextReaderPtr reader, char *fldname)
{
	/* handling of a node in the tree */
	xmlChar *att;
	char *endptr;
	int aValue=0;
	
	att=xmlTextReaderGetAttribute(reader,(xmlChar *)fldname);
	if(att!=NULL) {
		aValue = (int)strtod(att,&endptr);
			
		if(DEBUG) printf("%s: %d ",fldname,aValue);
	}
	xmlFree(att);

	return aValue;
}


			
/*读取DOUBLE类型的属性值 CJH add 7.30*/
double getdbl_attrib(xmlTextReaderPtr reader,char *fldname)
{
	/* handling of a node in the tree */
	xmlChar *att;
	char *endptr;
	double aValue=0;
	
	att=xmlTextReaderGetAttribute(reader,(xmlChar *)fldname);
	if(att!=NULL) {
		aValue = strtod(att,&endptr);
		
		if(DEBUG) printf("%s: %.2f ",fldname,aValue);
	}
	xmlFree(att);

	return aValue;
}

//
/*读取DOUBLE类型的属性值 CJH add 7.30*/
void getstr_attrib(xmlTextReaderPtr reader,char *fldname, char *ret)
{
	/* handling of a node in the tree */
	xmlChar *att;
	
	att=xmlTextReaderGetAttribute(reader,(xmlChar *)fldname);
	if(att!=NULL) {
		strcpy(ret,(char *)att);
		
		if(DEBUG) printf("%s: %s ",fldname,att);
	}
	xmlFree(att);

}

/******************************************************************************************/

//Process every node of XML1 file to get values 
//处理XML1每个节点获取相关值
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

	int kk=-1;//用kk记录请求的索引值
	int iPara=-1;

	kk=*reqcount;
	iPara=*algparacount;

    name = xmlTextReaderName(reader);
    if (name == NULL)
        name = xmlStrdup(BAD_CAST "--");
	nt=xmlTextReaderNodeType(reader);

	/*如果元素名称是REQUEST，则获取属性ID,TYPE，ENABLE等值*/
	if (xmlStrcmp(name, (xmlChar *) "REQUEST")==0) {
		if(nt==XML_READER_TYPE_ELEMENT) 
		{
			kk++;iPara=0;
			p_req=&_xpthis->sd->reqlist[kk];
			p_req->id=kk;
			
			if(DEBUG) printf("Request %d ",kk);
			
			getstr_attrib(reader,"TYPE",p_req->type);

			p_req->enable=getint_attrib(reader,"ENABLE");
		}
	}

	/*如果元素名称是ALGORITHM，则获取属性name等值*/
	if (xmlStrcmp(name, (const xmlChar *) "ALGORITHM")==0) {
		if(nt==XML_READER_TYPE_ELEMENT) 
		{
			iPara=0;
			p_req=&_xpthis->sd->reqlist[kk];
			getstr_attrib(reader,"NAME",p_req->alg.algName);

		}
		else if(nt==XML_READER_TYPE_END_ELEMENT) /*</ALGORITHM>*/
		{
			_xpthis->sd->reqlist[kk].alg.paracount=iPara; /*参数个数*/
		}

	}

	/*如果元素名称是ALGORITHM，则获取属性name等值*/
	if (xmlStrcmp(name, (const xmlChar *) "PARAMETER")==0) {
		if(nt==XML_READER_TYPE_ELEMENT) 
		{
			p_req=&_xpthis->sd->reqlist[kk];
			getstr_attrib(reader,"NAME",p_req->alg.para[iPara].para_name);

			//att=xmlTextReaderGetAttribute(reader,"NAME");
			//strcpy(_xpthis->sd->reqlist[kk].alg.para[iPara].para_name,att);
			
			getstr_attrib(reader,"TYPE",p_req->alg.para[iPara].para_type);

			//att1=xmlTextReaderGetAttribute(reader,"TYPE");
			//strcpy(_xpthis->sd->reqlist[kk].alg.para[iPara].para_type,att1);
			
			p_req->alg.para[iPara].para_value=getdbl_attrib(reader,"VALUE");

			//att2=xmlTextReaderGetAttribute(reader,"VALUE");
			//strcpy(_xpthis->sd->reqlist[kk].alg.para[iPara].para_value,att2);

			//if(DEBUG)
				//printf("PARAMETER%d--(NAME:%s, TYPE:%s, VALUE:%s)\n",iPara,att,att1,att2);

			//xmlFree(att);
			//xmlFree(att1);
			//xmlFree(att2);

			iPara++; /*参数个数增加1*/
		}
	}
	
	*algparacount=iPara;

	*reqcount=kk;//set request count
    xmlFree(name);  
}


/*----------------------------------------------------------------------*/

/*Read values from XML2 file 读取XML2文件*/
static int get_request_from_xml1(char *filename) 
{
    xmlTextReaderPtr reader;
	xmlChar *name;
    
	int reqc=-1,parac=-1;
	int ret=0;
	
    reader = xmlNewTextReaderFilename(filename);
	
    if (reader != NULL) {
        ret = xmlTextReaderRead(reader);
		
		name = xmlTextReaderName(reader);/*获取reader的元素名称*/
		if (name == NULL)
			name = xmlStrdup(BAD_CAST "--");
		
		/*在这个例子中，我们需要确认文档是正确的类型。“DATA”是在这个示例中使用文档的根类型。*/
		if (xmlStrcmp(name, (const xmlChar *) "CONTROLOR")) {
			fprintf(stderr,"document of the wrong type, root node != CONTROLOR");
			xmlFree(name);
			xmlFreeTextReader(reader);
			return ret;
		}
		
		
        while (ret == 1) {/*处理每个节点*/
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
    
	
	/*更新共享数据*/
	_xpthis->sd->req_total=reqc+1;
	
	

	return ret;
}



/******************************************************************************************/

/******************************************************************************************/
/***********************************************************************/
/*
* 函数名 : process_XML2_Node
* 功能 : 处理XML2每个节点获取相关值
* 传入参数 : xmlTextReaderPtr reader,int *pmcount,int *vmcount,int *grcount,int *frcount
* 参数reader : XML读入的数据内容
* 参数pmcount : 物理机个数
* 参数vmcount : 虚拟机的数量
* 参数grcount : 物理机组的个数
* 参数frcount : 框的个数
* 返回值 : 无
* @created by 陈建海 in 2013.5.12
*/
void process_XML2_Node(xmlTextReaderPtr reader, 
					   int *pmcount,int *vmcount,
					   int *grcount,int *frcount) 
{
 
	xmlChar *name, *att;
	char *endptr,iput[20];
	int rr,nt;	
	double aValue;
	framePtr p_fr;
	pmPtr p_pm; vmPtr p_vm;
	pmgPtr p_pmg;
	cls_st_host hs;

	int kk=-1,ll=-1,ff=-1,gg=-1;//用kk记录物理机的索引值

	kk=*pmcount;
	ll=*vmcount;
	ff=*frcount;
	gg=*grcount;

    name = xmlTextReaderName(reader);
    if (name == NULL)
        name = xmlStrdup(BAD_CAST "--");
	nt=xmlTextReaderNodeType(reader);

	
	/*如果元素名称是FRAME，则获取属性ID，NAME, PMIDS等值*/
	if (xmlStrcmp(name,  (const xmlChar *) "FRAME")==0) {
		if(nt==XML_READER_TYPE_ELEMENT) 
		{
			ff++;

			p_fr=&_xpthis->sd->framelist[ff];
			p_fr->id=ff;//置序号

			//获取框的SID
			getstr_attrib(reader,"ID",p_fr->sid);

			//获取框的所有物理机ID
			getstr_attrib(reader,"PMIDS",p_fr->pmids);

		}
	}

	
	/*如果元素名称是GROUP，则获取属性ID，NAME, PMIDS等值*/
	if (xmlStrcmp(name, (const xmlChar *) "GROUP")==0) {
		if(nt==XML_READER_TYPE_ELEMENT) 
		{
			gg++;
			p_pmg =&_xpthis->sd->pmglist[gg];
			p_pmg->id=gg;//置序号
			

			/*获取组的SID*/
			getstr_attrib(reader,"ID",p_pmg->sid);

			//获取zu的所有物理机ID
			getstr_attrib(reader,"PMIDS",p_pmg->pmids);

		}
	}


	/*如果元素名称是PM，则获取属性ID，IP,STAT等值*/
	if (xmlStrcmp(name, (const xmlChar *) "PM")==0) {
		if(nt==XML_READER_TYPE_ELEMENT) 
		{
			kk++;
			p_pm=&_xpthis->sd->pmlist[kk];
			p_pm->host.id=kk;

			/*获取PM的SID*/
			getstr_attrib(reader,"ID",p_pm->host.sid);

			/*获取PM的IP*/
			getstr_attrib(reader,"IP",p_pm->host.ip);
			
			//获取PM状态
			p_pm->host.stat=getint_attrib(reader,"STAT");

			if(DEBUG) printf("\n----\n");
		}
		//return;
	}
	

	/*如果元素名称是VM，则获取属性ID，IP,STAT等值*/
	if (xmlStrcmp(name, (const xmlChar *) "VM")==0) {
		
		if(nt==XML_READER_TYPE_ELEMENT) 
		{
			ll++;
			
			p_vm=&_xpthis->sd->vmlist[ll];
			p_vm->host.id=ll;
			
			//获取主机Id
			getstr_attrib(reader,"ID",p_vm->host.sid);
			
			//获取主机IP
			getstr_attrib(reader,"IP",p_vm->host.ip);
			
			//获取主机的状态
			p_vm->host.stat=getint_attrib(reader,"STAT");


			//获取VM所在PM
			
			att=xmlTextReaderGetAttribute(reader,"AT");
			if(att!=NULL) {
				/*获得的是pm0，需要获取相应的pmid*/
				initHost(&hs);
				hs.sd = _xpthis->sd;
				
				p_pm=hs.getpmbysid(att);

				if(p_pm!=NULL)
					_xpthis->sd->vmlist[ll].at=p_pm->host.id;

				if(DEBUG) printf("atpm: %s ",att);
			}
			xmlFree(att);
			

			if(DEBUG) printf("\n-----\n");	
		}
	}

	/*如果元素名称是METRIC，则获取属性NAME，USED,TOTAL, RESERVED等值*/
	if (xmlStrcmp(name, (const xmlChar *) "METRIC")==0) {
		
		att=xmlTextReaderGetAttribute(reader,"NAME");		
		//{ CPU = 0, MEM, DISKIO, FPGA, ETH0, ETH1, ETH2, ETH3 };
		rr=0;

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
		
		//get the attribute value metricname
		if(ll==-1)
			strcpy(_xpthis->sd->pmlist[kk].host.load[rr].metric_name,att);
		else
			strcpy(_xpthis->sd->vmlist[ll].host.load[rr].metric_name,att);

		if(DEBUG) printf(" %s ",att); 

		xmlFree(att);
		
		//get the attribute value used
		att=xmlTextReaderGetAttribute(reader,"USED");
		if(att!=NULL) {
			//strcpy(iput,att);
			aValue = strtod(att,&endptr);

			if(ll==-1)//PM
			{
				_xpthis->sd->pmlist[kk].host.load[rr].used=aValue;
				if(DEBUG) printf("used: %.3f ",_xpthis->sd->pmlist[kk].host.load[rr].used);
			}
			else
			{
				_xpthis->sd->vmlist[ll].host.load[rr].used=aValue;
				if(DEBUG) printf("used: %.3f ",_xpthis->sd->vmlist[ll].host.load[rr].used);
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
				if(DEBUG) printf("total: %.3f ",_xpthis->sd->pmlist[kk].host.load[rr].total);
			}
			else
			{
				_xpthis->sd->vmlist[ll].host.load[rr].total=aValue;
				if(DEBUG) printf("total: %.3f ",_xpthis->sd->vmlist[ll].host.load[rr].total);
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
				if(DEBUG) printf("reserved: %.3f ",_xpthis->sd->pmlist[kk].host.load[rr].reserved);
			}
			else
			{
				_xpthis->sd->vmlist[ll].host.load[rr].reserved=aValue;
				if(DEBUG) printf("reserved: %.3f ",_xpthis->sd->vmlist[ll].host.load[rr].reserved);
			}			
		}
		xmlFree(att);
		if(DEBUG) printf("\n");
	}
	

	*pmcount=kk;//set PM count
	*vmcount=ll; //set VM count
	*frcount=ff;
	*grcount=gg;

    xmlFree(name);  
}

/*Read values from XML2 file 读取XML2文件*/
static int get_vmpm_from_xml2(char *filename) 
{
    xmlTextReaderPtr reader;
	xmlChar *name;
    
	int pc=-1,vc=-1,gc=-1,fc=-1;

	int ret=0;

	cls_st_frame cls_fr;
	cls_st_pmgroup cls_pmg;

	initframe(&cls_fr);
	initPmgroup(&cls_pmg);
	cls_fr.sd = _xpthis->sd;
	cls_pmg.sd = _xpthis->sd;
	
    reader = xmlNewTextReaderFilename(filename);
	
    if (reader != NULL) {
        ret = xmlTextReaderRead(reader);
		
		name = xmlTextReaderName(reader);/*获取reader的元素名称*/
		if (name == NULL)
			name = xmlStrdup(BAD_CAST "--");
		
		/*在这个例子中，我们需要确认文档是正确的类型。“DATA”是在这个示例中使用文档的根类型。*/
		if (xmlStrcmp(name,  (const xmlChar *) "DATA")) {
			fprintf(stderr,"document of the wrong type, root node != DATA");
			xmlFree(name);
			xmlFreeTextReader(reader);
			return ret;
		}
		
		 

        while (ret == 1) {
           process_XML2_Node(reader, &pc,&vc,&gc,&fc);
				_xpthis->sd->pm_total=pc+1;
				_xpthis->sd->vm_total=vc+1;
				_xpthis->sd->gr_total=gc+1;
				_xpthis->sd->fr_total=fc+1;

            ret = xmlTextReaderRead(reader);
	    }
        
		xmlFreeTextReader(reader);

        if (ret != 0) {
            printf("%s : failed to parse\n", filename);
        }
    
	} 
	else 
	    printf("Unable to open %s\n", filename);
    
	
	/*更新共享数据*/
	_xpthis->sd->pm_total=pc+1;
	_xpthis->sd->vm_total=vc+1;
	_xpthis->sd->gr_total=gc+1;
	_xpthis->sd->fr_total=fc+1;

	cls_fr.get_frame_pmids();
	cls_pmg.get_group_pmids();

	return ret;
}



/*************************************************************************/
//Process every node of XML21 file to get values 
//处理XML21每个节点获取相关值 pm2pm
void process_XML21_Node(xmlTextReaderPtr reader, int *swcount,int *pscount,int *sscount) 
{
    /* handling of a node in the tree */
	xmlChar *name;
	//char *endptr;
	int nt;	

	int swc=-1;//用vd记录流量关系的索引值
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
	/*1.读取交换机信息：如果元素名称是SW，则获取属性ID，PORTNUM,LEV等值*/
	if (xmlStrcmp(name, (xmlChar *) "SW")==0) {
		
		if(nt==XML_READER_TYPE_ELEMENT) swc++;

		/*读取ID*/
		getstr_attrib(reader,(xmlChar*)"ID",p_sw->sid);
		
		p_sw->portnum=getint_attrib(reader,"PORTNUM");

		p_sw->level=getint_attrib(reader,"LEV");

	}

	/*****************************************************/
	/*2.读取交换机与物理机的连接信息：如果元素名称是SW，则获取属性ID，PORTNUM,LEV等值*/
	if (xmlStrcmp(name, (xmlChar *) "P2S")==0) {
		
		if(nt==XML_READER_TYPE_ELEMENT) psc++;

		/*读取ID*/
		p_ps->id=getint_attrib(reader,"ID");
		/*读取SRC为PMID*/
		getstr_attrib(reader,"SRC",p_ps->pm_sid);
		/*读取端口号*/
		p_ps->port_id=getint_attrib(reader,"SPORT");
		/*读取目标交换机SID*/
		getstr_attrib(reader,"DST",p_ps->switch_sid);
		/*读取端口号*/
		p_ps->swithch_port=getint_attrib(reader,"DPORT");

		p_ps->bandwidth=(float)getdbl_attrib(reader,"BW"); 
		p_ps->delay=(float)getdbl_attrib(reader,"DELAY");

	}


	
	/*****************************************************/
	/*2.读取交换机与物理机的连接信息：如果元素名称是SW，则获取属性ID，PORTNUM,LEV等值*/
	if (xmlStrcmp(name, (xmlChar *) "S2S")==0) {
		
		if(nt==XML_READER_TYPE_ELEMENT) ssc++;

		/*读取ID*/
		p_ps->id=getint_attrib(reader,"ID");
		/*读取SRC为PMID*/
		getstr_attrib(reader,"SRC",p_ss->sw_src_sid);
		/*读取目标交换机SID*/
		getstr_attrib(reader,"DST",p_ss->sw_dst_sid);
		/*读取带宽*/
		p_ps->bandwidth=(float)getdbl_attrib(reader,"BW"); 
		/*读取时延*/
		p_ps->delay=(float)getdbl_attrib(reader,"DELAY");
	}


	printf("\n");

	
	
	*swcount=swc;//set p2p count
	*pscount=psc;
	*sscount=ssc;
	//
	///*p2p总数*/
	_xpthis->sd->sw_total=*swcount;
	_xpthis->sd->p2s_total=*pscount;
	_xpthis->sd->s2s_total=*sscount;

	xmlFree(name);  

	return;
}

/*读取XML 物理机拓扑结构 CJH ADD 7.29*/
/*Read values from XML21 file 读取XML21，pm2pm物理机之间的时延带宽信息文件*/
int get_pm2pm_from_xml21(char *filename)
{

    xmlTextReaderPtr reader;
	xmlChar *name;
    
	int swc=0,psc=0,ssc=0;

	int ret=0;
	
    reader = xmlNewTextReaderFilename(filename);
	
    if (reader != NULL) {
        ret = xmlTextReaderRead(reader);
		
		name = xmlTextReaderName(reader);/*获取reader的元素名称*/
		if (name == NULL)
			name = xmlStrdup(BAD_CAST "--");
		
		/*在这个例子中，我们需要确认文档是正确的类型。“DATA”是在这个示例中使用文档的根类型。*/
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
    
	_xpthis->sd->sw_total=swc; /*更新共享数据的全局变量*/
	_xpthis->sd->p2s_total=psc;
	_xpthis->sd->s2s_total=ssc;

	ret=swc+1;

	return ret;

}


/***********************************************************************/
/*
* 函数名 : process_XML22_Node
* 功能 : 处理XML22每个节点获取相关值
* Process every node of XML22 file to get values  to v2vlist
* 处理XML22每个节点获取相关值

* 传入参数 : xmlTextReaderPtr reader,int *VDcount
* 参数reader : XML读入的数据内容
* 参数vdcount : 物理机个数
* 返回值 : 无
* @created by 陈建海 in 2013.5.12
*/
/*----------------------------------------------------------------------*/

void process_XML22_Node(xmlTextReaderPtr reader, int *vdcount) 
{
    /* handling of a node in the tree */
	xmlChar *name, *att;
	char *endptr;
	char *strvm[MAX_VM_COUNT_PER_GROUP];
	char t_vmids[PMIDS_SIZE];
	int nt,k;	
	double aValue;
	
	
	int vd=-1;//用vd记录流量关系的索引值

	vd=*vdcount;
	name = xmlTextReaderName(reader);
    if (name == NULL)
        name = xmlStrdup(BAD_CAST "--");
	nt=xmlTextReaderNodeType(reader);

	/*如果元素名称是AFFINITY，则获取属性VMIDS，TRAFFIC,DELAY,TYPE,AT等值*/
	if (xmlStrcmp(name, (xmlChar *) "AFFINITY")==0) {
		if(nt==XML_READER_TYPE_ELEMENT) vd++;

		//AFFTYPE
		att=xmlTextReaderGetAttribute(reader,"TYPE");
		if(att!=NULL)
		{
			aValue = strtod(att,&endptr);
			_xpthis->sd->v2vlist[vd].affinity=(int)aValue;
			_xpthis->sd->v2vlist[vd].affinity=(int)aValue;
		
			if(DEBUG) printf("type:%3d ",_xpthis->sd->v2vlist[vd].affinity);
		}	
		xmlFree(att);

		
		//TRAFFIC
		att=xmlTextReaderGetAttribute(reader,"TRAFFIC");
		if(att!=NULL)
		{
			aValue = strtod(att,&endptr);
			_xpthis->sd->v2vlist[vd].traffic=aValue;
			_xpthis->sd->v2vlist[vd].traffic=aValue;
		
			printf("TRAFFIC:%5.f ",_xpthis->sd->v2vlist[vd].traffic);
		}	
		xmlFree(att);

		//DELAY 时延要求
		att=xmlTextReaderGetAttribute(reader,"DELAY");
		if(att!=NULL)
		{
			aValue = strtod(att,&endptr);
			_xpthis->sd->v2vlist[vd].delay=aValue;
			
			printf("DELAY:%4.f ",_xpthis->sd->v2vlist[vd].delay);
		}
		xmlFree(att);
	
		
		//AT
		att=xmlTextReaderGetAttribute(reader,"AT");
		if(att!=NULL)
		{
			strcpy(_xpthis->sd->v2vlist[vd].at,att);
			strcpy(_xpthis->sd->v2vlist[vd].at,att);		
			printf("AT: %6s ",_xpthis->sd->v2vlist[vd].at);
		}	
		xmlFree(att);

		/*VMIDS*/
		att=xmlTextReaderGetAttribute(reader,"VMIDS");
		if(att!=NULL) {
			strcpy(_xpthis->sd->v2vlist[vd].vmids,att);
			printf("VMIDS:%s ",_xpthis->sd->v2vlist[vd].vmids);			
		}
		xmlFree(att);

		/*从vmids处理得到p_vms*/
		{
			cls_st_host hh;
			initHost(&hh);
			hh.sd = _xpthis->sd;
			strcpy(t_vmids,_xpthis->sd->v2vlist[vd].vmids);
				_xpthis->sd->v2vlist[vd].countofvm=split_str(t_vmids,",",strvm);
			
			for(k=0;k<_xpthis->sd->v2vlist[vd].countofvm;k++)
				_xpthis->sd->v2vlist[vd].p_vms[k]=hh.getvmbysid(strvm[k]); 
		
		}	

		
		att=xmlTextReaderGetAttribute(reader,"SRCID");
		if(att!=NULL) {
			aValue = strtod(att,&endptr);			
			_xpthis->sd->v2vlist[vd].p_src_vm=&_xpthis->sd->vmlist[(int)aValue];
			printf("nodetype :%d, id: %d ",nt,_xpthis->sd->v2vlist[vd].p_src_vm->host.id);			
		}
		xmlFree(att);

		att=xmlTextReaderGetAttribute(reader,"DSTID");
		if(att!=NULL) {
			aValue = strtod(att,&endptr);
			_xpthis->sd->v2vlist[vd].p_dst_vm=&_xpthis->sd->vmlist[(int)aValue];
			
			printf("DSTID: %s ",_xpthis->sd->v2vlist[vd].p_dst_vm->host.sid);
		}
		xmlFree(att);
				

		
		printf("\n");
		//return;
	}
	
	*vdcount=vd;//set v2v count
	xmlFree(name);  
}



/*Read values from XML22 file 读取XML22，虚拟机之间的流量信息文件*/
int get_vm2vm_from_xml22(char *filename) 
{
    xmlTextReaderPtr reader;
	xmlChar *name;
    
	int vd=-1,ret=0;
	
    reader = xmlNewTextReaderFilename(filename);
	
    if (reader != NULL) {
        ret = xmlTextReaderRead(reader);
		
		name = xmlTextReaderName(reader);/*获取reader的元素名称*/
		if (name == NULL)
			name = xmlStrdup(BAD_CAST "--");
		
		/*在这个例子中，我们需要确认文档是正确的类型。“DATA”是在这个示例中使用文档的根类型。*/
		if (xmlStrcmp(name, (xmlChar *) "DATA")) {
			fprintf(stderr,"document of the wrong type, root node != DATA");
			xmlFree(name);
			xmlFreeTextReader(reader);
			return ret;
		}		
		
        while (ret == 1) {
            process_XML22_Node(reader,&vd);
			
            ret = xmlTextReaderRead(reader);
	    }
        
		xmlFreeTextReader(reader);

        if (ret != 0) {
            printf("%s : failed to parse\n", filename);
        }
    
	} 
	else 
	    printf("Unable to open %s\n", filename);
    
	_xpthis->sd->v2v_total=vd+1;/*在全局共享数据区记录v2v的总数*/
	
	return ret;
}
