#ifndef __EXCEPTION_H__   
#define __EXCEPTION_H__   
  
#include   <stdio.h>
#include   <stdlib.h> 
#include   <setjmp.h>
  
  
#define T Except_t   
typedef struct Except_t{   
        char *reason;   
}Except_t;   
           
typedef struct Except_frame{   
        struct Except_frame *prev;   
        jmp_buf env;   
        const char *file;   
        int line;   
        const T* exception;   
}Except_frame;   
  
extern Except_frame *Except_stack;      //ȫ�ֱ���    
  
//�쳣��״̬����    
enum {EXCEPT_ENTERED=0,EXCEPT_RAISED,   
     EXCEPT_HANDLED,EXCEPT_FINALIZED};    
  
#define throw(e) except_raise(&(e),__FILE__,__LINE__)   
  
#define rethrow except_raise(except_frame.exception,except_frame.file,except_frame.line)   
                      
void abort_without_exception(const Except_t *e,const char *file,int line);   
  
//��ջ��Ԫ�ش�ջ�е����������׳�    
void except_raise(const T *e,const char *file,int line);   
              
//try�����þ��ǽ�һ��������������env��except_frameѹ��Except_stackջ�С�   
//����except_flagΪsetjmp�ķ���ֵ����ʾ�쳣��״̬    
#define try do{ 
			volatile int except_flag; 
            Except_frame except_frame;
			except_frame.prev = Except_stack; 
            Except_stack = &except_frame; 
            except_flag = setjmp(except_frame.env); 
            if (except_flag == EXCEPT_ENTERED)    
            {   
  
//����͸ո�ѹ��Except_stack�е�except_frame��Ӧ��longjmp��������   
//�ͽ����ջ���浯����������������Ļ����ͻָ��������������   
//����Ļ������ص�setjmp()��λ�����½��д�����ʱ��except_flag   
//��ֵΪEXCEPT_RAISED    
#define catch(e) \   
                if(except_flag == EXCEPT_ENTERED) \   
                    Except_stack = Except_stack->prev; \   
            }else if(except_frame.exception == &(e)){ \   
                except_flag = EXCEPT_HANDLED;   
  
#define try_return \   
                   switch(Except_stack = Except_stack->prev,0) \   
                       default: return    
  
#define catch_else \   
                   if(except_flag == EXCEPT_ENTERED) \   
                       Except_stack = Except_stack->prev; \   
                   }else{ \   
                     except_flag = EXCEPT_HANDLED;   
  
//���û����صĴ������������׳�    
#define end_try \   
                if(except_flag == EXCEPT_ENTERED) \   
                    Except_stack = Except_stack->prev; \   
                } \   
                if (except_flag == EXCEPT_RAISED) \   
                    except_raise(except_frame.exception, \   
                        except_frame.file,except_frame.line); \   
                }while(0)   
                   
                   
#define finally \   
                if(except_flag == EXCEPT_ENTERED) \   
                    Except_stack = Except_stack->prev; \   
                }{ \   
                    if(except_flag == EXCEPT_ENTERED) \   
                        except_flag = EXCEPT_FINALIZED;    
  
#undef T   
#endif  
