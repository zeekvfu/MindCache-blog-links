#!/usr/bin/python2.7
# -*- coding: UTF-8 -*-


import multiprocessing
import multiprocessing.pool


class NoDaemonProcess(multiprocessing.Process):
    # make 'daemon' attribute always return False
    def _get_daemon(self):
        return False
    def _set_daemon(self, value):
        pass
    daemon = property(_get_daemon, _set_daemon)

# We sub-class multiprocessing.pool.Pool instead of multiprocessing.Pool
# because the latter is only a wrapper function, not a proper class.
class NoDaemonProcessPool(multiprocessing.pool.Pool):
    Process = NoDaemonProcess


def create_process_pool(index):
    print index
    li = range(3)
    pool = multiprocessing.Pool(processes = len(li))
    for sub_index in li:
        pool.apply_async(print_process_index, (index, sub_index))
    pool.close()
    pool.join()


def print_process_index(index, sub_index):
    print "%d-%d" %(index, sub_index)


li = range(3)
# pool = multiprocessing.Pool(processes = len(li))
pool = NoDaemonProcessPool(processes = len(li))
pool.map(create_process_pool, li)
# pool.map_async(create_process_pool, li)
# for index in li:
    # pool.apply(create_process_pool, [index])
    # pool.apply_async(create_process_pool, [index])
pool.close()
pool.join()


