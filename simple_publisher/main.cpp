#include "Publisher.h"

int main()
{
    char* secret_key = std::getenv("PUBLISHER_SECRET");
    Publisher().Stream(secret_key);
    return 0;
}