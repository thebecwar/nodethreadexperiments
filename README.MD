## Building the source:

- npm install node-gyp
- node-gyp configure
- node-gyp build

## Undocumented/Under-documented Gotchyas

### Persistents are not copyable by default

By default V8's `Persistent` template uses the `NonCopyablePersistentTraits`, which means you can't use them with C++ standard library collections, because you can't initialize the values in your collection. For example:

```
//#include <node.h>

std::map<std::string, v8::Persistent<v8::value>> persistents;
void SetValue(v8::Isolate* isolate, v8::Local<v8::Value>& val)
{
  std::string key("someKey");
  persistents.emplace(key, v8::Persistent<v8::Value>(isolate, val);
}
```

This will fail with a compiler error about not being able to convert from a `Primitive*` to a `Object*` (or similar). In order to enable the copy constructor, you have to define your collection using the copyable version of the template, as follows:

```
std::map<std::string, v8::Persistent<v8::Value, v8::CopyablePersistentTraits<Value>>> storageMap;
```

The documentation is pretty sparse on whether this will create issues further down the road. (Like handle leaks.)

### Constructing a new Isolate to run some code on a new thread is not well documented

_(note: the sample 'hello.cc' provided with the node.js source does show the setup.)_

The basic process is:
1. Initialize a `v8::Isolate::CreateParams` struct.
2. Set the `array_buffer_allocator` member with the result of `v8::ArrayBuffer::Allocator::NewDefaultAllocator()`. (This is created somewhere in the V8 code using the `new` keyword. It is your responsibility to `delete` it.)
3. Create a new `Isolate*` by calling `Isolate::New` with the create parameters.
4. Open a block to contain the next set of objects. They must all be disposed _before_ the `Isolate`, or you will segfault.
5. Create an `v8::Isolate::Scope`
6. Create a `v8::Locker` object. (This may not be strictly necessary, but I haven't tested it yet.)
7. Create a `v8::HandleScope`
8. Using the isolate, create a `Local<ObjectTemplate>` to store your globals.
9. Add whatever objects you need to the global scope.
10. Create a `v8::Local<v8::Context>` using `v8::Context::New(isolate, NULL, globals)`.
11. Create a `v8::Context::Scope`

You can now run whatever JS code you want on your new isolate.

To tear down, go in the opposite order of construction with two caveats. (If you started a block after creating the isolate, and are using RAII, the compiler will destruct the scope in the correct order.) First, once the scope is completely destroyed, call `Dispose()` on the isolate to free it's resources. Do not call delete on your own, you will cause issues.  Second, `delete` the `ArrayBuffer::Allocator*` you created in the params.

### Handle scopes

A handle scope is required before constructing a `v8::Local<>`. If you're calling into JS from within a callback registered with `uv_async_init`, you will need to create a `v8::HandleScope` using the isolate that you obtain from calling `v8::Isolate::GetCurrent()`. 


