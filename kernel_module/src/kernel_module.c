#include <linux/kernel.h>       //piszemy czesc kernela
#include <linux/module.h>       //tutaj jest to modul
#include <linux/proc_fs.h>      //ktory ma obslugiwac PROCFS
#include <linux/uaccess.h>
#include <asm/uaccess.h>        //i posluguje sie: copy_from_user() i copy_to_user()
#include <asm/io.h>             //a calosc komunikowac sie bedzie z peryferiami: writel() i readl()

#define BUF_SIZE            100
#define MEM_BASE_ADDR       0x00100000
#define MEM_SIZE            16
#define SYKT_GPIO_FINISHER (0x0000)
#define FINISHER_FAIL       (0x3333)
#define SYKT_GPIO_CZ1_READ  (0x168)    
#define SYKT_GPIO_CZ1_WRITE  (0x16C) 
#define SYKT_GPIO_CZ2_READ  (0x170)    
#define SYKT_GPIO_CZ2_WRITE  (0x174)    
#define SYKT_GPIO_INT_READ (0x178)
#define SYKT_GPIO_INT_WRITE (0x17C)


//*******************************inicjacja wskaznikow***************************************************

void __iomem *read_cz1;               
void __iomem *read_cz2;                
void __iomem *write_cz1;                
void __iomem *write_cz2;   
void __iomem *int_read;    
void __iomem *int_write;          
void __iomem *finisher;                //FINISHER




//**********************************funckje odczytu******************************************************

static ssize_t read_czujnik1_func(struct file *file, char __user *ubuf, size_t count, loff_t *offset){
    char buf[BUF_SIZE];
    int len=0;
    

    if(*offset>0 || count<BUF_SIZE){
        return 0;                           
    }

    len=snprintf(buf, BUF_SIZE, "%x\n", readl(read_cz1));
                                            

    if(copy_to_user(ubuf, buf, len)){       
        return -EFAULT;    
    }

    *offset = len;
    return len;
}


static ssize_t read_czujnik2_func(struct file *file, char __user *ubuf, size_t count, loff_t *offset){
    char buf[BUF_SIZE];
    int len=0;
    
    
    
    if(*offset>0 || count<BUF_SIZE){
        return 0;                         
    }

    len=snprintf(buf, BUF_SIZE, "%x\n", readl(read_cz2)); 
                                           

    if(copy_to_user(ubuf, buf, len)){      
        return -EFAULT;    
    }

    *offset = len;
    return len;
}


static ssize_t read_int_func(struct file *file, char __user *ubuf, size_t count, loff_t *offset){
    char buf[BUF_SIZE];
    int len=0;
    
    
    if(*offset>0 || count<BUF_SIZE){
        return 0;                           
    }

    len=snprintf(buf, BUF_SIZE, "%x\n", readl(int_read)); 
                                         

    if(copy_to_user(ubuf, buf, len)){     
        return -EFAULT;    
    }

    *offset = len;
    return len;
}

//**********************************funckje zapisu******************************************************


static ssize_t write_czujnik1_func(struct file *file, const char __user *ubuf, size_t count, loff_t *offset){
    char buf[BUF_SIZE];
    int len=0;
    unsigned int input;
    
    if(count>BUF_SIZE){
        len=BUF_SIZE;                          
    }
    else{
      len=count;
    }
    
    if(copy_from_user(buf, ubuf, len)){       
        return -EFAULT;    
    }
    
    sscanf(buf, "%x\n", &input); 
    writel(input,write_cz1);
    return len;
}
static ssize_t write_czujnik2_func(struct file *file, const char __user *ubuf, size_t count, loff_t *offset){
    char buf[BUF_SIZE];
    int len=0;
    unsigned int input;
    
    if(count>BUF_SIZE){
        len=BUF_SIZE;                          
    }
    else{
      len=count;
    }
    
    if(copy_from_user(buf, ubuf, len)){       
        return -EFAULT;    
    }
    
    sscanf(buf, "%x\n", &input); 
    writel(input,write_cz2);
    return len;
}

static ssize_t write_int_func(struct file *file, const char __user *ubuf, size_t count, loff_t *offset){
    char buf[BUF_SIZE];
    int len=0;
    unsigned int input;
    
    if(count>BUF_SIZE){
        len=BUF_SIZE;                          
    }
    else{
      len=count;
    }
    
    if(copy_from_user(buf, ubuf, len)){       
        return -EFAULT;    
    }
    
    sscanf(buf, "%x\n", &input); 
    writel(input,int_write);
    return len;
}

//***************************************definiowanie struktur itp **********************************************




static struct file_operations my_cz1_read = {
    .owner = THIS_MODULE,
    .read = read_czujnik1_func,
};



static struct file_operations my_cz2_read = {
    .owner = THIS_MODULE,
    .read = read_czujnik2_func,
};

static struct file_operations my_int_read = {
    .owner = THIS_MODULE,
    .read = read_int_func,
};

static struct file_operations my_cz1_write = {
    .owner = THIS_MODULE,
    .write = write_czujnik1_func,
};

static struct file_operations my_cz2_write = {
    .owner = THIS_MODULE,
    .write = write_czujnik2_func,
};
static struct file_operations my_int_write = {
    .owner = THIS_MODULE,
    .write = write_int_func,
};



struct proc_dir_entry *parent;
static struct proc_dir_entry *czujnik1_read;
static struct proc_dir_entry *czujnik2_read;
static struct proc_dir_entry *ent_int_read;
static struct proc_dir_entry *czujnik1_write;
static struct proc_dir_entry *czujnik2_write;
static struct proc_dir_entry *ent_int_write;


static int procfs_init(void){
    printk(KERN_ALERT "Inicjalizacja projektu\n");

    parent=proc_mkdir("sykom", NULL);                   //utworzenie katalogu w /proc
    czujnik1_read=proc_create("kmc7el1", 0660, parent, &my_cz1_read); 
    czujnik2_read=proc_create("kmc7el3", 0660, parent, &my_cz2_read); 
    ent_int_read = proc_create("kmc7el5", 0660, parent, &my_int_read);
    czujnik1_write=proc_create("kmc7el2", 0660, parent, &my_cz1_write); 
    czujnik2_write=proc_create("kmc7el4", 0660, parent, &my_cz2_write); 
    ent_int_write = proc_create("kmc7el6", 0660, parent, &my_int_write);
    
    
    
    

    read_cz1=ioremap(MEM_BASE_ADDR+SYKT_GPIO_CZ1_READ, MEM_SIZE);
    read_cz2=ioremap(MEM_BASE_ADDR+SYKT_GPIO_CZ2_READ, MEM_SIZE);
    int_read=ioremap(MEM_BASE_ADDR+SYKT_GPIO_INT_READ, MEM_SIZE);
    write_cz1=ioremap(MEM_BASE_ADDR+SYKT_GPIO_CZ1_WRITE, MEM_SIZE);
    write_cz2=ioremap(MEM_BASE_ADDR+SYKT_GPIO_CZ2_WRITE, MEM_SIZE);
     int_write=ioremap(MEM_BASE_ADDR+SYKT_GPIO_INT_WRITE, MEM_SIZE);
    finisher = ioremap(MEM_BASE_ADDR+SYKT_GPIO_FINISHER, MEM_SIZE);

    return 0;
}

static void procfs_cleanup(void){
    printk(KERN_WARNING "Koniec akcji\n");

    proc_remove(czujnik1_read);
    proc_remove(czujnik2_read);
    proc_remove(ent_int_read);
    proc_remove(czujnik1_write);
    proc_remove(czujnik2_write);
    proc_remove(ent_int_write);
    remove_proc_entry("sykom", NULL);

    writel(FINISHER_FAIL | ((127)<<16), finisher);     //wywolanie FINISHER_PASS - konczy prace qemu
}

module_init(procfs_init);
module_exit(procfs_cleanup);

MODULE_INFO(intree, "Y");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Marcin Kruszewski");
MODULE_DESCRIPTION("SYKOM PROJECT");