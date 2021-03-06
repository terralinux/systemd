/*-*- Mode: C; c-basic-offset: 8; indent-tabs-mode: nil -*-*/

/***
  This file is part of systemd.

  Copyright 2010 Lennart Poettering

  systemd is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  systemd is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with systemd; If not, see <http://www.gnu.org/licenses/>.
***/

#include <errno.h>
#include <dbus/dbus.h>
#include <sys/prctl.h>

#include "dbus-execute.h"
#include "missing.h"
#include "ioprio.h"
#include "strv.h"
#include "dbus-common.h"

DEFINE_BUS_PROPERTY_APPEND_ENUM(bus_execute_append_kill_mode, kill_mode, KillMode);

DEFINE_BUS_PROPERTY_APPEND_ENUM(bus_execute_append_input, exec_input, ExecInput);
DEFINE_BUS_PROPERTY_APPEND_ENUM(bus_execute_append_output, exec_output, ExecOutput);

int bus_execute_append_env_files(DBusMessageIter *i, const char *property, void *data) {
        char **env_files = data, **j;
        DBusMessageIter sub, sub2;

        assert(i);
        assert(property);

        if (!dbus_message_iter_open_container(i, DBUS_TYPE_ARRAY, "(sb)", &sub))
                return -ENOMEM;

        STRV_FOREACH(j, env_files) {
                dbus_bool_t b = false;
                char *fn = *j;

                if (fn[0] == '-') {
                        b = true;
                        fn++;
                }

                if (!dbus_message_iter_open_container(&sub, DBUS_TYPE_STRUCT, NULL, &sub2) ||
                    !dbus_message_iter_append_basic(&sub2, DBUS_TYPE_STRING, &fn) ||
                    !dbus_message_iter_append_basic(&sub2, DBUS_TYPE_BOOLEAN, &b) ||
                    !dbus_message_iter_close_container(&sub, &sub2))
                        return -ENOMEM;
        }

        if (!dbus_message_iter_close_container(i, &sub))
                return -ENOMEM;

        return 0;
}

int bus_execute_append_oom_score_adjust(DBusMessageIter *i, const char *property, void *data) {
        ExecContext *c = data;
        int32_t n;

        assert(i);
        assert(property);
        assert(c);

        if (c->oom_score_adjust_set)
                n = c->oom_score_adjust;
        else {
                char *t;

                n = 0;
                if (read_one_line_file("/proc/self/oom_score_adj", &t) >= 0) {
                        safe_atoi(t, &n);
                        free(t);
                } else if (read_one_line_file("/proc/self/oom_adj", &t) >= 0) {
                        safe_atoi(t, &n);
                        free(t);

                        if (n == OOM_ADJUST_MAX)
                                n = OOM_SCORE_ADJ_MAX;
                        else
                                n = (n * OOM_SCORE_ADJ_MAX) / -OOM_DISABLE;
                }
        }

        if (!dbus_message_iter_append_basic(i, DBUS_TYPE_INT32, &n))
                return -ENOMEM;

        return 0;
}

int bus_execute_append_nice(DBusMessageIter *i, const char *property, void *data) {
        ExecContext *c = data;
        int32_t n;

        assert(i);
        assert(property);
        assert(c);

        if (c->nice_set)
                n = c->nice;
        else
                n = getpriority(PRIO_PROCESS, 0);

        if (!dbus_message_iter_append_basic(i, DBUS_TYPE_INT32, &n))
                return -ENOMEM;

        return 0;
}

int bus_execute_append_ioprio(DBusMessageIter *i, const char *property, void *data) {
        ExecContext *c = data;
        int32_t n;

        assert(i);
        assert(property);
        assert(c);

        if (c->ioprio_set)
                n = c->ioprio;
        else
                n = ioprio_get(IOPRIO_WHO_PROCESS, 0);

        if (!dbus_message_iter_append_basic(i, DBUS_TYPE_INT32, &n))
                return -ENOMEM;

        return 0;
}

int bus_execute_append_cpu_sched_policy(DBusMessageIter *i, const char *property, void *data) {
        ExecContext *c = data;
        int32_t n;

        assert(i);
        assert(property);
        assert(c);

        if (c->cpu_sched_set)
                n = c->cpu_sched_policy;
        else
                n = sched_getscheduler(0);

        if (!dbus_message_iter_append_basic(i, DBUS_TYPE_INT32, &n))
                return -ENOMEM;

        return 0;
}

int bus_execute_append_cpu_sched_priority(DBusMessageIter *i, const char *property, void *data) {
        ExecContext *c = data;
        int32_t n;

        assert(i);
        assert(property);
        assert(c);

        if (c->cpu_sched_set)
                n = c->cpu_sched_priority;
        else {
                struct sched_param p;
                n = 0;

                zero(p);
                if (sched_getparam(0, &p) >= 0)
                        n = p.sched_priority;
        }

        if (!dbus_message_iter_append_basic(i, DBUS_TYPE_INT32, &n))
                return -ENOMEM;

        return 0;
}

int bus_execute_append_affinity(DBusMessageIter *i, const char *property, void *data) {
        ExecContext *c = data;
        dbus_bool_t b;
        DBusMessageIter sub;

        assert(i);
        assert(property);
        assert(c);

        if (!dbus_message_iter_open_container(i, DBUS_TYPE_ARRAY, "y", &sub))
                return -ENOMEM;

        if (c->cpuset)
                b = dbus_message_iter_append_fixed_array(&sub, DBUS_TYPE_BYTE, &c->cpuset, CPU_ALLOC_SIZE(c->cpuset_ncpus));
        else
                b = dbus_message_iter_append_fixed_array(&sub, DBUS_TYPE_BYTE, &c->cpuset, 0);

        if (!b)
                return -ENOMEM;

        if (!dbus_message_iter_close_container(i, &sub))
                return -ENOMEM;

        return 0;
}

int bus_execute_append_timer_slack_nsec(DBusMessageIter *i, const char *property, void *data) {
        ExecContext *c = data;
        uint64_t u;

        assert(i);
        assert(property);
        assert(c);

        if (c->timer_slack_nsec_set)
                u = (uint64_t) c->timer_slack_nsec;
        else
                u = (uint64_t) prctl(PR_GET_TIMERSLACK);

        if (!dbus_message_iter_append_basic(i, DBUS_TYPE_UINT64, &u))
                return -ENOMEM;

        return 0;
}

int bus_execute_append_capability_bs(DBusMessageIter *i, const char *property, void *data) {
        ExecContext *c = data;
        uint64_t normal, inverted;

        assert(i);
        assert(property);
        assert(c);

        /* We store this negated internally, to match the kernel, bu
         * we expose it normalized. */

        normal = *(uint64_t*) data;
        inverted = ~normal;

        return bus_property_append_uint64(i, property, &inverted);
}

int bus_execute_append_capabilities(DBusMessageIter *i, const char *property, void *data) {
        ExecContext *c = data;
        char *t = NULL;
        const char *s;
        dbus_bool_t b;

        assert(i);
        assert(property);
        assert(c);

        if (c->capabilities)
                s = t = cap_to_text(c->capabilities, NULL);
        else
                s = "";

        if (!s)
                return -ENOMEM;

        b = dbus_message_iter_append_basic(i, DBUS_TYPE_STRING, &s);

        if (t)
                cap_free(t);

        if (!b)
                return -ENOMEM;

        return 0;
}

int bus_execute_append_rlimits(DBusMessageIter *i, const char *property, void *data) {
        ExecContext *c = data;
        int r;
        uint64_t u;

        assert(i);
        assert(property);
        assert(c);

        assert_se((r = rlimit_from_string(property)) >= 0);

        if (c->rlimit[r])
                u = (uint64_t) c->rlimit[r]->rlim_max;
        else {
                struct rlimit rl;

                zero(rl);
                getrlimit(r, &rl);

                u = (uint64_t) rl.rlim_max;
        }

        if (!dbus_message_iter_append_basic(i, DBUS_TYPE_UINT64, &u))
                return -ENOMEM;

        return 0;
}

int bus_execute_append_command(DBusMessageIter *i, const char *property, void *data) {
        ExecCommand *c = data;
        DBusMessageIter sub, sub2, sub3;

        assert(i);
        assert(property);

        if (!dbus_message_iter_open_container(i, DBUS_TYPE_ARRAY, "(sasbttuii)", &sub))
                return -ENOMEM;

        LIST_FOREACH(command, c, c) {
                char **l;
                uint32_t pid;
                int32_t code, status;

                if (!c->path)
                        continue;

                if (!dbus_message_iter_open_container(&sub, DBUS_TYPE_STRUCT, NULL, &sub2) ||
                    !dbus_message_iter_append_basic(&sub2, DBUS_TYPE_STRING, &c->path) ||
                    !dbus_message_iter_open_container(&sub2, DBUS_TYPE_ARRAY, "s", &sub3))
                        return -ENOMEM;

                STRV_FOREACH(l, c->argv)
                        if (!dbus_message_iter_append_basic(&sub3, DBUS_TYPE_STRING, l))
                                return -ENOMEM;

                pid = (uint32_t) c->exec_status.pid;
                code = (int32_t) c->exec_status.code;
                status = (int32_t) c->exec_status.status;

                if (!dbus_message_iter_close_container(&sub2, &sub3) ||
                    !dbus_message_iter_append_basic(&sub2, DBUS_TYPE_BOOLEAN, &c->ignore) ||
                    !dbus_message_iter_append_basic(&sub2, DBUS_TYPE_UINT64, &c->exec_status.start_timestamp.realtime) ||
                    !dbus_message_iter_append_basic(&sub2, DBUS_TYPE_UINT64, &c->exec_status.start_timestamp.monotonic) ||
                    !dbus_message_iter_append_basic(&sub2, DBUS_TYPE_UINT64, &c->exec_status.exit_timestamp.realtime) ||
                    !dbus_message_iter_append_basic(&sub2, DBUS_TYPE_UINT64, &c->exec_status.exit_timestamp.monotonic) ||
                    !dbus_message_iter_append_basic(&sub2, DBUS_TYPE_UINT32, &pid) ||
                    !dbus_message_iter_append_basic(&sub2, DBUS_TYPE_INT32, &code) ||
                    !dbus_message_iter_append_basic(&sub2, DBUS_TYPE_INT32, &status))
                        return -ENOMEM;

                if (!dbus_message_iter_close_container(&sub, &sub2))
                        return -ENOMEM;
        }

        if (!dbus_message_iter_close_container(i, &sub))
                return -ENOMEM;

        return 0;
}
