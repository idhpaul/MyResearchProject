#ifndef __MYWRAPPER_H
#define __MYWRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif


typedef struct MYRTP MyRTP;

MYRTP* newMYRTPClass();
void* deleteMYRTPClass(MYRTP* obj);
void MYRTP_parsing(MYRTP* obj, const char* packet_data, int packet_len, char* payload_data, int* payload_data_size);


// typedef struct MyClass MyClass;
// MyClass* newMyClass();
// void MyClass_int_set(MyClass* v, int i);
// int MyClass_int_get(MyClass* v);
// void deleteMyClass(MyClass* v);

#ifdef __cplusplus
}
#endif
#endif