#ifndef STRING_HPP
#define STRING_HPP

#ifdef __cplusplus

class string {
   public:
    explicit string();
    explicit string(int len);
    string(const char* buffer);

    void set(const char* buffer);
    void clear();

    const char* c_str();
    const char* c_copy();

    char* rbuf;

    int length;

   private:
};

#endif

#endif
