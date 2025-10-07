#pragma once

#include <stdbool.h>

#define HEAD_SENTINEL 0xFE
#define TAIL_SENTINEL 0xFF
#define TASK_NONE TAIL_SENTINEL

#define NUM_TASKS 16
#define NUM_TASK_DATA 16

typedef void (*TaskFunc)(int taskId);

typedef struct {
    TaskFunc func;
    bool isActive;
    int prev;
    int next;
    int priority;
    int data[NUM_TASK_DATA];
} Task;

extern Task gTasks[];

void ResetTasks(void);
int CreateTask(TaskFunc func, int priority);
void DestroyTask(int taskId);
void RunTasks(void);
void TaskDummy(int taskId);
bool FuncIsActiveTask(TaskFunc func);
int FindTaskIdByFunc(TaskFunc func);
int GetTaskCount(void);
