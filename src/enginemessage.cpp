#include"enginemessage.h"
std::ostringstream EngineMessage::message("No Message");
std::ostringstream EngineMessage::simulationTree("No Message");
std::string EngineMessage::getMessage(){
        return message.str();
}
void EngineMessage::printMessageToConsole(){
        std::cout<<message.str()<<std::endl;
}

