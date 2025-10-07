#include "core/task.h"
#include <string.h>

Task gTasks[NUM_TASKS];

static void InsertTask(int newTaskId);
static int FindFirstActiveTask(void);

void TaskDummy(int taskId) {}

void ResetTasks(void) {
    for (int i = 0; i < NUM_TASKS; i++) {
        gTasks[i].isActive = false;
        gTasks[i].func = TaskDummy;
        gTasks[i].prev = i;
        gTasks[i].next = i + 1;
        gTasks[i].priority = -1;
        memset(gTasks[i].data, 0, sizeof(gTasks[i].data));
    }

    gTasks[0].prev = HEAD_SENTINEL;
    gTasks[NUM_TASKS - 1].next = TAIL_SENTINEL;
}

int CreateTask(TaskFunc func, int priority) {
    for (int i = 0; i < NUM_TASKS; i++) {
        if (!gTasks[i].isActive) {
            gTasks[i].func = func;
            gTasks[i].priority = priority;
            InsertTask(i);
            memset(gTasks[i].data, 0, sizeof(gTasks[i].data));
            gTasks[i].isActive = true;
            return i;
        }
    }

    return 0;
}

static void InsertTask(int newTaskId) {
    int taskId = FindFirstActiveTask();

    if (taskId == NUM_TASKS) {
        // The new task is the only task.
        gTasks[newTaskId].prev = HEAD_SENTINEL;
        gTasks[newTaskId].next = TAIL_SENTINEL;
        return;
    }

    while (1) {
        if (gTasks[newTaskId].priority < gTasks[taskId].priority) {
            // We've found a task with a higher priority value,
            // so we insert the new task before it.
            gTasks[newTaskId].prev = gTasks[taskId].prev;
            gTasks[newTaskId].next = taskId;
            if (gTasks[taskId].prev != HEAD_SENTINEL)
                gTasks[gTasks[taskId].prev].next = newTaskId;
            gTasks[taskId].prev = newTaskId;
            return;
        }

        if (gTasks[taskId].next == TAIL_SENTINEL) {
            // We've reached the end.
            gTasks[newTaskId].prev = taskId;
            gTasks[newTaskId].next = gTasks[taskId].next;
            gTasks[taskId].next = newTaskId;
            return;
        }

        taskId = gTasks[taskId].next;
    }
}

void DestroyTask(int taskId) {
    if (gTasks[taskId].isActive) {
        gTasks[taskId].isActive = false;

        if (gTasks[taskId].prev == HEAD_SENTINEL) {
            if (gTasks[taskId].next != TAIL_SENTINEL)
                gTasks[gTasks[taskId].next].prev = HEAD_SENTINEL;
        } else {
            if (gTasks[taskId].next == TAIL_SENTINEL) {
                gTasks[gTasks[taskId].prev].next = TAIL_SENTINEL;
            } else {
                gTasks[gTasks[taskId].prev].next = gTasks[taskId].next;
                gTasks[gTasks[taskId].next].prev = gTasks[taskId].prev;
            }
        }
    }
}

void RunTasks(void) {
    int taskId = FindFirstActiveTask();

    if (taskId != NUM_TASKS) {
        do {
            gTasks[taskId].func(taskId);
            taskId = gTasks[taskId].next;
        } while (taskId != TAIL_SENTINEL);
    }
}

static int FindFirstActiveTask(void) {
    int taskId;

    for (taskId = 0; taskId < NUM_TASKS; taskId++)
        if (gTasks[taskId].isActive && gTasks[taskId].prev == HEAD_SENTINEL)
            break;

    return taskId;
}

bool FuncIsActiveTask(TaskFunc func) {
    for (int i = 0; i < NUM_TASKS; i++)
        if (gTasks[i].isActive && gTasks[i].func == func)
            return true;

    return false;
}

int FindTaskIdByFunc(TaskFunc func) {
    for (int i = 0; i < NUM_TASKS; i++)
        if (gTasks[i].isActive && gTasks[i].func == func)
            return (int)i;

    return TASK_NONE; // No task was found.
}

int GetTaskCount(void) {
    int count = 0;

    for (int i = 0; i < NUM_TASKS; i++)
        if (gTasks[i].isActive)
            count++;

    return count;
}