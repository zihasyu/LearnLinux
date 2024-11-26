#include "FileThreadRW.h"
#include "unistd.h"
#include <stdio.h>

int main()
{
    char str[32] = "UESTCLinuxProgramingCourses\n";
    char str1[29] = "ThisisFileOperationTest\n";
    FileThread *wThread = new FileWriteThread(str);
    FileThread *wThread1 = new FileWriteThread(str1);
    FileThread *rThread = new FileReadThread(12);
    FileThread *rThread1 = new FileReadThread(15);

    wThread->Run();
    // sleep(1);
    wThread1->Run();
    rThread->Run();
    rThread1->Run();

    wThread->WaitForDeath();
    wThread1->WaitForDeath();
    rThread->WaitForDeath();
    rThread1->WaitForDeath();

    delete wThread;
    delete wThread1;
    delete rThread;
    delete rThread1;
    return 0;
}
