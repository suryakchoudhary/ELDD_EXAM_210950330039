#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/kdev_t.h>
#include<linux/types.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/uaccess.h>
#include<linux/semaphore.h>


#define NAME SYNCDRIV
#define NAME2 SYNCDRIV_2


MODULE_LICENSE("GPL");
MODULE_AUTHOR("SURYA");

struct cdev *my_cdev; 
struct cdev *my_2_cdev; 
struct semaphore sem;
struct completion data_read_done;
struct completion data_write_done;


//Device 1
int NAME_open(struct inode *inode,struct file *filp);
ssize_t NAME_read(struct file *filp,char __user *ubuff,size_t count,loff_t *offp);
ssize_t NAME_write(struct file *filp, const char __user *ubuff,size_t count,loff_t *offp);
int NAME_release(struct inode *inode,struct file *filp);


//Device 2
int NAME2_open(struct inode *inode,struct file *filp);
ssize_t NAME2_read(struct file *filp,char __user *ubuff,size_t count,loff_t *offp);
ssize_t NAME2_write(struct file *filp, const char __user *ubuff,size_t count,loff_t *offp);
int NAME2_release(struct inode *inode,struct file *filp);

struct file_operations fops=
{
	.owner   =   THIS_MODULE,
	.open    =   NAME_open,
	.read    =   NAME_read,
	.write   =   NAME_write,
	.release =   NAME_release,
};

dev_t DevNo, DevNo_2;

static int __init prog_init(void)
{
	int result, result_2;
	int Maj, Min;             //Device 1
	int Maj_2, Min_2;       //Devive 2
	
	//Device 1
	result = alloc_chrdev_region(&DevNo, 0, 1, "SYNCDRIV");
	Maj = MAJOR(DevNo);
	Min = MINOR(DevNo);
	printk(KERN_INFO "\nMAJOR NUMBER = %d............MINOR NUMBER = %d..\n",Maj, Min);

	//Device 2
	result_2 = alloc_chrdev_region(&DevNo_2, 0, 1, "SYNCDRIV_2");
	Maj_2 = MAJOR(DevNo_2);
	Min_2 = MINOR(DevNo_2);
	printk(KERN_INFO "\nMAJOR NUMBER = %d............MINOR NUMBER = %d..\n",Maj_2, Min_2);

	//Device 1
	if(result<0)
	{
		printk(KERN_ALERT "\n Region not available\n");
		return(-1);
	}

	my_cdev = cdev_alloc();
	my_cdev->ops = &fops;

	result=cdev_add(my_cdev, DevNo, 1);

	if(result<0)
	{
		printk(KERN_INFO "\n THE DEVICE NUMBER AND CDEV NOT CONNECTED.\n");
		unregister_chrdev_region(DevNo, 1);
		return(-1);
	}
	
	//Device 2
	if(result_2 < 0)
	{
		printk(KERN_ALERT "\n Region not available\n");
		return(-1);
	}

	my_2_cdev = cdev_alloc();
	my_2_cdev->ops = &fops;
	result_2=cdev_add(my_2_cdev, DevNo_2, 1);

	if(result_2 < 0)
	{
		printk(KERN_INFO "\n THE DEVICE NUMBER AND CDEV NOT CONNECTED.\n");
		unregister_chrdev_region(DevNo_2, 1);
		return(-1);
	}



	// semaphore initialize
	sema_init(&sem,1);

	// completion initialize
	init_completion(&data_read_done);
	init_completion(&data_write_done);
	printk(KERN_ALERT "\n SEMAPHORE IS INITAILIZED..\n");
	return 0;
}

static void __exit prog_exit(void)
{
	int Maj, Min;         //Device 1
	int Maj_2, Min_2;   //Device 2

	//Device 1
	Maj = MAJOR(DevNo);
	Min = MINOR(DevNo);
	cdev_del(my_cdev);
	unregister_chrdev_region(DevNo, 1);
	printk(KERN_INFO "\n THE MAJOR NUMBER DEVICE 1 = %d.. THE MINOR NUMBER DEVICE 1 = %d..\n",Maj, Min);

	//Device_2
	Maj_2 = MAJOR(DevNo_2);
	Min_2 = MINOR(DevNo_2);
	cdev_del(my_2_cdev);
	unregister_chrdev_region(DevNo_2, 1);
	printk(KERN_INFO "\n THE MAJOR NUMBER DEVIVE 2 = %d.. THE MINOR NUMBER DEVICE 2 = %d..\n",Maj_2, Min_2);
	printk(KERN_ALERT"\n Deallocated Everything\n");
}
module_init(prog_init);
module_exit(prog_exit);

//function definition

// Globally declared

char K_buff[50];

//Device 1
int NAME_open(struct inode *inode,struct file *filp)
{

	printk(KERN_ALERT "\n THE OPEN SYSTEM CALL IS CALLED...\n");
	return 0;
}

//Device 2
int NAME2_open(struct inode *inode,struct file *filp)
{

	printk(KERN_ALERT "\n THE OPEN SYSTEM CALL IS CALLED...\n");
	return 0;
}

//Device 1
ssize_t NAME_read(struct file *filp,char __user *U_buff,size_t count,loff_t *offp)
{
	unsigned long result;
	ssize_t retval;
	wait_for_completion_interruptible(&data_write_done);
	result = copy_to_user((char*)U_buff, (char*)K_buff, count);
	complete(&data_read_done);
	if(result == 0)
	{
		printk(KERN_ALERT "\n MESSAGE TO USER..\n...%s....\n", K_buff);
		printk(KERN_INFO  "\n DATA SEND COMPLETED..\n");
		retval = count;
		return retval;
	}
	else if(result>0)
	{  
		printk(KERN_ALERT "\n MESSAGE TO USER..\n...%s....\n", K_buff);
		printk(KERN_ALERT "\n THE PART OF DATA IS SENDED..\n");
		retval = (count-result);
		return retval;
	}
	else
	{
		printk(KERN_ALERT "\n ERROR IN READING\n");
		retval = -EFAULT;
		return retval;
	}

}


//Device 2
ssize_t NAME2_read(struct file *filp,char __user *U_buff,size_t count,loff_t *offp)
{
	unsigned long result;
	ssize_t retval;
	wait_for_completion_interruptible(&data_write_done);
	result = copy_to_user((char*)U_buff, (char*)K_buff, count);
	complete(&data_read_done);
	if(result == 0)
	{
		printk(KERN_ALERT "\n MESSAGE TO USER..\n...%s....\n", K_buff);
		printk(KERN_INFO  "\n DATA SEND COMPLETED..\n");
		retval = count;
		return retval;
	}
	else if(result>0)
	{  
		printk(KERN_ALERT "\n MESSAGE TO USER..\n...%s....\n", K_buff);
		printk(KERN_ALERT "\n THE PART OF DATA IS SENDED..\n");
		retval = (count-result);
		return retval;
	}
	else
	{
		printk(KERN_ALERT "\n ERROR IN READING\n");
		retval = -EFAULT;
		return retval;
	}
}


//Device 1
ssize_t NAME_write(struct file *filp, const char __user *U_buff, size_t count, loff_t *offp)
{

	unsigned long result;
	ssize_t retval;
	down(&sem);
	printk(KERN_ALERT "\n CRITICAL SECTION STARTED..\n");

	result=copy_from_user((char*)K_buff, (char*)U_buff, count);
	complete(&data_write_done);
	wait_for_completion_interruptible(&data_read_done);
	up(&sem);

	if(result == 0)
	{
		printk(KERN_ALERT "\n MESSAGE FROM USER..\n...%s....\n", K_buff);
		printk(KERN_INFO  "\n DATA RECEIVED COMPLETED..\n");
		retval = count;
		return retval;
	}
	else if(result > 0)
	{  
		printk(KERN_ALERT "\n MESSAGE FROM USER..\n...%s....\n",K_buff);
		printk(KERN_ALERT "\n THE PART OF DATA IS RECEIVED..\n ");
		retval = (count-result);
		return retval;
	}
	else
	{
		printk(KERN_ALERT "\n ERROR IN WRITING\n");
		retval = -EFAULT;
		return retval;
	}

}

//Device 2
ssize_t NAME2_write(struct file *filp, const char __user *U_buff, size_t count, loff_t *offp)
{ 
	unsigned long result;
	ssize_t retval;
	down(&sem);
	printk(KERN_ALERT "\n CRITICAL SECTION STARTED..\n");

	result=copy_from_user((char*)K_buff, (char*)U_buff, count);
	complete(&data_write_done);
	wait_for_completion_interruptible(&data_read_done);
	up(&sem);

	if(result == 0)
	{
		printk(KERN_ALERT "\n MESSAGE FROM USER..\n...%s....\n", K_buff);
		printk(KERN_INFO  "\n DATA RECEIVED COMPLETED..\n");
		retval = count;
		return retval;
	}
	else if(result > 0)
	{
		printk(KERN_ALERT "\n MESSAGE FROM USER..\n...%s....\n",K_buff);
		printk(KERN_ALERT "\n THE PART OF DATA IS RECEIVED..\n ");
		retval = (count-result);
		return retval;
	}
	else
	{
		printk(KERN_ALERT "\n ERROR IN WRITING\n");
		retval = -EFAULT;
		return retval;
	}
}


//Device 1
int NAME_release(struct inode *inode, struct file *filp)
{

	printk(KERN_ALERT "\n THE CLOSE SYSTEM CALL IS CALLED...\n");
	return 0;
}

//Device 2
int NAME2_release(struct inode *inode, struct file *filp)
{
	printk(KERN_ALERT "\n THE CLOSE SYSTEM CALL IS CALLED...\n");
	return 0;
}
