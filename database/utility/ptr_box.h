#ifndef PTR_BOX_H_GUARD
#define PTR_BOX_H_GUARD

#include <utility>

namespace data_struct
{
    template <typename Type>
    class PtrBox {
    protected:
        PtrBox() noexcept = default;
        PtrBox (PtrBox const&) noexcept = default;
        PtrBox& operator= (PtrBox const&) noexcept = default;
        ~PtrBox() noexcept = default;

        PtrBox (PtrBox&& rhs) noexcept
            : ptr (std::exchange (rhs.ptr, nullptr))
        {}

        PtrBox& operator= (PtrBox&& rhs) noexcept {
            ptr = std::exchange (rhs.ptr, nullptr);
            return *this;
        }

        PtrBox (Type* ptr_) noexcept
            : ptr (ptr_)
        {}

    protected:
        Type* ptr = nullptr;
    };
}

#endif