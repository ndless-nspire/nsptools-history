#include <windows.h>

class AppStreamCalc
{
    public:
        // Default constructor
        AppStreamCalc();

        ~AppStreamCalc();

        static DWORD WINAPI threadProc(void* param);

        void shutdownCalc();

        void sendInput(const char keyCode);

    private:
        bool runCalcThread();
		
		PROCESS_INFORMATION processInfo;
};
