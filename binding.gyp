{
    'target_defaults': {
        'defines': [
            
        ],
        'include_dirs': [
            
        ],
    },
    'targets': [
        {
            'target_name': 'experiments',
            'include_dirs': [ 'libuv/include', 'threadpool/include', 'sharedobj/include', 'include' ],
            'sources': [
                # libuv wrapper classes
                'libuv/src/UVAsync.cc',
                'libuv/src/UVBarrier.cc',
                'libuv/src/UVCondition.cc',
                'libuv/src/UVIdleCallback.cc',
                'libuv/src/UVMutex.cc',
                'libuv/src/UVRwLock.cc',
                'libuv/src/UVSemaphore.cc',
                'libuv/src/UVSharedLib.cc',
                'libuv/src/UVThread.cc',
                'libuv/src/UVTimer.cc',
                'libuv/src/UVTls.cc',
                
                # threadpool classes
                'threadpool/src/ThreadPool.cc',
                'threadpool/src/JsWorkerThread.cc',
                'threadpool/src/JsWorkItem.cc',
                
                # Shared Object type
                'sharedobj/src/Variant.cc',
                'sharedobj/src/SharedObject.cc',
                'sharedobj/src/ObjectKey.cc',

                # Shared & Initialization
                'src/main.cc',
                'src/helpers.cc',
                ],
        }
    ]
}