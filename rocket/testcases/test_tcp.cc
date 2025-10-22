#include "../rocket/common/log.h"
#include "../rocket/net/tcp/net_addr.h"
#include "../rocket/common/config.h"

int main() {

  rocket::Config::SetGlobalConfiger("../config/rocket.xml");
  rocket::Logger::InitLogger();

    rocket::IPNetAddr addr("127.0.0.1", 1234);
    INFOLOG("create addr %s", addr.toString().c_str());
  
}