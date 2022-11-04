#ifndef KM2_BACKEND_AS_BASE_H
#define KM2_BACKEND_AS_BASE_H

namespace km2 {
namespace backend {

#define km2_backend_decl_as \
public: \
    template<typename T> \
    inline const T* as() const { return dynamic_cast<T*>(this); } \
    template<typename T> \
    inline T* as() { return dynamic_cast<T*>(this); } \
private:

}
}

#endif // KM2_BACKEND_AS_BASE_H
