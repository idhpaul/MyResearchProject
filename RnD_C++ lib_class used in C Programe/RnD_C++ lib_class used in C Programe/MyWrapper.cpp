//#include "MyClass.h"
#include "mslm_rtp.h"
#include "MyWrapper.h"

extern "C" {

        MYRTP* newMYRTPClass()
        {
                return new MYRTP();
        }
        void* deleteMYRTPClass(MYRTP* obj)
        {
                delete obj;
        }

        void MYRTP_parsing(MYRTP* obj, const char* packet_data, int packet_len, char* payload_data, int* payload_data_size)
        {
                obj->MYRTP_parsing(packet_data, packet_len, payload_data, payload_data_size);
        }


        // MyClass* newMyClass() {
        //         return new MyClass();
        // }

        // void MyClass_int_set(MyClass* v, int i) {
        //         v->int_set(i);
        // }

        // int MyClass_int_get(MyClass* v) {
        //         return v->int_get();
        // }

        // void deleteMyClass(MyClass* v) {
        //         delete v;
        // }
}