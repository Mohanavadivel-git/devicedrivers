#include <linux/init.h>
#include <linux/module.h>
//step 1
#include <linux/moduleparam.h>

//step2 - creating a variable
int param_var = 0;

//step3 - register variable macro
//module_param(name_var,type, permissions)
/*permissions - 
  S_IRUSR
  S_IWUSR
  S_IXUSR
  S_IWGRP
  S_IRGRP
  
  S_IRUSR | S_IWUSR
 */
module_param(param_var, int, S_IRUSR | S_IWUSR);

void display(void){
	printk(KERN_ALERT "TEST: param=%d\n",param_var);
}

static int hello_init(void){
	printk(KERN_ALERT "TEST: Hello world, this is mycode\n");
	display();
	return 0;
}

static void hello_exit(void){
	printk (KERN_ALERT "TEST: Goodbye, from mycode\n");
}

module_init(hello_init);
module_exit(hello_exit);
