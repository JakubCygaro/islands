#ifndef UTIL_HPP
#define UTIL_HPP

#define PROPERTY(TYPE, FIELD_NAME)                                       \
private:                                                                 \
    TYPE m_##FIELD_NAME;                                                 \
                                                                         \
public:                                                                  \
    inline TYPE get_##FIELD_NAME() const { return m_##FIELD_NAME; }      \
    inline void set_##FIELD_NAME(TYPE value) { m_##FIELD_NAME = value; } \
                                                                         \
private:

#define PROTECTED_PROPERTY(TYPE, FIELD_NAME)                             \
protected:                                                               \
    TYPE m_##FIELD_NAME;                                                 \
                                                                         \
public:                                                                  \
    inline TYPE get_##FIELD_NAME() const { return m_##FIELD_NAME; }      \
    inline void set_##FIELD_NAME(TYPE value) { m_##FIELD_NAME = value; } \
                                                                         \
private:

#define name_of(IDEN) #IDEN

#endif
