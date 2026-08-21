#define _CRT_SECURE_NO_WARNINGS  // 消除scanf/strcpy安全警告

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>   // 用于_getch()函数
#include <windows.h> // 用于控制台颜色控制

// 结构体
// 乘客信息结构体
typedef struct Customer {
    char name[50];          // 乘客姓名
    int ticketCount;        // 所需票数
    struct Customer* next;  // 指向下一个乘客（乘客链表）
} Customer, * CustomerPtr;

// 航班信息结构体
typedef struct Flight {
    // 1. 起降机场信息
    char startCity[50];     // 起点站名（如"北京"）
    char endCity[50];       // 终点站名（如"上海"）

    // 2. 航班标识信息
    char flightNumber[10];  // 航班号（如"CA1234"）

    // 3. 运营时间信息
    char dayOfWeek[10];     // 飞行周日（如"星期一"）
    char time[10];          // 航行时刻（如"09:30"）

    // 4. 票务信息
    int capacity;           // 乘员定额（总座位数）
    int remainingTickets;   // 当前余票量
    float price;            // 票价（单位：元）

    // 5. 乘客信息（乘客链表）
    CustomerPtr bookedList; // 已订票乘客名单
    CustomerPtr waitList;   // 候补用户名单

    struct Flight* next;    // 指向下一个航班（航班链表）
} Flight, * FlightPtr;

// 全局变量
Flight* flightList = NULL;  // 航班链表头指针
HANDLE hConsole;            // 控制台句柄（控制颜色）

// 函数声明
void initConsole();         // 初始化控制台
void printMenu();           // 打印主菜单
void printSeparator();      // 打印分隔线
void setColor(int color);   // 设置文字颜色
void printHeader(const char* title); // 打印带颜色的标题

// 核心功能函数
void addFlight();           // 添加航班
void deleteFlight();        // 删除航班
void searchFlight();        // 查询航班
void bookTicket();          // 办理订票
void returnTicket();        // 办理退票
void displayAllFlights();   // 显示所有航班
void saveToFile();          // 保存数据到文件
void loadFromFile();        // 从文件加载数据
void deleteSavedFlight();    //安全删除数据


// 工具函数
// 设置字体颜色
void setColor(int color){
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

// 打印分隔线
void printSeparator() {
    setColor(3); 
    for (int i = 0; i < 100; i++) printf("=");
    printf("\n");
    setColor(15);
}

// 打印带颜色的标题
void printHeader(const char* title) {
    system("cls");
    printSeparator();
    setColor(15); 
    printf("  %s \n", title);
    printSeparator();
    setColor(15);
}

// 初始化控制台
void initConsole() {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    system("title 航空客运订票系统"); // 设置窗口标题
    system("mode con cols=100 lines=30"); // 设置窗口大小
    system("cls"); // 清屏
}

// 一、录入新航线信息
void addFlight() {
    printHeader("添加航班信息");

    // 创建新航班节点
    Flight* newFlight = (Flight*)malloc(sizeof(Flight));
    if (newFlight == NULL) {
        setColor(12); // 红色错误提示
        printf("内存分配失败！无法添加航班。\n");
        setColor(15);
        _getch(); // 按任意键返回（_getch避免报错）
        return;
    }

    // 初始化新航班
    newFlight->bookedList = NULL;
    newFlight->waitList = NULL;
    newFlight->next = NULL;

    setColor(11);
    printf("请输入航班详细信息：\n");
    setColor(15);

    printf("起点站名: ");
    scanf("%s", newFlight->startCity);
    printf("终点站名: ");
    scanf("%s", newFlight->endCity);
    printf("航班号: ");
    scanf("%s", newFlight->flightNumber);
    printf("飞行周日(如:星期一): ");
    scanf("%s", newFlight->dayOfWeek);
    printf("航行时刻(如:09:30): ");
    scanf("%s", newFlight->time);
    printf("乘员定额: ");
    scanf("%d", &newFlight->capacity);
    newFlight->remainingTickets = newFlight->capacity; // 初始余票=总座位
    printf("票价(元): ");
    scanf("%f", &newFlight->price);

    // 添加到航班链表头部
    newFlight->next = flightList;
    flightList = newFlight;

    setColor(11); // 成功提示
    printf("\n航班添加成功！航班号：%s（%s→%s）\n",newFlight->flightNumber, newFlight->startCity, newFlight->endCity);
    setColor(15);
    _getch(); // 按任意键返回
}

//二、乘客查询航班
void searchFlight() {
    printHeader("航班查询");

    // 未录入航班信息
    if (flightList == NULL) {
        setColor(12);
        printf("暂无航班信息！\n");
        setColor(15);
        _getch();
        return;
    }

    // 查询
    char startCity[50], endCity[50];
    printf("请输入出发地: ");
    scanf("%s", startCity);
    printf("请输入目的地: ");
    scanf("%s", endCity);

    // 打印查询结果表头
    printf("\n");
    setColor(15);
    printf("查询结果（%s→%s）：\n", startCity, endCity);
    printSeparator();
    printf("%-10s %-8s %-8s %-8s %-8s %-8s\n","航班号", "飞行周日", "航行时刻", "乘员定额", "余票量", "票价(元)");
    printSeparator();
    setColor(15);

    Flight* current = flightList;
    int found = 0;

    // 遍历航班链表，查找匹配的航班
    while (current != NULL) {
        if (strcmp(current->startCity, startCity) == 0 && strcmp(current->endCity, endCity) == 0) {
            found = 1;
            // 根据余票量显示不同颜色
            if (current->remainingTickets == 0) setColor(12); // 无票红色
            else setColor(10); // 余票充足绿色

            // 按需求输出所有查询字段
            printf("%-10s %-8s %-8s %-8d %-8d %-8.1f\n",
                current->flightNumber,
                current->dayOfWeek,
                current->time,
                current->capacity,
                current->remainingTickets,
                current->price);
        }
        current = current->next;
    }

    if (!found) {
        setColor(12);
        printf("未找到从 %s 到 %s 的航班！\n", startCity, endCity);
    }
    setColor(7);
    printSeparator();
    printf("按任意键返回...\n");
    _getch();
}

// 三、乘客办理订票：订票功能 + 余票判断 + 候补推荐
void bookTicket() {
    printHeader("办理订票");

    char flightNum[10], name[50];
    int ticketCount;
    printf("请输入航班号: ");
    scanf("%s", flightNum);
    printf("请输入您的姓名: ");
    scanf("%s", name);
    printf("请输入订票数量: ");
    scanf("%d", &ticketCount);

    // 验证订票合法性
    if (flightList == NULL) {
        setColor(12);
        printf("暂无航班信息。\n");
        setColor(7);
        _getch();
        return;
    }

    if (ticketCount <= 0) {
        setColor(12);
        printf("订票数量需大于0。\n");
        setColor(7);
        _getch();
        return;
    }

    Flight* targetFlight = NULL;
    Flight* current = flightList;
    while (current != NULL) {
        if (strcmp(current->flightNumber, flightNum) == 0) {
            targetFlight = current;
            break;
        }
        current = current->next;
    }

    if (targetFlight == NULL) {
        setColor(12);
        printf("未找到航班号为 %s 的航班！\n", flightNum);
        setColor(7);
        _getch();
        return;
    }

    // 确认航班信息
    setColor(11);
    printf("\n航班信息确认：\n");
    printf("航线：%s→%s，航班号：%s\n",targetFlight->startCity, targetFlight->endCity, targetFlight->flightNumber);
    printf("飞行时间：%s %s，票价：￥%.2f\n",targetFlight->dayOfWeek, targetFlight->time, targetFlight->price);
    printf("当前余票：%d/%d（总座位）\n",targetFlight->remainingTickets, targetFlight->capacity);
    setColor(7);

    // 判断余票是否充足
    if (ticketCount <= targetFlight->remainingTickets) {
        // 余票充足，直接订票
        Customer* newCustomer = (Customer*)malloc(sizeof(Customer));
        if (newCustomer == NULL) {
            setColor(12);
            printf("内存分配失败，订票失败！\n");
            setColor(7);
            _getch();
            return;
        }
        strcpy(newCustomer->name, name);
        newCustomer->ticketCount = ticketCount;
        newCustomer->next = targetFlight->bookedList;
        targetFlight->bookedList = newCustomer;

        targetFlight->remainingTickets -= ticketCount;

        setColor(10);
        printf("\n订票成功！\n");
        printf("乘客：%s，票数：%d张，剩余余票：%d张\n",name, ticketCount, targetFlight->remainingTickets);
        setColor(7);
    }
    else {
        // 余票不足，先推荐同天、同路线其他航班
        setColor(14);
        printf("\n余票不足！当前余票：%d张，您需要：%d张\n",targetFlight->remainingTickets, ticketCount);

        // 查找航班
        Flight* alternatives[10];
        int altCount = 0;
        current = flightList;
        while (current != NULL && altCount < 10) {
            if (strcmp(current->dayOfWeek, targetFlight->dayOfWeek) == 0 &&
                strcmp(current->startCity, targetFlight->startCity) == 0 &&
                strcmp(current->endCity, targetFlight->endCity) == 0 &&
                strcmp(current->flightNumber, targetFlight->flightNumber) != 0 &&
                current->remainingTickets > 0) {
                alternatives[altCount++] = current;
            }
            current = current->next;
        }

        // 推荐航班
        // 有可推荐航班
        if (altCount > 0) {
            printf("\n为您推荐同天其他航班：\n");
            for (int i = 0; i < altCount; i++) {
                printf("[%d] 航班号：%s，时间：%s，余票：%d，票价：%.2f\n",i + 1, alternatives[i]->flightNumber, alternatives[i]->time,alternatives[i]->remainingTickets, alternatives[i]->price);
            }
            printf("[0] 不更换航班，加入候补队列\n");
            printf("请选择：");

            int choice;
            scanf("%d", &choice);
            if (choice > 0 && choice <= altCount) {
                // 选择推荐航班，提示重新订票
                setColor(10);
                printf("为您推荐航班 %s，重新进入订票即可。\n",alternatives[choice - 1]->flightNumber);
                setColor(7);
                _getch();
                return;
            }
        }
        // 无可推荐航班
        else {
            printf("\n当天无其他同路线航班可推荐。\n");
        }

        // 是否候补
        printf("\n是否加入候补队列？(1-是，0-否)：");
        int waitChoice;
        scanf("%d", &waitChoice);

        // 参与候补：加入链表
        if (waitChoice == 1) {
            Customer* newWaiter = (Customer*)malloc(sizeof(Customer));
            if (newWaiter == NULL) {
                setColor(12);
                printf("内存分配失败！\n");
                setColor(7);
                _getch();
                return;
            }
            strcpy(newWaiter->name, name);
            newWaiter->ticketCount = ticketCount;
            newWaiter->next = NULL;

            // 加入候补链表
            if (targetFlight->waitList == NULL) {
                targetFlight->waitList = newWaiter;
            }
            else {
                Customer* last = targetFlight->waitList;
                while (last->next != NULL) last = last->next;
                last->next = newWaiter;
            }

            setColor(10);
            printf("已加入候补队列，当前余票更新时将优先为您办理订票业务。\n");
            setColor(7);
        }
        // 拒绝候补：取消订票
        else {
            printf("已取消订票。\n");
        }
    }
    _getch();
}

// 四、乘客办理退票：资格验证 + 候补自动补位
void returnTicket() {
    printHeader("办理退票业务");

    if (flightList == NULL) {
        setColor(12);
        printf("暂无航班信息。\n");
        setColor(7);
        _getch();
        return;
    }

    char flightNum[10], name[50];
    int returnCount;
    printf("请输入退票航班号: ");
    scanf("%s", flightNum);
    printf("请输入乘客姓名: ");
    scanf("%s", name);
    printf("请输入退票数量: ");
    scanf("%d", &returnCount);

    if (returnCount <= 0) {
        setColor(12);
        printf("退票数量需大于0。\n");
        setColor(7);
        _getch();
        return;
    }

    // 寻找目标航班
    FlightPtr targetFlight = flightList;
    while (targetFlight != NULL && strcmp(targetFlight->flightNumber, flightNum) != 0) {
        targetFlight = targetFlight->next;
    }

    if (targetFlight == NULL) {
        setColor(12);
        printf("暂无航班信息。\n", flightNum);
        setColor(7);
        _getch();
        return;
    }

    // 验证退票资格：遍历已订票乘客链表
    CustomerPtr curr = targetFlight->bookedList;
    CustomerPtr prev = NULL;
    int success = 0;

    while (curr != NULL) {
        if (strcmp(curr->name, name) == 0) {
            if (curr->ticketCount >= returnCount) {
                targetFlight->remainingTickets += returnCount;
                curr->ticketCount -= returnCount;
                success = 1;
                printf("\n退票成功。\n");

                // 若该乘客退票至0，将其从链表剔除
                if (curr->ticketCount == 0) {
                    CustomerPtr toDelete = curr;   //记录要删除的乘客节点
                    if (prev == NULL)targetFlight->bookedList = curr->next;
                    else prev->next = curr->next;
                    free(toDelete);   // 先指向下一个乘客，再释放内存！！！！！
                    printf("\n退票成功，您已退还全部票额。\n");
                }
                 else {
                     printf("\n退票成功，您还剩余%d张票。\n",curr->ticketCount);
                 }
                break;
            }
            else {
                 setColor(12);
                 printf("查询到您只订购了 %d 张票，无法退还 %d 张票。\n", curr->ticketCount, returnCount);
                 setColor(7);
                 _getch();
                 return;
            }
        }
        prev = curr;
        curr = curr->next;
    }

    // 找不到该乘客
    if (!success) {
        setColor(12);
        printf("未查询到 [%s] 的订票记录。\n", name);
        setColor(7);
        _getch();
        return;
    }

    // 执行退票
    setColor(10);
    printf("\n[Done] 已释放 %d 张余票。\n", returnCount);
    setColor(7);

    // 候补自动补位逻辑：有余票 + 有候补
    printf("正在检查候补队列......\n");
    CustomerPtr wCurr = targetFlight->waitList;
    CustomerPtr wPrev = NULL;

    while (wCurr != NULL && targetFlight->remainingTickets > 0) {

        //若该候补乘客需票量不大于余票
        if (wCurr->ticketCount <= targetFlight->remainingTickets) {
            setColor(11); 
            printf("候补乘客 [%s] 已成功订购 %d 张票。\n",wCurr->name, wCurr->ticketCount);

            // 更新余票
            targetFlight->remainingTickets -= wCurr->ticketCount;

            // 从候补链表移除该乘客节点
            CustomerPtr toTransfer = wCurr;
            if (wPrev == NULL) targetFlight->waitList = wCurr->next;
            else wPrev->next = wCurr->next;

            // 将该乘客加入已订票链表
            toTransfer->next = targetFlight->bookedList;
            targetFlight->bookedList = toTransfer;

            // 移动指针到下一个候补：指针重置
            if (wPrev == NULL) wCurr = targetFlight->waitList;
            else wCurr = wPrev->next;
            setColor(7);
        }
        else {
            // 若该候补乘客需票量大于余票，按顺序检索下一位候补乘客
            wPrev = wCurr;
            wCurr = wCurr->next;
        }
    }
    printf("候补业务处理完毕，按任意键继续");
    _getch();
}

// 五、航班表
    void displayAllFlights(){
    printHeader("航班运行总览表");
    if (flightList == NULL) {
        printf("暂无航班信息。\n");
        _getch();
        return;
    }

    printf("%-10s %-16s %-10s %-8s %-6s %-6s %-10s\n",
        "航班号", "航线(起→终)", "周日", "时刻", "总票", "余票", "票价");
    printSeparator();

    FlightPtr f = flightList;
    while (f != NULL) {
        char route[40];
        sprintf(route, "%s-%s", f->startCity, f->endCity);
        printf("%-10s %-16s %-10s %-8s %-6d %-6d ￥%.2f\n",f->flightNumber, route, f->dayOfWeek, f->time,f->capacity, f->remainingTickets, f->price);
        f = f->next;
    }
    printSeparator();
    printf("按任意键回主菜单...");
    _getch();
}

// 六、安全删除航班数据：安全验证 + 内存释放
void deleteSavedFlight() {
    printHeader("删除已保存航班");

    // 检查是否有航班可删除
    if (flightList == NULL) {
        setColor(12); 
        printf("暂无航班信息。\n");
        setColor(7);
        _getch();
        return;
    }

    // 显示所有已保存航班，管理员选择需删除的航班
    setColor(14); 
    printf("【已保存航班列表】\n");
    printSeparator();
    printf("序号  航班号   航线(起→终)   飞行时间   余票/总票\n");
    printSeparator();
    setColor(7);

    // 临时数组存储航班指针：按序号删除选项
    FlightPtr flights[100]; 
    int flightCount = 0;
    FlightPtr current = flightList;

    //存储航班指针
    while (current != NULL && flightCount < 100) {
        flights[flightCount] = current; 
        char route[40];
        sprintf(route, "%s→%s", current->startCity, current->endCity);
        printf("[%d]   %-8s %-13s %s %-4s %d/%d\n",
            flightCount + 1,
            current->flightNumber,
            route,
            current->dayOfWeek,
            current->time,
            current->remainingTickets,
            current->capacity);
        current = current->next;
        flightCount++;
    }

    printSeparator();
    printf("请输入要删除的航班序号 (1-%d，输入0取消): ", flightCount);
    int choice;
    scanf("%d", &choice);

    // 验证选择合法性
    if (choice < 0 || choice > flightCount) {
        printf("输入错误，已取消删除操作。\n");
        _getch();
        return;
    }
    if (choice == 0) {
        printf("已取消删除操作。\n");
        _getch();
        return;
    }

    // 获取要删除的航班指针
    FlightPtr targetFlight = flights[choice - 1];

    // 二次确认，防止误删
    setColor(11); 
    printf("\n【删除确认】您确定要删除以下航班吗？\n");
    printf("航班号：%s\n", targetFlight->flightNumber);
    printf("航线：%s→%s\n", targetFlight->startCity, targetFlight->endCity);
    printf("飞行时间：%s %s\n", targetFlight->dayOfWeek, targetFlight->time);
    printf("总座位数：%d，当前余票：%d\n", targetFlight->capacity, targetFlight->remainingTickets);
    printf("票价：￥%.2f\n", targetFlight->price);

    // 统计该航班人数
    int bookedCount = 0, waitCount = 0;
    CustomerPtr cust = targetFlight->bookedList;
    while (cust != NULL) { bookedCount++; cust = cust->next; }
    cust = targetFlight->waitList;
    while (cust != NULL) { waitCount++; cust = cust->next; }
    printf("该航班有 %d 名已订票乘客，%d 名候补乘客\n", bookedCount, waitCount);
    printf("删除后所有相关数据将永久丢失！\n");
    setColor(7);

    // 最终确认
    printf("请再次确认是否删除 (1-确认删除，0-取消): ");
    int confirm;
    scanf("%d", &confirm);
    if (confirm != 1) {
        printf("已取消删除。\n");
        _getch();
        return;
    }

    // 执行删除操作
    // 从航班链表中移除该航班节点
    FlightPtr prev = NULL, curr = flightList;
    while (curr != NULL && curr != targetFlight) {
        prev = curr;
        curr = curr->next;
    }
    // 删除头结点
    if (prev == NULL) { 
        flightList = targetFlight->next;
    }
    // 删除其他节点
    else { 
        prev->next = targetFlight->next;
    }

    // 释放该航班相关乘客内存，防止内存泄漏
    // 释放已订票乘客
    CustomerPtr currCust = targetFlight->bookedList;
    while (currCust != NULL) {
        CustomerPtr temp = currCust;
        currCust = currCust->next;
        free(temp);
    }
    // 释放候补乘客
    currCust = targetFlight->waitList;
    while (currCust != NULL) {
        CustomerPtr temp = currCust;
        currCust = currCust->next;
        free(temp);
    }

    // 释放航班结点的内存
    free(targetFlight);

    // 删除成功
    setColor(10); 
    printf("\n删除成功！该航班及所有相关数据已彻底清除。\n");
    setColor(7);
    _getch();
}

// 零、保存数据！！！！！
void saveToFile() {
    FILE* fp = fopen("flights.dat", "wb");
    if (fp == NULL) return;

    FlightPtr curr = flightList;
    while (curr != NULL) {
        fwrite(curr, sizeof(Flight), 1, fp);
        curr = curr->next;
    }
    fclose(fp);
    setColor(10);
    printf("\n[Done] 数据已自动保存到本地。\n");
    setColor(7);
}

// 加载数据
void loadFromFile() {

    // 二进制只读模式("rb") 打开磁盘上的数据文件。
    FILE* fp = fopen("flights.dat", "rb");
    if (fp == NULL) return;

    //在栈(Stack)上开辟的临时结构体变量,负责承接从硬盘里读取的数据。
    Flight temp;

    // fread 按照 sizeof(Flight)，从文件中每次读取一个完整的结构体块数据,并存入私有的 temp 空间。
    // 若读取成功，返回值为 1，循环继续；直到读完文件末尾 EOF，循环停止。
    while (fread(&temp, sizeof(Flight), 1, fp)) {

        // 每读到一个航班，在堆(Heap)上通过 malloc 重新申请一块永久的内存。
        // 通过结构体直接赋值（浅拷贝），把 temp 里的数据克隆到新内存中。
        FlightPtr newF = (FlightPtr)malloc(sizeof(Flight));
        *newF = temp;

        // 保存在磁盘二进制文件里的 bookedList 和 waitList 实际上是上一次运行时的内存地址（指针）。
        // 当程序重启后，这些地址已经失效，变成“野指针”。
        // 因此，需要重置内存的指针。
        newF->bookedList = NULL;
        newF->waitList = NULL;
        newF->next = flightList;
        flightList = newF;
    }
    fclose(fp);
}

// 主函数
int main() {
    initConsole();
    loadFromFile(); // 启动时加载已保存数据

    int choice = -1;
    do {
        system("cls");
        setColor(11); // 浅蓝色标题
        printf("╔══════════════════════════════════════════════════════════╗\n");
        printf("║             Airline Booking 航空客运订票系统             ║\n");
        printf("╚══════════════════════════════════════════════════════════╝\n");
        setColor(7);
        printf("  [1] 管理员：录入新航线数据\n");
        printf("  [2] 乘  客：查询航班\n");
        printf("  [3] 乘  客：票务订购\n");
        printf("  [4] 乘  客：退票候补\n");
        printf("  [5] 航班表：显示所有航班状态\n");
        printf("  [6] 管理员：安全删除数据\n");
        printf("  [0] 安全退出并保存\n");
        printSeparator();
        printf("请输入您需要办理的业务 (0-6): ");

        if (scanf("%d", &choice) != 1) {
            fflush(stdin); // 清空调皮的非数字输入
            choice = -1;
        }

        switch (choice) {
        case 1: addFlight(); break;
        case 2: searchFlight(); break;
        case 3: bookTicket(); break;
        case 4: returnTicket(); break;
        case 5: displayAllFlights(); break;
        case 6:
            deleteSavedFlight(); // 调用删除函数
            break;
        case 0:
            saveToFile();
            printf("\n已安全退出并保存！\n");
            Sleep(1200);
            break;
        default:
            printf("输入错误，暂无业务信息，请输入序号0-6。\n");
            Sleep(1000);
        }
    } while (choice != 0);

    return 0; // 整个项目圆满结束！
}