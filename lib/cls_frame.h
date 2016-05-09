#ifndef CLS_FRAME_H
#define CLS_FRAME_H

#include "drsdata.h"
#include "drsdef.h"
#include "drslib.h"
#include "util.h"

typedef struct {
	st_frame frame;	
	st_share_data *sd;

	void (*sid2ids)(int frame_id);
	void (*get_frame_pmids)();
	framePtr (*getfrbysid)(char *sid);


} cls_st_frame;

typedef cls_st_frame *clsstframePtr;
clsstframePtr _fthis;

void initframe(clsstframePtr THIS);
void fr_sid2ids(int frame_id);
void get_frame_pmids();
framePtr getfrbysid(char *sid);

#endif