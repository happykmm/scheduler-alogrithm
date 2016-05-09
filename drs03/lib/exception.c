#include "exception.h"   
  
Except_frame *Except_stack = NULL;   
  
void except_raise(const Except_t *e,const char *file,int line)   
{   
     Except_frame *p = Except_stack;   
        
     assert(e);   
     if(p == NULL){   
          abort_without_exception(e,file,line);   
     }   
     p->exception = e;   
     p->file = file;   
     p->line = line;   
     Except_stack = Except_stack->prev;   
     longjmp(p->env,EXCEPT_RAISED);   
 }   
  
void abort_without_exception(const Except_t *e,const char *file,int line)   
{   
     fprintf(stderr,"Uncaught exception");   
     if(e->reason)   
         fprintf(stderr," %s",e->reason);   
     else  
         fprintf(stderr," at 0x%p",e);   
        
     if (file && line > 0)   
         fprintf(stderr, "raised at %s:%d\n",file,line);   
     fprintf(stderr,"aborting...\n");   
     fflush(stderr);   
     abort();   
}  
