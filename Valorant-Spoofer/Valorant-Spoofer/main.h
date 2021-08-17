#pragma once
#include <iostream>
#include <windows.h>
#include <string.h>
#include <ctime>
#include <random>
#include <string>
#include <malloc.h>
#include <stdio.h>
#include <vector>
#include <sstream>
#include <string>
#include <thread>
#include "xor.h"
using namespace std;

namespace utils {
    std::string randstr(std::string::size_type length)
    {
        static auto& chrs = "0123456789"
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        thread_local static std::mt19937 rg{ std::random_device{}() };
        thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);
        std::string s;
        s.reserve(length);
        while (length--)
            s += chrs[pick(rg)];
        return s;
    }
    std::string randnum(std::string::size_type length)
    {
        static auto& chrs = "0123456789";
        thread_local static std::mt19937 rg{ std::random_device{}() };
        thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);
        std::string s;
        s.reserve(length);
        while (length--)
            s += chrs[pick(rg)];
        return s;
    }
    std::string utf8_encode(const std::wstring& wstr)
    {
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
        std::string strTo(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
        return strTo;
    }
    template< typename ... Args >
    std::string stringer(Args const& ... args)
    {
        std::ostringstream stream;
        using List = int[];
        (void)List {
            0, ((void)(stream << args), 0) ...
        };
        return stream.str();
    }
}
namespace elements {
    string bvc = utils::stringer(_xor_("amidewinx64 /IV "), utils::randnum(4));
    string svc = utils::stringer(_xor_("amidewinx64 /SV "), utils::randnum(4));
    string ssnc = utils::stringer(_xor_("amidewinx64 /SS "), utils::randnum(11));
    string suc = utils::stringer(_xor_("amidewinx64 /SU "), utils::randstr(32));
    string ssc = utils::stringer(_xor_("amidewinx64 /SK "), utils::randnum(4));
    string sfc = utils::stringer(_xor_("amidewinx64 /SF "), utils::randnum(11));
    string bvcc = utils::stringer(_xor_("amidewinx64 /BV "), utils::randnum(4));
    string bsnc = utils::stringer(_xor_("amidewinx64 /BS "), utils::randnum(11));
    string bvhc = utils::stringer(_xor_("amidewinx64 /BVH "), utils::randnum(4));
    string bsnhc = utils::stringer(_xor_("amidewinx64 /BSH "), utils::randnum(11));
    string ctc = utils::stringer(_xor_("amidewinx64 /CT "), utils::randstr(3));
    string cvc = utils::stringer(_xor_("amidewinx64 /CV "), utils::randnum(4));
    string csnc = utils::stringer(_xor_("amidewinx64 /CS "), utils::randnum(11));
    string ctnc = utils::stringer(_xor_("amidewinx64 /CA "), utils::randnum(4));
    string cskc = utils::stringer(_xor_("amidewinx64 /CSK "), utils::randnum(4));
    string cthc = utils::stringer(_xor_("amidewinx64 /CTH "), utils::randnum(3));
    string cvhc = utils::stringer(_xor_("amidewinx64 /CVH "), utils::randnum(4));
    string csnhc = utils::stringer(_xor_("amidewinx64 /CSH "), utils::randnum(11));
    string ctnhc = utils::stringer(_xor_("amidewinx64 /CAH "), utils::randnum(4));
    string cskhc = utils::stringer(_xor_("amidewinx64 /CSKH "), utils::randnum(4));
    string psnc = utils::stringer(_xor_("amidewinx64 /PSN "), utils::randnum(11));
    string ppnc = utils::stringer(_xor_("amidewinx64 /PPN "), utils::randnum(4));

    string commands[] = { elements::bvc, elements::svc, elements::ssnc,
        elements::suc, elements::ssc, elements::sfc, elements::bvcc, elements::bsnc,
        elements::bvhc, elements::bsnhc, elements::ctc, elements::cvc,
        elements::csnc, elements::ctnc, elements::cskc, elements::cthc,
        elements::cvhc, elements::csnhc, elements::ctnhc, elements::cskhc,
        elements::psnc, elements::ppnc
    };
}