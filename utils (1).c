#include "utils.h"

struct msg prepare_message(int i_Index, float i_Temperature) {
    struct msg message;
    message.Index = i_Index;
    message.T = i_Temperature;
    return message;
}
