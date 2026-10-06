#pragma once

namespace jp {
    template <typename _Type>
    void __veccopy(_Type*& src, const _Type* dst, const int size) {
        src = new _Type[size];
        for (int i = 0; i < size; i++) {
            src[i] = dst[i];
        }
    }

    template <typename _Type>
    void __vecsub(_Type*& src, const _Type* dst, const int buffer, const int pos = 0) {
        if (pos < 0) {
            return;
        }
        src = new _Type[buffer];
        for (int i = pos; i < pos + buffer; i++) {
            src[i - pos] = dst[i];
        }
    }

    template <class _Type>
    class Reader {
        private: _Type* data;
        private: int size = 0;
        private: mutable int bytesKeep = 0;
        private: mutable int streampos = 0;

        public: Reader(const _Type* data, const int size) {
            __veccopy(this->data, data, size);
            this->size = size;
            this->bytesKeep = size;
        }

        public: Reader(const Reader& c) {
            __veccopy(data, c.data, c.size);
            size = c.size;
            bytesKeep = c.bytesKeep;
            streampos = c.streampos;
        }

        public: int getStreamPos() const {
            return streampos;
        }

        public: ~Reader() {
            delete[] data;
        }

        public: int getStreamSize() const {
            return size;
        }

        public: int getKeep() const {
            return bytesKeep;
        }

        public: int read(_Type*& buffer, const int bufferSize) const {
            int nextpos = streampos + bufferSize;
            int realBuffer = bufferSize;
            int pos = streampos;
            if (!isEnd()) {
                if (nextpos > size) {
                    realBuffer -= nextpos - size;
                    streampos = nextpos = size;
                }
                bytesKeep -= realBuffer;
                __vecsub(buffer, data, realBuffer, pos);
            }
            else {
                bytesKeep = 0;
                return 0;
            }
            return realBuffer;
        }

        public: bool isEnd() const {
            return streampos == size;
        }

        public: bool goTo(const int pos) const {
            if (pos < 0 || pos > size) {
                return false;
            }
            streampos = pos;
            bytesKeep = size - streampos;
            return true;
        } 
    };
}