#include "JsWorkItem.h"

namespace threadpool
{
	using node::ObjectWrap;
	using namespace v8;

	// Static members
	v8::Persistent<v8::Function> JsWorkItem::constructor;

	void JsWorkItem::Init(Local<Object> exports)
	{
		Isolate* isolate = exports->GetIsolate();

		Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, JsWorkItem::New);
		tpl->SetClassName(String::NewFromUtf8(isolate, "JsWorkItem"));
		tpl->InstanceTemplate()->SetInternalFieldCount(1);

		NODE_SET_PROTOTYPE_METHOD(tpl, "runSync", JsWorkItem::RunSync);

		tpl->InstanceTemplate()->SetAccessor(
			String::NewFromUtf8(isolate, "script"),
			JsWorkItem::GetScript);

		JsWorkItem::constructor.Reset(isolate, tpl->GetFunction());
		exports->Set(String::NewFromUtf8(isolate, "JsWorkItem"), tpl->GetFunction());
	}
	void JsWorkItem::New(const FunctionCallbackInfo<Value>& args)
	{
		Isolate* isolate = args.GetIsolate();
		if (!args.IsConstructCall())
		{
			isolate->ThrowException(Exception::SyntaxError(String::NewFromUtf8(isolate, "Must be constructed with new")));
			return;
		}

		if (args.Length() == 0 || !args[0]->IsString())
		{
			isolate->ThrowException(Exception::SyntaxError(String::NewFromUtf8(isolate, "argument 0: script as string")));
			return;
		}

		String::Utf8Value val(args[0]->ToString(isolate));
		std::string script(*val);

		JsWorkItem* self = new JsWorkItem(script);
		self->Wrap(args.This());
		args.GetReturnValue().Set(args.This());
	}

	// JS Instance Methods
	void JsWorkItem::RunSync(const FunctionCallbackInfo<Value>& args)
	{
		Local<Context> context = args.GetIsolate()->GetCurrentContext();
		JsWorkItem* self = ObjectWrap::Unwrap<JsWorkItem>(args.Holder());
		Local<Value> result = self->Execute(context);
		args.GetReturnValue().Set(result);
	}

	// JS Properties
	void JsWorkItem::GetScript(Local<String> property, const PropertyCallbackInfo<Value>& info)
	{
		JsWorkItem* self = ObjectWrap::Unwrap<JsWorkItem>(info.Holder());
		info.GetReturnValue().Set(String::NewFromUtf8(info.GetIsolate(), self->m_script.c_str()));
	}
	
	// Internal Instance Methods
	JsWorkItem::JsWorkItem(std::string& script) : m_script(script)
	{
	}

	Local<Value> JsWorkItem::Execute(Local<Context>& context)
	{
		Isolate* isolate = context->GetIsolate();

		bool ok = this->Compile(context);
		if (!ok)
		{
			// todo: decide what to do with errors
		}

		Local<Script> script = this->m_compiledScripts[isolate].Get(isolate);
		Local<Value> result = script->Run(context).ToLocalChecked();

		return result;
	}

	bool JsWorkItem::Compile(Local<Context>& context)
	{
		Isolate* isolate = context->GetIsolate();

		auto pcs = this->m_compiledScripts.find(isolate);
		if (pcs == this->m_compiledScripts.end())
		{
			Local<String> source = String::NewFromUtf8(isolate, this->m_script.c_str());
			Local<Script> script = Script::Compile(context, source).ToLocalChecked();

			// todo: check for errors

			//this->m_compiledScripts[isolate] = Persistent<Script>();
			this->m_compiledScripts[isolate].Reset(isolate, script);
		}

		return true;
	}
}