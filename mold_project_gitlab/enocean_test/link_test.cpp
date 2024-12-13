#include <iostream>

#include <eoLink.h>

int main(int /*argc*/, char* /*argv*/ []) {
  eoGateway myGateway;
  std::cout << "Opening Connection to ttyAMA0" << std::endl;
  if (myGateway.Open("/dev/ttyAMA0") != EO_OK) {
    std::cerr << "Failed to open ttyAMA0" << std::endl;
    return 1;
  }
  myGateway.LearnMode = true;
  eoSerialCommand cmd(&myGateway);
  // Read Sw and HW version of the device
  {
    CO_RD_VERSION_RESPONSE version;
    if (cmd.ReadVersion(version) != EO_OK) {
      std::cerr << "Failed to read ReadVersion" << std::endl;
      return 2;
    }
    // Failed to read the version of the device
    std::cout << "read version:" << version.appDescription << " " << std::dec
              << static_cast<int>(version.appVersion[0]) << "." << std::dec
              << static_cast<int>(version.appVersion[1]) << "." << std::dec
              << static_cast<int>(version.appVersion[2]) << "." << std::dec
              << static_cast<int>(version.appVersion[3]) << " ID:0x" << std::hex
              << version.chipID << std::endl;
  }
  std::uint32_t base_id{};
  {
    CO_RD_IDBASE_RESPONSE idbase;
    if (cmd.ReadIDBase(idbase) != EO_OK) {
      std::cerr << "Failed to read ReadIDBase" << std::endl;
      return 3;
    }
    // Failed to read the version of the device
    std::cout << "read idbase, baseID:" << std::hex << idbase.baseID
              << " remainingWrites:" << std::dec
              << static_cast<int>(idbase.remainingWrites) << std::endl;
    base_id = idbase.baseID;
  }
  myGateway.TeachInModule->SetRPS(0x02, 0x01);
  myGateway.TeachInModule->Set1BS(0x00, 0x01);

  std::cout << "doiing test!" << std::endl;
  eoMessage myMessage = eoMessage(1);
  //            myMessage.RORG = RORG_1BS;
  myMessage.RORG = RORG_RPS;
  myMessage.sourceID = base_id + 1000;
  std::cout << "myMessage.sourceID:" << std::hex << myMessage.sourceID
            << std::endl;
  if (myGateway.Send(myMessage) != EO_OK) {
    std::cerr << "myGateway.Send(myMessage) != EO_OK" << std::endl;
  }

  return 0;
}
