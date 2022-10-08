#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "azure_threadx_demo.h"

#define DEMO_QUEUE_SIZE         100
#define DEMO_STACK_SIZE         1024
#define DEMO_BYTE_POOL_SIZE     9120
#define DEMO_BLOCK_POOL_SIZE    100

TX_THREAD thread_0;
TX_THREAD thread_1;
TX_THREAD thread_2;
TX_THREAD thread_3;
TX_THREAD thread_4;
TX_THREAD thread_5;
TX_THREAD thread_6;
TX_THREAD thread_7;

TX_QUEUE queue_0;
TX_MUTEX mutex_0;
TX_SEMAPHORE semaphore_0;
TX_BYTE_POOL byte_pool_0;
TX_BLOCK_POOL block_pool_0;
TX_EVENT_FLAGS_GROUP event_flags_0;

ULONG thread_0_counter;
ULONG thread_1_counter;
ULONG thread_1_messages_sent;
ULONG thread_2_counter;
ULONG thread_2_messages_received;
ULONG thread_3_counter;
ULONG thread_4_counter;
ULONG thread_5_counter;
ULONG thread_6_counter;
ULONG thread_7_counter;

void thread_0_entry(ULONG thread_input);
void thread_1_entry(ULONG thread_input);
void thread_2_entry(ULONG thread_input);
void thread_3_and_4_entry(ULONG thread_input);
void thread_5_entry(ULONG thread_input);
void thread_6_and_7_entry(ULONG thread_input);

int azure_threadx_demo_init(int argc, char *argv[])
{
    tx_kernel_enter();
    return 0;
}

int azure_threadx_demo_exit(void)
{
    return -1;
}

void tx_application_define(void *first_unused_memory)
{
    CHAR *pointer = (CHAR *)TX_NULL;

    // 创建一个字节内存池，从中分配线程堆栈
    tx_byte_pool_create(&byte_pool_0, "byte pool 0", first_unused_memory, DEMO_BYTE_POOL_SIZE);

    // 在此处输入系统定义内容，例如线程创建和其他分类创建信息

    // 为线程0分配堆栈
    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    // 创建主线程
    tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0, pointer, DEMO_STACK_SIZE, 1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

    // 为线程1分配堆栈
    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    // 创建线程1。这些线程通过ThreadX消息队列传递信息。值得注意的是，这些线程有一个时间片
    tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1, pointer, DEMO_STACK_SIZE, 16, 16, 4, TX_AUTO_START);

    // 为线程2分配堆栈
    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    // 创建线程2。这些线程通过ThreadX消息队列传递信息。值得注意的是，这些线程有一个时间片
    tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2, pointer, DEMO_STACK_SIZE, 16, 16, 4, TX_AUTO_START);

    // 为线程3分配堆栈
    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    // 创建线程3。这些线程竞争ThreadX计数信号量。这里有趣的是，两个线程共享相同的指令区域
    tx_thread_create(&thread_3, "thread 3", thread_3_and_4_entry, 3, pointer, DEMO_STACK_SIZE, 8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);

    // 为线程4分配堆栈
    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    // 创建线程4。这些线程竞争ThreadX计数信号量。这里有趣的是，两个线程共享相同的指令区域
    tx_thread_create(&thread_4, "thread 4", thread_3_and_4_entry, 4, pointer, DEMO_STACK_SIZE, 8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);

    // 为线程5分配堆栈
    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    // 创建线程5。该线程仅挂起一个事件标志，该标志将由thread_0设置
    tx_thread_create(&thread_5, "thread 5", thread_5_entry, 5, pointer, DEMO_STACK_SIZE, 4, 4, TX_NO_TIME_SLICE, TX_AUTO_START);

    // 为线程6分配堆栈
    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    // 创建线程6。这些线程竞争ThreadX互斥
    tx_thread_create(&thread_6, "thread 6", thread_6_and_7_entry, 6, pointer, DEMO_STACK_SIZE, 8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);

    // 为线程7分配堆栈
    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    // 创建线程7。这些线程竞争ThreadX互斥
    tx_thread_create(&thread_7, "thread 7", thread_6_and_7_entry, 7, pointer, DEMO_STACK_SIZE, 8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);

    // 分配消息队列
    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, DEMO_QUEUE_SIZE * sizeof(ULONG), TX_NO_WAIT);

    // 创建线程1和线程2共享的消息队列
    tx_queue_create(&queue_0, "queue 0", TX_1_ULONG, pointer, DEMO_QUEUE_SIZE * sizeof(ULONG));

    // 创建线程3和4使用的信号量
    tx_semaphore_create(&semaphore_0, "semaphore 0", 1);

    // 创建线程1和5使用的事件标志组
    tx_event_flags_create(&event_flags_0, "event flags 0");

    // 创建线程6和7使用的互斥锁，而不使用优先级继承
    tx_mutex_create(&mutex_0, "mutex 0", TX_NO_INHERIT);

    // 为小型块池分配内存
    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, DEMO_BLOCK_POOL_SIZE, TX_NO_WAIT);

    // 创建块内存池以从中分配消息缓冲区
    tx_block_pool_create(&block_pool_0, "block pool 0", sizeof(ULONG), pointer, DEMO_BLOCK_POOL_SIZE);

    // 分配块并释放块内存
    tx_block_allocate(&block_pool_0, (VOID **)&pointer, TX_NO_WAIT);

    // 将块释放回池
    tx_block_release(pointer);
}

void thread_0_entry(ULONG thread_input)
{
    UINT status;

    // 这个线程只是处于while forever sleep循环中
    while (1) {
        // 增加线程计数器
        thread_0_counter++;

        // 打印结果
        printf("**** ThreadX SMP Linux Demonstration **** (c) 1996-2022 Microsoft Corporation\n\n");
        printf("           thread 0 events sent:          %lu\n", thread_0_counter);
        printf("           thread 1 messages sent:        %lu\n", thread_1_counter);
        printf("           thread 2 messages received:    %lu\n", thread_2_counter);
        printf("           thread 3 obtained semaphore:   %lu\n", thread_3_counter);
        printf("           thread 4 obtained semaphore:   %lu\n", thread_4_counter);
        printf("           thread 5 events received:      %lu\n", thread_5_counter);
        printf("           thread 6 mutex obtained:       %lu\n", thread_6_counter);
        printf("           thread 7 mutex obtained:       %lu\n\n", thread_7_counter);

        // 睡眠10个滴答
        tx_thread_sleep(10);

        // 将事件标志0设置为唤醒线程5
        status =  tx_event_flags_set(&event_flags_0, 0x1, TX_OR);
        if (status != TX_SUCCESS) {
            break;
        }
    }
}

void thread_1_entry(ULONG thread_input)
{
    UINT status;

    // 此线程只将消息发送到线程2共享的队列
    while (1) {
        // 增加线程计数器
        thread_1_counter++;

        // 将消息发送到队列0
        status =  tx_queue_send(&queue_0, &thread_1_messages_sent, TX_WAIT_FOREVER);
        if (status != TX_SUCCESS) {
            break;
        }

        // 增加发送的消息
        thread_1_messages_sent++;
    }
}

void thread_2_entry(ULONG thread_input)
{
    UINT status;
    ULONG received_message;

    // 此线程检索线程1放置在队列中的消息
    while (1) {
        // 增加线程计数器
        thread_2_counter++;

        // 从队列中检索消息
        status = tx_queue_receive(&queue_0, &received_message, TX_WAIT_FOREVER);
        if ((status != TX_SUCCESS) || (received_message != thread_2_messages_received)) {
            break;
        }

        // 增加接收的消息计数
        thread_2_messages_received++;
    }
}

void thread_3_and_4_entry(ULONG thread_input)
{
    UINT status;

    // 此函数从线程3和线程4执行。如下面的循环所示，这些函数争夺semaphore_0的所有权
    while (1) {
        // 增加线程计数器
        if (thread_input == 3) {
            thread_3_counter++;
        } else {
            thread_4_counter++;
        }

        // 获取带暂停的信号灯
        status = tx_semaphore_get(&semaphore_0, TX_WAIT_FOREVER);
        if (status != TX_SUCCESS) {
            break;
        }

        // 休眠2个滴答以保持信号量
        tx_thread_sleep(2);

        // 释放信号灯
        status = tx_semaphore_put(&semaphore_0);
        if (status != TX_SUCCESS) {
            break;
        }
    }
}

void thread_5_entry(ULONG thread_input)
{
    UINT status;
    ULONG actual_flags;

    // 这个线程只是等待一个永久循环中的事件
    while (1) {
        // 增加线程计数器
        thread_5_counter++;

        // 等待事件标志0
        status =  tx_event_flags_get(&event_flags_0, 0x1, TX_OR_CLEAR,  &actual_flags, TX_WAIT_FOREVER);
        if ((status != TX_SUCCESS) || (actual_flags != 0x1)) {
            break;
        }
    }
}

void thread_6_and_7_entry(ULONG thread_input)
{
    UINT status;

    // 这个函数从线程6和线程7执行。如下面的循环所示，这些函数争夺mutex_0的所有权
    while (1) {
        // 增加线程计数器
        if (thread_input == 6) {
            thread_6_counter++;
        } else {
            thread_7_counter++;
        }

        // 获取带有挂起的互斥锁
        status = tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);
        if (status != TX_SUCCESS) {
            break;
        }

        // 再次使用挂起获取互斥锁。这表明一个拥有线程可以多次检索它所拥有的互斥锁
        status = tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);
        if (status != TX_SUCCESS) {
            break;
        }

        // 休眠2个滴答以保持互斥锁
        tx_thread_sleep(2);

        // 释放互斥锁
        status = tx_mutex_put(&mutex_0);
        if (status != TX_SUCCESS) {
            break;
        }

        // 再次释放互斥锁。这实际上会释放所有权，因为它获得了两次
        status = tx_mutex_put(&mutex_0);
        if (status != TX_SUCCESS) {
            break;
        }
    }
}
