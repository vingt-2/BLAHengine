#define BLA_DECLARE_SINGLETON(CLASSNAME)                                                 \
public:                                                                                  \
    static CLASSNAME* s_blaSingletonInstance;                                            \
    static void AssignSingletonInstance(CLASSNAME* instance){                            \
        if(s_blaSingletonInstance == nullptr)                                            \
           { s_blaSingletonInstance = instance; } }                                      \
    static CLASSNAME* AssignAndReturnSingletonInstance(CLASSNAME* instance){             \
        if(s_blaSingletonInstance == nullptr)                                            \
                   { s_blaSingletonInstance = instance; }                                \
        return s_blaSingletonInstance; }                                                 \
    static CLASSNAME* GetSingletonInstance() { return s_blaSingletonInstance;}           \
    static const CLASSNAME* GetSingletonInstanceRead() { return s_blaSingletonInstance;}

#define BLA_IMPLEMENT_SINGLETON(CLASSNAME) \
    CLASSNAME* CLASSNAME::s_blaSingletonInstance = nullptr;

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