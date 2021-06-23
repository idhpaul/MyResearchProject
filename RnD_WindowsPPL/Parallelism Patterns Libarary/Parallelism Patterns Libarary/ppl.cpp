#include <ppltasks.h>
#include <iostream>

using namespace concurrency;
using namespace std;

int wmain()
{
    auto t = create_task([]() -> int
        {
            return 0;
        });

    auto ADD5 = [](int n) {return n + 5; };
    auto MUL10 = [](int n) {return n * 10; };
    auto TRANSDouble = [](int n) { return double(n); };
    auto TRANSChar = [](double dn) {
        if (dn > 0)
            return std::string("양수");
        else if (dn <= 0)
            return std::string("음수");
    };

    auto res1 = t.then(ADD5).then(MUL10).then(TRANSDouble).then(TRANSChar);
    auto res2 = t.then(TRANSChar);

    std::cout << "res1 : " << res1.get() << std::endl;
    std::cout << "res2 : " << res2.get() << std::endl;

    // 입력된 정수를 +1 증가시키는 람다 함수 생성
    auto increment = [](int n) { return n + 1; };

    // task 체인을 실행하고 결과값을 출력
    auto result = t.then(increment).then(increment).then(increment).get();
    wcout << result << endl;
}




