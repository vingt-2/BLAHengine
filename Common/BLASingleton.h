#define BLA_DECLARE_SINGLETON(CLASSNAME)                                                    \
public:                                                                                     \
    static CLASSNAME* s_blaSingletonInstance;                                               \
    static void AssignSingletonInstance(CLASSNAME* instance);                               \
    static CLASSNAME* AssignAndReturnSingletonInstance(CLASSNAME* instance);                \
    static CLASSNAME* GetSingletonInstance();                                               \
    static const CLASSNAME* GetSingletonInstanceRead();                                     \
private:

#define BLA_DECLARE_EXPORTED_ACCESS_SINGLETON(CLASSNAME)                                    \
public:                                                                                     \
    static CLASSNAME* s_blaSingletonInstance;                                               \
    static void AssignSingletonInstance(CLASSNAME* instance);                               \
    static CLASSNAME* AssignAndReturnSingletonInstance(CLASSNAME* instance);                \
    BLACORE_API static CLASSNAME* GetSingletonInstance();                                   \
    BLACORE_API static const CLASSNAME* GetSingletonInstanceRead();                         \
private:

#define BLA_DECLARE_EXPORTED_SINGLETON(CLASSNAME)                                           \
public:                                                                                     \
    static CLASSNAME* s_blaSingletonInstance;                                               \
    BLACORE_API static void AssignSingletonInstance(CLASSNAME* instance);                   \
    BLACORE_API static CLASSNAME* AssignAndReturnSingletonInstance(CLASSNAME* instance);    \
    BLACORE_API static CLASSNAME* GetSingletonInstance();                                   \
    BLACORE_API static const CLASSNAME* GetSingletonInstanceRead();                         \
private:

#define BLA_IMPLEMENT_SINGLETON(CLASSNAME)                                                  \
    CLASSNAME* CLASSNAME::s_blaSingletonInstance = nullptr;                                 \
    CLASSNAME* CLASSNAME::GetSingletonInstance(){ return s_blaSingletonInstance; }          \
    const CLASSNAME* CLASSNAME::GetSingletonInstanceRead() { return s_blaSingletonInstance;}\
    void CLASSNAME::AssignSingletonInstance(CLASSNAME* instance){                           \
        if(s_blaSingletonInstance == nullptr)                                               \
           { s_blaSingletonInstance = instance; } }                                         \
    CLASSNAME* CLASSNAME::AssignAndReturnSingletonInstance(CLASSNAME* instance){            \
        if(s_blaSingletonInstance == nullptr)                                               \
                   { s_blaSingletonInstance = instance; }                                   \
        return s_blaSingletonInstance; }

#define BLA_IMPLEMENT_CONSTRUCT_TEMPLATED_SINGLETON(CLASSNAME, ConstructorCall)                            \
    template<> CLASSNAME* CLASSNAME::s_blaSingletonInstance = new ConstructorCall;                         \
    template<> CLASSNAME* CLASSNAME::GetSingletonInstance(){ return s_blaSingletonInstance; }              \
    template<> const CLASSNAME* CLASSNAME::GetSingletonInstanceRead() { return s_blaSingletonInstance;}    \
    template<> void CLASSNAME::AssignSingletonInstance(CLASSNAME* instance){                               \
        if(s_blaSingletonInstance == nullptr)                                                              \
           { s_blaSingletonInstance = instance; } }                                                        \
   template<>  CLASSNAME* CLASSNAME::AssignAndReturnSingletonInstance(CLASSNAME* instance){                \
        if(s_blaSingletonInstance == nullptr)                                                              \
                   { s_blaSingletonInstance = instance; }                                                  \
        return s_blaSingletonInstance; } 

#define BLA_DECLARE_THREAD_LOCAL_SINGLETON(CLASSNAME)                                       \
public:                                                                                     \
    static BLA_THREAD CLASSNAME* s_blaSingletonInstance;                                    \
    static void AssignSingletonInstance(CLASSNAME* instance);                               \
    static CLASSNAME* AssignAndReturnSingletonInstance(CLASSNAME* instance);                \
    static CLASSNAME* GetSingletonInstance();                                               \
    static const CLASSNAME* GetSingletonInstanceRead();                                     \
private:

#define BLA_IMPLEMENT_THREAD_LOCAL_SINGLETON(CLASSNAME)                                     \
    BLA_THREAD CLASSNAME* CLASSNAME::s_blaSingletonInstance = nullptr;                      \
    CLASSNAME* CLASSNAME::GetSingletonInstance(){ return s_blaSingletonInstance; }          \
    const CLASSNAME* CLASSNAME::GetSingletonInstanceRead() { return s_blaSingletonInstance;}\
    void CLASSNAME::AssignSingletonInstance(CLASSNAME* instance){                           \
        if(s_blaSingletonInstance == nullptr)                                               \
           { s_blaSingletonInstance = instance; } }                                         \
    CLASSNAME* CLASSNAME::AssignAndReturnSingletonInstance(CLASSNAME* instance){            \
        if(s_blaSingletonInstance == nullptr)                                               \
                   { s_blaSingletonInstance = instance; }                                   \
        return s_blaSingletonInstance; } 


//template<class T>
//class Singleton
//{
//private:
//    static T* s_singletonInstance;
//
//public:
//    Singleton()
//    {
//        s_singletonInstance = nullptr;
//    }
//
//    static void AssignSingletonInstance(T* instance)
//    {
//            if (s_singletonInstance == nullptr)
//            {
//                s_singletonInstance = instance;
//            }
//    }
//    static T* GetSingletonInstance() { return s_singletonInstance; }
//    static const T* GetSingletonInstanceRead() { return s_singletonInstance; }
//};