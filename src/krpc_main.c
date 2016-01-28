

#include <linux/init.h>
#include <linux/in.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include "krpc.h"
#include "priv.h"

#define KRPC_MAX_BUF_RING_DEPTH		2048

static struct {
	u8 name[KRPC_MAX_STRING_LEN];
	msg_handler handler;
}resp_proc_rbl[KRPC_OPTYPE_MAX];

struct _krpc_queue{
	struct mutex lock;
	u32 head;
	u32 tail;
	struct buff_desc *buf_ring;
};

static struct _krpc_queue *task_queue;
static struct task_struct *krpc_trans_thread;
static int is_server = 0;
module_param(is_server, int, 0444);

int krpc_msg_handler_register(int optype, msg_handler handler)
{

	return 0;
}

static int krpc_task_queue_init(void)
{
	int ret, i, j;
	struct buff_desc *desc;

	task_queue = kmalloc(sizeof(struct _krpc_queue), GFP_KERNEL);
	if (!task_queue) {
		printk(KERN_ERR "alloc task queue failed.\n");
		ret = -ENOMEM;
		goto ALLOC_TASK_QUEUE_FAILED;
	}
	
	mutex_init(&task_queue->lock);
	task_queue->head = 0;
	task_queue->tail = 0;
	
	task_queue->buf_ring = kmalloc(KRPC_MAX_BUF_RING_DEPTH *
		sizeof(struct buff_desc), GFP_KERNEL);
	if (!task_queue->buf_ring) {
		printk(KERN_ERR "alloc ring buffer failed.\n");
		ret = -ENOMEM;
		goto ALLOC_RING_BUF_FAILED;
	}

	for (i = 0; i < KRPC_MAX_BUF_RING_DEPTH; i++) {
		desc = &task_queue->buf_ring[i];
		desc->state = KRPC_BUFFER_STATE_FREE;
		desc->buffer = (u8*)__get_free_page(GFP_KERNEL);
		if (!desc->buffer) {
			ret = -ENOMEM;
			goto ALLOC_DESC_BUF_FAILED;
		}
	}

	return 0;
ALLOC_DESC_BUF_FAILED:
	for (j = 0; j < i; j++) {
		desc = &task_queue->buf_ring[i];
		free_page((unsigned long)desc->buffer);
	}
	kfree(task_queue->buf_ring);

ALLOC_RING_BUF_FAILED:
	mutex_destroy(&task_queue->lock);

	kfree(task_queue);
ALLOC_TASK_QUEUE_FAILED:

	return ret;
}

static int krpc_socket_proc(void *data)
{

	while(!kthread_should_stop()) {
		printk(KERN_INFO "krpc_socket_proc called\n");
		msleep(5000);
	}

	printk(KERN_INFO "krpc_socket_proc exits...\n");
	return 0;
}

static int krpc_server_init(void)
{
	krpc_trans_thread = kthread_run(krpc_socket_proc, NULL,
		"krpc_socket");

	if (!krpc_trans_thread) {
		printk(KERN_ERR "create socket proc thread failed\n");
		return -ESRCH;
	}

	return 0;
}

static void krpc_server_free(void)
{
	kthread_stop(krpc_trans_thread);
}

static void krpc_task_queue_free(void)
{
	int i;
	struct buff_desc *desc;

	for (i = 0; i < KRPC_MAX_BUF_RING_DEPTH; i++) {
		desc = &task_queue->buf_ring[i];
		free_page((unsigned long)desc->buffer);
	}
	kfree(task_queue->buf_ring);

	mutex_destroy(&task_queue->lock);

	kfree(task_queue);
}

static int krpc_init(void)
{
	int rc;

	printk(KERN_INFO "krpc begin to init.\n");

	rc = krpc_task_queue_init();
	if (rc) {
		printk(KERN_ERR "init task queue failed, ret = %d\n", rc);
		goto TASK_QUEUE_INIT_FAILED;
	}

	if (is_server)
		krpc_server_init();
	
	return 0;
TASK_QUEUE_INIT_FAILED:

	return rc;
}

static void krpc_exit(void)
{
	printk(KERN_INFO "krpc stopped.\n");
	krpc_task_queue_free();
	if (is_server)
		krpc_server_free();
}

module_init(krpc_init);
module_exit(krpc_exit);

