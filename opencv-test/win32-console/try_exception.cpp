#include "try_exception.h"
#include <string>
#include <eh.h> // exception handling, for vc++ by MSFT, need to use seh external library for gcc/llvm
#include <exception>

void make_exception() {
	// access violation address
    *(int*)0 = 0;
}

_se_translator_function prev = nullptr;
/* can also refer to MSDN sample
https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/set-se-translator?view=msvc-170
*/
void set() {
    prev = _set_se_translator([](unsigned int u, _EXCEPTION_POINTERS* pExp) {
        std::string error = "SEH Exception: ";
        switch (u) {
        case 0xc0000005:
            error += "Access Violation";
            break;
        case 0xc0000409:
            error += "Stack buffer overrun";
            break;
        default:
            char result[11];
            sprintf_s(result, 11, "0x%08X", u);
            error += result;
        };
        throw std::exception(error.c_str());
        });
}

void rollback() { _set_se_translator(prev); }

int access_violation() {
	// original idea is invoking signal stacks into call stacks, 
	// manually throwing exception when something goes wrong which native exception unwilling to catch

	// 1. vs must set `with SEH exception` in project property -> code gen -> enable c++ exception
	// 2. set se translator passing function pointer for exception translator(can catch/throw with std::exception)
	
    set();

    try {
        make_exception();
    }
    catch (std::exception& e) {
        printf("except: %s\n", e.what());
    }

    rollback();

    return 0;
}
