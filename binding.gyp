{
    'target_defaults': {
        'defines': [
            
        ],
        'include_dirs': [
            
        ],
    },
    'targets': [
        {
            'target_name': 'libuvwrap',
            #'type': 'shared_library',
            'include_dirs': [ 'libuv/include' ],
            'sources': [
                'libuv/src/UVAsync.cc',
                'libuv/src/UVBarrier.cc',
                'libuv/src/UVCondition.cc',
                'libuv/src/UVIdleCallback.cc',
                'libuv/src/UVMutex.cc',
                'libuv/src/UVRwLock.cc',
                'libuv/src/UVSemaphore.cc',
                'libuv/src/UVThread.cc',
                'libuv/src/UVTimer.cc',
                'libuv/src/UVTls.cc'],
        }
    ]
}