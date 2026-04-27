#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

typedef struct producer_args_t {
    int loops;
    int thread_id;
    const char *name;
    int produced;
    int last_produced;
    int wait_count;
} producer_args_t;

typedef struct consumer_args_t {
    int loops;
    int thread_id;
    const char *name;
    int consumed;
    int last_consumed;
    int wait_count;
} consumer_args_t;

int buffer = 0;
int count = 0;
int loops = 100;

static int total_producers = 1;
static int total_consumers = 2;
static int waiting_producers = 0;
static int waiting_consumers = 0;
static long progress_epoch = 0;
static long last_seen_progress = -1;
static int stalled_timeouts = 0;
static long last_reported_progress = -1;

static const char *COLOR_RESET = "\x1b[0m";
static const char *COLOR_BLUE = "\x1b[1;34m";
static const char *COLOR_GREEN = "\x1b[1;32m";
static const char *COLOR_RED = "\x1b[1;31m";

static const char *role_color(const char *name) {
    if (strcmp(name, "producer") == 0) {
        return COLOR_BLUE;
    }
    if (strcmp(name, "consumer") == 0) {
        return COLOR_GREEN;
    }
    return "";
}

static void maybe_warn_potential_deadlock(const char *trigger_name, int trigger_id) {
    int all_threads_sleeping = (waiting_producers == total_producers && waiting_consumers == total_consumers);
    if (all_threads_sleeping && progress_epoch == last_seen_progress) {
        stalled_timeouts++;
    } else {
        stalled_timeouts = 0;
        last_seen_progress = progress_epoch;
    }

    if (!all_threads_sleeping || stalled_timeouts < 2 || last_reported_progress == progress_epoch) {
        return;
    }

    printf("%s\n"
           "==================== POTENTIAL DEADLOCK WARNING ====================\n"
           "No progress for multiple timed waits while ALL worker threads are sleeping.\n"
           "trigger=%s-%d count=%d buffer=%d progress_epoch=%ld\n"
           "waiting_producers=%d/%d waiting_consumers=%d/%d\n"
           "====================================================================%s\n",
           COLOR_RED, trigger_name, trigger_id, count, buffer, progress_epoch, waiting_producers, total_producers,
           waiting_consumers, total_consumers, COLOR_RESET);
    fflush(stdout);
    last_reported_progress = progress_epoch;
}

static void debug_log(const char *name, int id, const char *fmt, ...) {
    struct timespec ts;
    const char *color = role_color(name);
    const char *color_end = color[0] ? COLOR_RESET : "";
    clock_gettime(CLOCK_REALTIME, &ts);
    printf("[%ld.%03ld] [%s%s-%d tid=%lu%s] ", ts.tv_sec, ts.tv_nsec / 1000000, color, name, id,
           (unsigned long)pthread_self(), color_end);

    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
    fflush(stdout);
}

static void set_timeout_1s(struct timespec *ts) {
    clock_gettime(CLOCK_REALTIME, ts);
    ts->tv_sec += 1;
}

void put(int value) {
    assert(count == 0);
    count = 1;
    buffer = value;
}

int get() {
    assert(count == 1);
    count = 0;
    return buffer;
}

void *producer(void *arg) {
    producer_args_t *args = (producer_args_t *)arg;
    int i;

    debug_log(args->name, args->thread_id, "started, loops=%d", args->loops);
    for (i = 0; i < args->loops; i++) {
        pthread_mutex_lock(&mutex);

        while (count == 1) {
            struct timespec ts;
            set_timeout_1s(&ts);
            args->wait_count++;
            waiting_producers++;
            debug_log(args->name, args->thread_id, "buffer full, waiting... (count=%d buffer=%d wait_count=%d)", count,
                      buffer, args->wait_count);
            int rc = pthread_cond_timedwait(&cond, &mutex, &ts);
            if (rc == ETIMEDOUT) {
                debug_log(args->name, args->thread_id, "still blocked after 1s (count=%d buffer=%d produced=%d)", count,
                          buffer, args->produced);
                maybe_warn_potential_deadlock(args->name, args->thread_id);
            } else if (rc != 0) {
                debug_log(args->name, args->thread_id, "cond wait error: %s", strerror(rc));
            }
            waiting_producers--;
        }

        put(i);
        progress_epoch++;
        args->last_produced = i;
        args->produced++;
        debug_log(args->name, args->thread_id, "produced=%d (total_produced=%d count=%d)", i, args->produced, count);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
    debug_log(args->name, args->thread_id, "finished (produced=%d last_produced=%d waits=%d)", args->produced,
              args->last_produced, args->wait_count);
    return NULL;
}

void *consumer(void *arg) {
    consumer_args_t *args = (consumer_args_t *)arg;
    int i;

    debug_log(args->name, args->thread_id, "started, loops=%d", args->loops);
    for (i = 0; i < args->loops; i++) {
        pthread_mutex_lock(&mutex);

        while (count == 0) {
            struct timespec ts;
            set_timeout_1s(&ts);
            args->wait_count++;
            waiting_consumers++;
            debug_log(args->name, args->thread_id, "buffer empty, waiting... (count=%d wait_count=%d consumed=%d)",
                      count, args->wait_count, args->consumed);
            int rc = pthread_cond_timedwait(&cond, &mutex, &ts);
            if (rc == ETIMEDOUT) {
                debug_log(args->name, args->thread_id, "still blocked after 1s (count=%d consumed=%d last_consumed=%d)",
                          count, args->consumed, args->last_consumed);
                maybe_warn_potential_deadlock(args->name, args->thread_id);
            } else if (rc != 0) {
                debug_log(args->name, args->thread_id, "cond wait error: %s", strerror(rc));
            }
            waiting_consumers--;
        }

        int value = get();
        progress_epoch++;
        args->last_consumed = value;
        args->consumed++;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        debug_log(args->name, args->thread_id, "consumed=%d (total_consumed=%d count=%d)", value, args->consumed,
                  count);
    }
    debug_log(args->name, args->thread_id, "finished (consumed=%d last_consumed=%d waits=%d)", args->consumed,
              args->last_consumed, args->wait_count);
    return NULL;
}

int main(void) {
    pthread_t p1, c1, c2;
    producer_args_t producer_args = {
        .loops = loops * 2, .thread_id = 1, .name = "producer", .produced = 0, .last_produced = -1, .wait_count = 0};
    consumer_args_t consumer1_args = {
        .loops = loops, .thread_id = 1, .name = "consumer", .consumed = 0, .last_consumed = -1, .wait_count = 0};
    consumer_args_t consumer2_args = {
        .loops = loops, .thread_id = 2, .name = "consumer", .consumed = 0, .last_consumed = -1, .wait_count = 0};

    int planned_produce = producer_args.loops;
    int planned_consume = consumer1_args.loops + consumer2_args.loops;
    debug_log("main", 0, "planned_produce=%d planned_consume=%d", planned_produce, planned_consume);
    if (planned_consume > planned_produce) {
        debug_log("main", 0, "WARNING: consumers want more items than producer provides; some consumer will block.");
    }

    pthread_create(&p1, NULL, producer, &producer_args);
    pthread_create(&c1, NULL, consumer, &consumer1_args);
    pthread_create(&c2, NULL, consumer, &consumer2_args);
    pthread_join(p1, NULL);
    pthread_join(c1, NULL);
    pthread_join(c2, NULL);

    debug_log("main", 0, "summary: produced=%d, c1_consumed=%d(last=%d), c2_consumed=%d(last=%d)",
              producer_args.produced, consumer1_args.consumed, consumer1_args.last_consumed, consumer2_args.consumed,
              consumer2_args.last_consumed);
    return 0;
}
