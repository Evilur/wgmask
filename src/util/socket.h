#include <netinet/in.h>

class Socket final {
public:
    static constexpr int MTU = 1420;

    explicit Socket();

    void Bind(char* str) const;

    int Recieve(char* buffer) const;

private:
    int _socket_fd = -1;
};
