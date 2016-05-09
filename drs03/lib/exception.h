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
  
extern Except_frame *Except_stack;      //全局变量    
  
//异常的状态常量    
enum {EXCEPT_ENTERED=0,EXCEPT_RAISED,   
     EXCEPT_HANDLED,EXCEPT_FINALIZED};    
  
#define throw(e) except_raise(&(e),__FILE__,__LINE__)   
  
#define rethrow except_raise(except_frame.exception,except_frame.file,except_frame.line)   
                      
void abort_without_exception(const Except_t *e,const char *file,int line);   
  
//将栈顶元素从栈中弹出，重新抛出    
void except_raise(const T *e,const char *file,int line);   
              
//try的作用就是将一个包含环境变量env的except_frame压入Except_stack栈中。   
//其中except_flag为setjmp的返回值，表示异常的状态    
#define try do{ 
			volatile int except_flag; 
            Except_frame except_frame;
			except_frame.prev = Except_stack; 
            Except_stack = &except_frame; 
            except_flag = setjmp(except_frame.env); 
            if (except_flag == EXCEPT_ENTERED)    
            {   
  
//如果和刚刚压入Except_stack中的except_frame对应的longjmp不发生，   
//就将其从栈里面弹出来，而如果发生的话，就恢复这个环境变量所   
//保存的环境，回到setjmp()的位置重新进行处理，这时候except_flag   
//的值为EXCEPT_RAISED    
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
  
//如果没有相关的处理函数，重新抛出    
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
