#!/usr/bin/env python2.7
# coding=utf-8
# vim: ts=4 sw=4

import os
import errno
import time
import signal


def kill_operation(pid, signal):
    try:
        os.kill(pid, signal)
    except OSError as err:
        if err.errno == errno.EPERM or err.errno == errno.ESRCH:
            return False
    return True


pid = os.fork()
if pid == 0:
    print "in child process ..."
    time.sleep(5*60)
    os._exit(0)
else:
    print "parent process %d, sub process id %d" % (os.getpid(), pid)


max_sleep_time = 20
current_sleep_time = 0
sleep_interval = 5
existence = True
while existence:
    if (current_sleep_time < max_sleep_time):
        try:
            os.waitpid(pid, os.WNOHANG)
        except OSError as err:
            if err.errno == errno.ECHILD:
                break
        time.sleep(sleep_interval)
        current_sleep_time += sleep_interval
    else:
        print("kill 9\t\t" + str(kill_operation(pid, 15)))
    existence = kill_operation(pid, 0)
    print("kill 0\t\t" + str(existence))


exit(0)


