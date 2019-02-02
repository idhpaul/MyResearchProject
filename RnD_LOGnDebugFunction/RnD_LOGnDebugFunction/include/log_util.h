#ifndef __LOG_UTIL__
#define __LOG_UTIL__

#define LOG_LVL_TRACE   50
#define LOG_LVL_DEBUG   40
#define LOG_LVL_INFO    30
#define LOG_LVL_WARNING 20
#define LOG_LVL_ERROR   10
#define LOG_LVL_FATAL   0

/*
* ������ level���� LOG�� ���������� ���������� üũ�ϴ� macro
*/
#define LOG_IS_TRACE    (LOGgetLevel() >= LOG_LVL_TRACE)
#define LOG_IS_DEBUG    (LOGgetLevel() >= LOG_LVL_DEBUG)
#define LOG_IS_INFO     (LOGgetLevel() >= LOG_LVL_INFO)
#define LOG_IS_WARNING  (LOGgetLevel() >= LOG_LVL_WARNING)
#define LOG_IS_ERROR    (LOGgetLevel() >= LOG_LVL_ERROR)
#define LOG_IS_FATAL    (LOGgetLevel() >= LOG_LVL_FATAL)

int LOGsetInfo(const char *dir, const char *prefix);
int LOGlogging(char log_type, const char *src_file, const char *func, int line_no, const char *fmt, ...);
int LOGsetLevel(int log_lvl);
int LOGgetLevel(void);

/*
* �Լ��� ���� ���۰� ���Ḧ log�� �����ϴ� macro �Լ�
*/
#define LOG_CALL(func)\
	LOG_TRACE("%s #### starting...", #func);\
	func;\
	LOG_TRACE("%s #### end.", #func)

/*
* Trace Log�� �����ϴ� macro �Լ�
*/
#define LOG_TRACE(...) \
    do { \
        if(LOG_IS_TRACE) { \
            LOGlogging('T', __FILE__, __func__, __LINE__, __VA_ARGS__);\
        } \
    } while(0)

/*
* debug Log�� �����ϴ� macro �Լ�
*/
#define LOG_DEBUG(...) \
    do { \
        if(LOG_IS_DEBUG) { \
            LOGlogging('D', __FILE__, __func__, __LINE__, __VA_ARGS__);\
        } \
    } while(0)

/*
* �߿� ���� Log�� �����ϴ� macro �Լ�
*/
#define LOG_INFO(...) \
    do { \
        if(LOG_IS_INFO) { \
            LOGlogging('I', __FILE__, __func__, __LINE__, __VA_ARGS__);\
        } \
    } while(0)

/*
* warning Log�� �����ϴ� macro �Լ�
*/
#define LOG_WARNING(...) \
    do { \
        if(LOG_IS_WARNING) { \
            LOGlogging('W', __FILE__, __func__, __LINE__, __VA_ARGS__);\
        } \
    } while(0)

/*
* error Log�� �����ϴ� macro �Լ�
*/
#define LOG_ERROR(...) \
    do { \
        if(LOG_IS_ERROR) { \
            LOGlogging('E', __FILE__, __func__, __LINE__, __VA_ARGS__);\
        } \
    } while(0)


/*
* fatal error Log�� �����ϴ� macro �Լ�
*/
#define LOG_FATAL(...) \
    do { \
        if(LOG_IS_FATAL) { \
            LOGlogging('F', __FILE__, __func__, __LINE__, __VA_ARGS__);\
        } \
    } while(0)

#endif // __LOG_UTIL__
