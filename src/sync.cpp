#include "../include/common.h"

#ifdef _WIN32
void syncInit(SyncPrimitives& sync) {
    sync.startEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    sync.cantProceedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    sync.continueEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    sync.terminateEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    sync.threadHandle = NULL;
}

void syncDestroy(SyncPrimitives& sync) {
    CloseHandle(sync.startEvent);
    CloseHandle(sync.cantProceedEvent);
    CloseHandle(sync.continueEvent);
    CloseHandle(sync.terminateEvent);
    if (sync.threadHandle) CloseHandle(sync.threadHandle);
}

void syncSignalStart(SyncPrimitives& sync) {
    SetEvent(sync.startEvent);
}

void syncWaitForStart(SyncPrimitives& sync) {
    WaitForSingleObject(sync.startEvent, INFINITE);
}

void syncSignalCantProceed(SyncPrimitives& sync) {
    SetEvent(sync.cantProceedEvent);
}

void syncWaitForCantProceed(SyncPrimitives& sync) {
    WaitForSingleObject(sync.cantProceedEvent, INFINITE);
}

void syncSignalContinue(SyncPrimitives& sync) {
    SetEvent(sync.continueEvent);
}

void syncWaitForContinue(SyncPrimitives& sync) {
    WaitForSingleObject(sync.continueEvent, INFINITE);
}

void syncSignalTerminate(SyncPrimitives& sync) {
    SetEvent(sync.terminateEvent);
}

void syncWaitForTerminate(SyncPrimitives& sync) {
    WaitForSingleObject(sync.terminateEvent, INFINITE);
}

void syncWaitForThread(SyncPrimitives& sync) {
    WaitForSingleObject(sync.threadHandle, INFINITE);
}

int syncCheckTerminate(SyncPrimitives& sync) {
    return WaitForSingleObject(sync.terminateEvent, 0) == WAIT_OBJECT_0;
}

int syncCheckContinue(SyncPrimitives& sync) {
    return WaitForSingleObject(sync.continueEvent, 0) == WAIT_OBJECT_0;
}

void syncResetContinue(SyncPrimitives& sync) {
    ResetEvent(sync.continueEvent);
}

#else
void syncInit(SyncPrimitives& sync) {
    pthread_mutex_init(&sync.mutex, NULL);
    pthread_cond_init(&sync.startCond, NULL);
    pthread_cond_init(&sync.cantProceedCond, NULL);
    pthread_cond_init(&sync.continueCond, NULL);
    pthread_cond_init(&sync.terminateCond, NULL);
    sync.startFlag = 0;
    sync.cantProceedFlag = 0;
    sync.continueFlag = 0;
    sync.terminateFlag = 0;
}

void syncDestroy(SyncPrimitives& sync) {
    pthread_mutex_destroy(&sync.mutex);
    pthread_cond_destroy(&sync.startCond);
    pthread_cond_destroy(&sync.cantProceedCond);
    pthread_cond_destroy(&sync.continueCond);
    pthread_cond_destroy(&sync.terminateCond);
}

void syncSignalStart(SyncPrimitives& sync) {
    pthread_mutex_lock(&sync.mutex);
    sync.startFlag = 1;
    pthread_cond_signal(&sync.startCond);
    pthread_mutex_unlock(&sync.mutex);
}

void syncWaitForStart(SyncPrimitives& sync) {
    pthread_mutex_lock(&sync.mutex);
    while (sync.startFlag == 0) {
        pthread_cond_wait(&sync.startCond, &sync.mutex);
    }
    pthread_mutex_unlock(&sync.mutex);
}

void syncSignalCantProceed(SyncPrimitives& sync) {
    pthread_mutex_lock(&sync.mutex);
    sync.cantProceedFlag = 1;
    pthread_cond_signal(&sync.cantProceedCond);
    pthread_mutex_unlock(&sync.mutex);
}

void syncWaitForCantProceed(SyncPrimitives& sync) {
    pthread_mutex_lock(&sync.mutex);
    while (sync.cantProceedFlag == 0) {
        pthread_cond_wait(&sync.cantProceedCond, &sync.mutex);
    }
    sync.cantProceedFlag = 0;
    pthread_mutex_unlock(&sync.mutex);
}

void syncSignalContinue(SyncPrimitives& sync) {
    pthread_mutex_lock(&sync.mutex);
    sync.continueFlag = 1;
    pthread_cond_signal(&sync.continueCond);
    pthread_mutex_unlock(&sync.mutex);
}

void syncWaitForContinue(SyncPrimitives& sync) {
    pthread_mutex_lock(&sync.mutex);
    while (sync.continueFlag == 0) {
        pthread_cond_wait(&sync.continueCond, &sync.mutex);
    }
    sync.continueFlag = 0;
    pthread_mutex_unlock(&sync.mutex);
}

void syncSignalTerminate(SyncPrimitives& sync) {
    pthread_mutex_lock(&sync.mutex);
    sync.terminateFlag = 1;
    pthread_cond_signal(&sync.terminateCond);
    pthread_mutex_unlock(&sync.mutex);
}

void syncWaitForTerminate(SyncPrimitives& sync) {
    pthread_mutex_lock(&sync.mutex);
    while (sync.terminateFlag == 0) {
        pthread_cond_wait(&sync.terminateCond, &sync.mutex);
    }
    pthread_mutex_unlock(&sync.mutex);
}

void syncWaitForThread(SyncPrimitives& sync) {
    pthread_join(sync.thread, NULL);
}

int syncCheckTerminate(SyncPrimitives& sync) {
    pthread_mutex_lock(&sync.mutex);
    int result = sync.terminateFlag;
    pthread_mutex_unlock(&sync.mutex);
    return result;
}

int syncCheckContinue(SyncPrimitives& sync) {
    pthread_mutex_lock(&sync.mutex);
    int result = sync.continueFlag;
    pthread_mutex_unlock(&sync.mutex);
    return result;
}

void syncResetContinue(SyncPrimitives& sync) {
    pthread_mutex_lock(&sync.mutex);
    sync.continueFlag = 0;
    pthread_mutex_unlock(&sync.mutex);
}
#endif
