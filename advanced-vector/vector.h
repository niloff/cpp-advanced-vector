#pragma once
#include <cassert>
#include <cstdlib>
#include <new>
#include <utility>
#include <memory>
#include <algorithm>
/**
 * Шаблонный класс, отвечающий за хранение буфера,
 * который вмещает заданное количество элементов, и предоставляет доступ к элементам по индексу
 */
template <typename T>
class RawMemory {
public:
    RawMemory() = default;
    explicit RawMemory(size_t capacity);
    RawMemory(const RawMemory&) = delete;
    RawMemory& operator=(const RawMemory& rhs) = delete;
    RawMemory(RawMemory&& other) noexcept;
    RawMemory& operator=(RawMemory&& rhs) noexcept;
    ~RawMemory();

    T* operator+(size_t offset) noexcept;
    const T* operator+(size_t offset) const noexcept;
    const T& operator[](size_t index) const noexcept;
    T& operator[](size_t index) noexcept;

    void Swap(RawMemory& other) noexcept;
    const T* GetAddress() const noexcept;
    T* GetAddress() noexcept;
    size_t Capacity() const;

private:
    /**
     * Выделяет сырую память под n элементов и возвращает указатель на неё
     */
    static T* Allocate(size_t n);
    /**
     * Освобождает сырую память, выделенную ранее по адресу buf при помощи Allocate
     */
    static void Deallocate(T* buf) noexcept;

    T* buffer_ = nullptr;
    size_t capacity_ = 0;
};
/**
 * Улучшенный контейнер вектор
 */
template <typename T>
class Vector {
public:
    using iterator = T*;
    using const_iterator = const T*;
    /**
     * Конструктор по умолчанию.
     * Инициализирует вектор нулевого размера и вместимости.
     */
    Vector() = default;
    /**
     * Конструктор, который создаёт вектор заданного размера.
     * Вместимость созданного вектора равна его размеру, а элементы проинициализированы значением по умолчанию
     */
    explicit Vector(size_t size);
    /**
     * Копирующий конструктор.
     * Создаёт копию элементов исходного вектора.
     * Имеет вместимость, равную размеру исходного вектора, то есть выделяет память без запаса.
     */
    Vector(const Vector& other);
    /**
     * Оператор копирующего присваивания
     */
    Vector& operator=(const Vector& rhs);
    /**
     * Перемещающий конструктор
     */
    Vector(Vector&& other) noexcept;
    /**
     * Оператор перемещающего присваивания
     */
    Vector& operator=(Vector&& rhs) noexcept;
    /**
     * Деструктор. Разрушает содержащиеся в векторе элементы и освобождает занимаемую ими память.
     */
    ~Vector();

    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator begin() const noexcept;
    const_iterator end() const noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;
    /**
     * Вставляет новый элемент в контейнер в указанную позицию.
     * Значение элемента задается путём прямой передачи параметров его конструктору.
     */
    template <typename... Args>
    iterator Emplace(const_iterator pos, Args&&... args);
    /**
     * Удаляет элемент, на который указывает переданный итератор
     */
    iterator Erase(const_iterator pos);
    /**
     * Вставляет элемент в заданную позицию вектора.
     * Возвращает итератор, указывающий на вставленный элемент в новом блоке памяти.
     */
    iterator Insert(const_iterator pos, const T& value);
    /**
     * Вставляет элемент в заданную позицию вектора.
     * Возвращает итератор, указывающий на вставленный элемент в новом блоке памяти.
     */
    iterator Insert(const_iterator pos, T&& value);
    /**
     * Возвращает число элементов в контейнере
     */
    size_t Size() const noexcept;
    /**
     * Возвращает количество элементов, для которых в данный момент в контейнере выделено пространство.
     */
    size_t Capacity() const noexcept;
    const T& operator[](size_t index) const noexcept;
    T& operator[](size_t index) noexcept;
    /**
     * Резервирует достаточно места, чтобы вместить количество элементов, равное capacity.
     * Если новая вместимость не превышает текущую, метод не делает ничего.
     */
    void Reserve(size_t capacity);
    /**
     * Обмен содержимого вектора с другим вектором
     */
    void Swap(Vector& other) noexcept;
    /**
     * Изменяет размер контейнера, чтобы содержать count элементов.
     * Если текущий размер меньше, чем count, дополнительные элементы добавляются и инициализируется value.
     * Если текущий размер больше, чем count, контейнер уменьшается до его первых элементов count.
     */
    void Resize(size_t count);
    /**
     * Добавляет заданное значение элемента в конец контейнера.
     */
    template <typename Type>
    void PushBack(Type&& value);
    /**
     * Удаляет последний элемент контейнера.
     */
    void PopBack();
    /**
     * Добавляет новый элемент в конец контейнера.
     * Значение элемента задается путём прямой передачи параметров его конструктору.
     */
    template <typename... Args>
    T& EmplaceBack(Args&&... args);
private:
    RawMemory<T> CreateCopy(size_t capacity);
    RawMemory<T> data_;
    size_t size_ = 0;
};

template<typename T>
inline RawMemory<T>::RawMemory(size_t capacity) :
    buffer_(Allocate(capacity)),
    capacity_(capacity) { }

template<typename T>
inline RawMemory<T>::RawMemory(RawMemory&& other) noexcept {
    buffer_ = other.buffer_;
    capacity_ = other.capacity_;
    other.buffer_ = nullptr;
    other.capacity_ = 0;
}

template<typename T>
inline RawMemory<T>& RawMemory<T>::operator=(RawMemory&& rhs) noexcept {
    if (this != &rhs) {
        buffer_.~RawMemory();
        capacity_ = 0;
        Swap(rhs);
    }
    return *this;
}

template<typename T>
inline RawMemory<T>::~RawMemory() {
    Deallocate(buffer_);
}

template<typename T>
inline T* RawMemory<T>::operator+(size_t offset) noexcept {
    assert(offset <= capacity_);
    return buffer_ + offset;
}

template<typename T>
inline const T* RawMemory<T>::operator+(size_t offset) const noexcept {
    return const_cast<RawMemory&>(*this) + offset;
}

template<typename T>
inline const T& RawMemory<T>::operator[](size_t index) const noexcept {
    return const_cast<RawMemory&>(*this)[index];
}

template<typename T>
inline T& RawMemory<T>::operator[](size_t index) noexcept {
    assert(index < capacity_);
    return buffer_[index];
}

template<typename T>
inline void RawMemory<T>::Swap(RawMemory& other) noexcept {
    std::swap(buffer_, other.buffer_);
    std::swap(capacity_, other.capacity_);
}

template<typename T>
inline const T* RawMemory<T>::GetAddress() const noexcept {
    return buffer_;
}

template<typename T>
inline T* RawMemory<T>::GetAddress() noexcept {
    return buffer_;
}

template<typename T>
inline size_t RawMemory<T>::Capacity() const {
    return capacity_;
}
/**
 * Выделяет сырую память под n элементов и возвращает указатель на неё
 */
template<typename T>
inline T* RawMemory<T>::Allocate(size_t n) {
    return n != 0 ? static_cast<T*>(operator new(n * sizeof(T))) : nullptr;
}
/**
 * Освобождает сырую память, выделенную ранее по адресу buf при помощи Allocate
 */
template<typename T>
inline void RawMemory<T>::Deallocate(T* buf) noexcept {
    operator delete(buf);
}
/**
 * Конструктор, который создаёт вектор заданного размера.
 * Вместимость созданного вектора равна его размеру, а элементы проинициализированы значением по умолчанию
 */
template<typename T>
inline Vector<T>::Vector(size_t size) :
    data_(size),
    size_(size) {
    std::uninitialized_value_construct_n(data_.GetAddress(), size);
}
/**
 * Копирующий конструктор.
 * Создаёт копию элементов исходного вектора.
 * Имеет вместимость, равную размеру исходного вектора, то есть выделяет память без запаса.
 */
template<typename T>
inline Vector<T>::Vector(const Vector& other):
    data_(other.size_),
    size_(other.size_) {
    std::uninitialized_copy_n(other.data_.GetAddress(), size_, data_.GetAddress());
}
/**
 * Оператор копирующего присваивания
 */
template<typename T>
inline Vector<T>& Vector<T>::operator=(const Vector& rhs) {
    if (this != &rhs) {
        if (rhs.size_ > data_.Capacity()) {
            Vector rhs_copy(rhs);
            Swap(rhs_copy);
        }
        else {
            if (rhs.size_ < size_) {
                std::copy(rhs.data_.GetAddress(), rhs.data_.GetAddress() + rhs.size_, data_.GetAddress());
                std::destroy_n(data_.GetAddress() + rhs.size_, size_ - rhs.size_);
            }
            else {
                std::copy(rhs.data_.GetAddress(), rhs.data_.GetAddress() + size_, data_.GetAddress());
                std::uninitialized_copy_n(rhs.data_.GetAddress() + size_, rhs.size_ - size_, data_.GetAddress() + size_);
            }
            size_ = rhs.size_;
        }
    }
    return *this;
}
/**
 * Перемещающий конструктор
 */
template<typename T>
inline Vector<T>::Vector(Vector&& other) noexcept {
    Swap(other);
}
/**
 * Оператор перемещающего присваивания
 */
template<typename T>
inline Vector<T>& Vector<T>::operator=(Vector&& rhs) noexcept {
    if (this != &rhs) {
        Swap(rhs);
    }
    return *this;
}
/**
 * Деструктор. Разрушает содержащиеся в векторе элементы и освобождает занимаемую ими память.
 */
template<typename T>
inline Vector<T>::~Vector() {
    std::destroy_n(data_.GetAddress(), size_);
}

template<typename T>
typename Vector<T>::iterator Vector<T>::begin() noexcept {
    return data_.GetAddress();
}

template<typename T>
typename Vector<T>::iterator Vector<T>::end() noexcept {
    return data_.GetAddress() + size_;
}

template<typename T>
typename Vector<T>::const_iterator Vector<T>::begin() const noexcept {
    return data_.GetAddress();
}

template<typename T>
typename Vector<T>::const_iterator Vector<T>::end() const noexcept {
    return data_.GetAddress() + size_;
}

template<typename T>
typename Vector<T>::const_iterator Vector<T>::cbegin() const noexcept {
    return begin();
}

template<typename T>
typename Vector<T>::const_iterator Vector<T>::cend() const noexcept {
    return end();
}
/**
 * Вставляет новый элемент в контейнер в указанную позицию.
 * Значение элемента задается путём прямой передачи параметров его конструктору.
 */
template<typename T>
template<typename ...Args>
typename Vector<T>::iterator Vector<T>::Emplace(const_iterator pos, Args && ...args) {
    assert(pos >= begin() && pos <= end());
    iterator it_element = nullptr;
    size_t shift = pos - begin();
    if (size_ == Capacity()) {
        RawMemory<T> data(size_ == 0 ? 1 : size_ * 2);
        it_element = new (data + shift) T(std::forward<Args>(args)...);
        if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
            std::uninitialized_move_n(begin(), shift, data.GetAddress());
            std::uninitialized_move_n(begin() + shift, size_ - shift, data.GetAddress() + shift + 1);
        }
        else {
            try {
                std::uninitialized_copy_n(begin(), shift, data.GetAddress());
                std::uninitialized_copy_n(begin() + shift, size_ - shift, data.GetAddress() + shift + 1);
            }
            catch (...) {
                std::destroy_n(data.GetAddress() + shift, 1);
                throw;
            }
        }
        std::destroy_n(begin(), size_);
        data_.Swap(data);
    }
    else {
        if (size_ != 0) {
            new (data_ + size_) T(std::move(*(end() - 1)));
            try {
                std::move_backward(begin() + shift, end(), end() + 1);
            }
            catch (...) {
                std::destroy_n(end(), 1);
                throw;
            }
            std::destroy_at(begin() + shift);
        }
        it_element = new (data_ + shift) T(std::forward<Args>(args)...);
    }
    ++size_;
    return it_element;
}
/**
 * Удаляет элемент, на который указывает переданный итератор
 */
template<typename T>
typename Vector<T>::iterator Vector<T>::Erase(const_iterator pos) {
    assert(pos >= begin() && pos < end());
    size_t shift = pos - begin();
    std::move(begin() + shift + 1, end(), begin() + shift);
    PopBack();
    return begin() + shift;
}
/**
 * Вставляет элемент в заданную позицию вектора.
 * Возвращает итератор, указывающий на вставленный элемент в новом блоке памяти.
 */
template<typename T>
typename Vector<T>::iterator Vector<T>::Insert(const_iterator pos, const T& value) {
    return Emplace(pos, value);
}
/**
 * Вставляет элемент в заданную позицию вектора.
 * Возвращает итератор, указывающий на вставленный элемент в новом блоке памяти.
 */
template<typename T>
typename Vector<T>::iterator Vector<T>::Insert(const_iterator pos, T&& value) {
    return Emplace(pos, std::move(value));
}
/**
 * Возвращает число элементов в контейнере
 */
template<typename T>
inline size_t Vector<T>::Size() const noexcept {
    return size_;
}
/**
 * Возвращает количество элементов, для которых в данный момент в контейнере выделено пространство.
 */
template<typename T>
inline size_t Vector<T>::Capacity() const noexcept {
    return data_.Capacity();
}

template<typename T>
inline const T& Vector<T>::operator[](size_t index) const noexcept {
    return const_cast<Vector&>(*this)[index];
}

template<typename T>
inline T& Vector<T>::operator[](size_t index) noexcept {
    assert(index < size_);
    return data_[index];
}
/**
 * Резервирует достаточно места, чтобы вместить количество элементов, равное capacity.
 * Если новая вместимость не превышает текущую, метод не делает ничего.
 */
template<typename T>
inline void Vector<T>::Reserve(size_t capacity) {
    if (capacity <= data_.Capacity()) {
        return;
    }
    RawMemory<T> data(CreateCopy(capacity));
    data_.Swap(data);
    std::destroy_n(data.GetAddress(), Size());
}
/**
 * Обмен содержимого вектора с другим вектором
 */
template<typename T>
inline void Vector<T>::Swap(Vector& other) noexcept {
    data_.Swap(other.data_);
    std::swap(size_, other.size_);
}
/**
 * Изменяет размер контейнера, чтобы содержать count элементов.
 * Если текущий размер меньше, чем count, дополнительные элементы добавляются и инициализируется value.
 * Если текущий размер больше, чем count, контейнер уменьшается до его первых элементов count.
 */
template<typename T>
inline void Vector<T>::Resize(size_t count) {
    if (count > size_) {
        Reserve(count);
        std::uninitialized_value_construct_n(data_.GetAddress() + size_, count - size_);
    }
    else {
        std::destroy_n(data_.GetAddress() + count, size_ - count);
    }
    size_ = count;
}
/**
 * Добавляет заданное значение элемента в конец контейнера.
 */
template<typename T>
template<typename Type>
inline void Vector<T>::PushBack(Type&& value) {
    EmplaceBack(std::forward<Type>(value));
}
/**
 * Удаляет последний элемент контейнера.
 */
template<typename T>
inline void Vector<T>::PopBack() {
    if (size_ > 0) {
        std::destroy_at(data_.GetAddress() + size_ - 1);
        --size_;
    }
}
/**
 * Добавляет новый элемент в конец контейнера.
 * Значение элемента задается путём прямой передачи параметров его конструктору.
 */
template<typename T>
template<typename ...Args>
inline T& Vector<T>::EmplaceBack(Args && ...args) {
    return *Emplace(end(), std::forward<Args>(args)...);
}

template<typename T>
RawMemory<T> Vector<T>::CreateCopy(size_t capacity) {
    RawMemory<T> data(capacity);
    if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
        std::uninitialized_move_n(data_.GetAddress(), Size(), data.GetAddress());
    }
    else{
        std::uninitialized_copy_n(data_.GetAddress(), Size(), data.GetAddress());
    }
    return data;
}
