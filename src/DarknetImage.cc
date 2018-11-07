//
// Created by Muscalu Stefan-Gabriel on 2018-11-07.
//

#include "DarknetImage.h"

using namespace Napi;
Napi::FunctionReference DarknetImage::constructor;

Object DarknetImage::Init(Napi::Env env, Object exports) {
	Napi::Function func = DefineClass(env, "DarknetImage", {
//			InstanceAccessor("width", &FFFrame::GetWidth, nullptr,
//											 napi_property_attributes::napi_enumerable),

			InstanceMethod("release", &DarknetImage::Release,
										 napi_property_attributes::napi_enumerable),
			InstanceMethod("toRGB", &DarknetImage::ToRGB,
										 napi_property_attributes::napi_enumerable),
			InstanceMethod("letterbox", &DarknetImage::LetterBox,
										 napi_property_attributes::napi_enumerable),

			StaticMethod("fromRGB", &DarknetImage::FromRGB,
									 (napi_property_attributes) (napi_property_attributes::napi_enumerable |
																							 napi_property_attributes::napi_static))
	});

	constructor = Napi::Persistent(func);
	constructor.SuppressDestruct();
	exports.Set("DarknetImage", func);
	return exports;
}


DarknetImage::DarknetImage(const Napi::CallbackInfo &info) : Napi::ObjectWrap<DarknetImage>(info) {

	_original_data = info[0].As<Float32Array>();

	_image.data = _original_data.Data();

	_image.w = info[1].ToNumber();
	_image.h = info[2].ToNumber();
	_image.c = info[3].ToNumber();
}

image const &DarknetImage::get_image() const {
	return _image;
}

Napi::Value DarknetImage::FromRGB(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();
	auto buff = info[0].As<Uint8Array>();
	uint32_t w = info[1].ToNumber();
	uint32_t h = info[2].ToNumber();
	uint32_t c = info[3].ToNumber();
	Function callback = info[4].As<Napi::Function>();

	auto *worker = new DarknetImageWorkers::RGB2DarknetImage(
			callback,
			buff,
			w, h, c
	);
	worker->Queue();
	return env.Undefined();
}

Napi::Value DarknetImage::LetterBox(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();
	uint32_t w = info[0].ToNumber();
	uint32_t h = info[1].ToNumber();
	Function callback = info[2].As<Napi::Function>();

	auto *worker = new DarknetImageWorkers::Letterbox(
			callback,
			this,
			w, h
	);
	worker->Queue();
	return env.Undefined();
}


