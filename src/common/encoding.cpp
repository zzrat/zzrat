/*
 * Copyright (c) 2024, zz
 * All rights reserved.
 *
 * Author: zz <3875657991@qq.com>
 */

#include "common/encoding.h"

#include <codecvt>
#include <locale>

template<class Facet>
struct deletable_facet : Facet
{
	template<class ...Args>
	deletable_facet(Args&& ...args)
		: Facet(std::forward<Args>(args)...) {}
	~deletable_facet() {}
};
using mbs_facet_t = deletable_facet<std::codecvt_byname<wchar_t, char, std::mbstate_t>>;

#if defined(_WIN32) || defined(_WIN64)
const char* EncodingConversion::GBK_LOCALE_NAME = ".936";
#else
const char* EncodingConversion::GBK_LOCALE_NAME = "zh_CN.GBK";
#endif

std::string EncodingConversion::ToString(const std::wstring& wstr)
{
	//std::locale::global(std::locale(""));
	const mbs_facet_t& cvt = std::use_facet<mbs_facet_t>(std::locale());
	std::wstring_convert<mbs_facet_t> converter(&cvt);
	std::string str = converter.to_bytes(wstr);
	return str;
}

std::wstring EncodingConversion::ToWString(const std::string& str)
{
	//std::locale::global(std::locale(""));
	const mbs_facet_t& cvt = std::use_facet<mbs_facet_t>(std::locale());
	std::wstring_convert<mbs_facet_t> converter(&cvt);
	std::wstring wstr = converter.from_bytes(str);
	return wstr;
}

std::string EncodingConversion::ToGBK(const std::wstring& wstr)
{
	std::wstring_convert<mbs_facet_t> conv(new mbs_facet_t(GBK_LOCALE_NAME));
	std::string  str = conv.to_bytes(wstr);
	return str;
}

std::wstring EncodingConversion::FromGBK(const std::string& str)
{
	std::wstring_convert<mbs_facet_t> conv(new mbs_facet_t(GBK_LOCALE_NAME));
	std::wstring wstr = conv.from_bytes(str);
	return wstr;
}

std::string EncodingConversion::ToUTF8(const std::wstring& wstr)
{
#if defined(__CYGWIN__) || defined(__MINGW32__) || defined(__MINGW64__)
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	return convert.to_bytes((char16_t*)wstr.data(), (char16_t*)wstr.data() + wstr.size());
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
	std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
	return convert.to_bytes(wstr.data(), wstr.data() + wstr.size());
#elif defined(_WIN32) || defined(_WIN64)
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
	return convert.to_bytes(wstr.data(), wstr.data() + wstr.size());
#endif
}

std::wstring EncodingConversion::FromUTF8(const std::string& str)
{
#if defined(__CYGWIN__) || defined(__MINGW32__) || defined(__MINGW64__)
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	auto tmp = convert.from_bytes(str.data(), str.data() + str.size());
	return std::wstring(tmp.data(), tmp.data() + tmp.size());
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
	std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
	return convert.from_bytes(str.data(), str.data() + str.size());
#elif defined(_WIN32) || defined(_WIN64)
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
	return convert.from_bytes(str.data(), str.data() + str.size());
#endif
}

std::string EncodingConversion::GBKToUTF8(const std::string& str)
{
	return ToUTF8(FromGBK(str));
}

std::string EncodingConversion::UTF8ToGBK(const std::string& str)
{
	return ToGBK(FromUTF8(str));
}

std::u16string EncodingConversion::UTF8toUTF16(const std::string& str)
{
#if defined(_MSC_VER)
	std::wstring_convert<std::codecvt_utf8_utf16<uint16_t>, uint16_t> convert;
	auto tmp = convert.from_bytes(str.data(), str.data() + str.size());
	return std::u16string(tmp.data(), tmp.data() + tmp.size());
#else
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	return convert.from_bytes(str.data(), str.data() + str.size());
#endif
}

std::u32string EncodingConversion::UTF8toUTF32(const std::string& str)
{
#if defined(_MSC_VER)
	std::wstring_convert<std::codecvt_utf8<uint32_t>, uint32_t> convert;
	auto tmp = convert.from_bytes(str.data(), str.data() + str.size());
	return std::u32string(tmp.data(), tmp.data() + tmp.size());
#else
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
	return convert.from_bytes(str.data(), str.data() + str.size());
#endif
}

std::string EncodingConversion::UTF16toUTF8(const std::u16string& str)
{
#if defined(_MSC_VER)
	std::wstring_convert<std::codecvt_utf8_utf16<uint16_t>, uint16_t> convert;
	return convert.to_bytes((uint16_t*)str.data(), (uint16_t*)str.data() + str.size());
#else
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	return convert.to_bytes(str.data(), str.data() + str.size());
#endif
}

std::u32string EncodingConversion::UTF16toUTF32(const std::u16string& str)
{
	std::string bytes;
	bytes.reserve(str.size() * 2);

	for (const char16_t ch : str)
	{
		bytes.push_back((uint8_t)(ch / 256));
		bytes.push_back((uint8_t)(ch % 256));
	}

#if defined(_MSC_VER)
	std::wstring_convert<std::codecvt_utf16<uint32_t>, uint32_t> convert;
	auto tmp = convert.from_bytes(bytes);
	return std::u32string(tmp.data(), tmp.data() + tmp.size());
#else
	std::wstring_convert<std::codecvt_utf16<char32_t>, char32_t> convert;
	return convert.from_bytes(bytes);
#endif
}

std::string EncodingConversion::UTF32toUTF8(const std::u32string& str)
{
#if defined(_MSC_VER)
	std::wstring_convert<std::codecvt_utf8<uint32_t>, uint32_t> convert;
	return convert.to_bytes((uint32_t*)str.data(), (uint32_t*)str.data() + str.size());
#else
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
	return convert.to_bytes(str.data(), str.data() + str.size());
#endif
}

std::u16string EncodingConversion::UTF32toUTF16(const std::u32string& str)
{
#if defined(_MSC_VER)
	std::wstring_convert<std::codecvt_utf16<uint32_t>, uint32_t> convert;
	std::string bytes = convert.to_bytes((uint32_t*)str.data(), (uint32_t*)str.data() + str.size());
#else
	std::wstring_convert<std::codecvt_utf16<char32_t>, char32_t> convert;
	std::string bytes = convert.to_bytes(str.data(), str.data() + str.size());
#endif

	std::u16string result;
	result.reserve(bytes.size() / 2);

	for (size_t i = 0; i < bytes.size(); i += 2)
		result.push_back((char16_t)((uint8_t)(bytes[i]) * 256 + (uint8_t)(bytes[i + 1])));

	return result;
}


