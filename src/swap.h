/*-*- Mode: C; c-basic-offset: 8; indent-tabs-mode: nil -*-*/

#ifndef fooswaphfoo
#define fooswaphfoo

/***
  This file is part of systemd.

  Copyright 2010 Lennart Poettering
  Copyright 2010 Maarten Lankhorst

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

typedef struct Swap Swap;

#include "unit.h"

typedef enum SwapState {
        SWAP_DEAD,
        SWAP_ACTIVATING,
        SWAP_ACTIVE,
        SWAP_DEACTIVATING,
        SWAP_ACTIVATING_SIGTERM,
        SWAP_ACTIVATING_SIGKILL,
        SWAP_DEACTIVATING_SIGTERM,
        SWAP_DEACTIVATING_SIGKILL,
        SWAP_FAILED,
        _SWAP_STATE_MAX,
        _SWAP_STATE_INVALID = -1
} SwapState;

typedef enum SwapExecCommand {
        SWAP_EXEC_ACTIVATE,
        SWAP_EXEC_DEACTIVATE,
        _SWAP_EXEC_COMMAND_MAX,
        _SWAP_EXEC_COMMAND_INVALID = -1
} SwapExecCommand;

typedef struct SwapParameters {
        char *what;
        int priority;
        bool noauto:1;
        bool nofail:1;
        bool handle:1;
} SwapParameters;

struct Swap {
        Meta meta;

        char *what;

        SwapParameters parameters_etc_fstab;
        SwapParameters parameters_proc_swaps;
        SwapParameters parameters_fragment;

        bool from_etc_fstab:1;
        bool from_proc_swaps:1;
        bool from_fragment:1;

        bool failure:1;

        /* Used while looking for swaps that vanished or got added
         * from/to /proc/swaps */
        bool is_active:1;
        bool just_activated:1;

        usec_t timeout_usec;

        ExecCommand exec_command[_SWAP_EXEC_COMMAND_MAX];
        ExecContext exec_context;

        SwapState state, deserialized_state;

        ExecCommand* control_command;
        SwapExecCommand control_command_id;
        pid_t control_pid;

        Watch timer_watch;

        /* In order to be able to distinguish dependencies on
        different device nodes we might end up creating multiple
        devices for the same swap. We chain them up here. */

        LIST_FIELDS(struct Swap, same_proc_swaps);
};

extern const UnitVTable swap_vtable;

int swap_add_one(Manager *m, const char *what, const char *what_proc_swaps, int prio, bool no_auto, bool no_fail, bool handle, bool set_flags);

int swap_add_one_mount_link(Swap *s, Mount *m);

int swap_dispatch_reload(Manager *m);
int swap_fd_event(Manager *m, int events);

const char* swap_state_to_string(SwapState i);
SwapState swap_state_from_string(const char *s);

const char* swap_exec_command_to_string(SwapExecCommand i);
SwapExecCommand swap_exec_command_from_string(const char *s);

#endif
