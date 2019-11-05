//
// Created by wushuohan on 19-5-21.
//

#ifndef MUDUO_FILEREADSTREAM_H
#define MUDUO_FILEREADSTREAM_H

#include <cstdio>
#include <assert.h>

namespace muduo {
    namespace FileUtil {


        class FileReadStream {
        public:
            typedef char Ch;    //!< Character type (byte).

            //! Constructor.
            /*!
                \param fp File pointer opened for read.
                \param buffer user-supplied buffer.
                \param bufferSize size of buffer in bytes. Must >=4 bytes.
            */
            FileReadStream(std::FILE *fp, char *buffer, size_t bufferSize) : fp_(fp), buffer_(buffer),
                                                                             bufferSize_(bufferSize), bufferLast_(0),
                                                                             current_(buffer_), readCount_(0),
                                                                             count_(0), eof_(false) {
                assert(fp_ != 0);
                assert(bufferSize >= 4);
                Read();
            }

            Ch Peek() const { return *current_; }

            Ch Take() {
                Ch c = *current_;
                Read();
                return c;
            }

            size_t Tell() const { return count_ + static_cast<size_t>(current_ - buffer_); }

            // Not implemented
            void Put(Ch) { assert(false); }

            void Flush() { assert(false); }

            Ch *PutBegin() {
                assert(false);
                return 0;
            }

            size_t PutEnd(Ch *) {
                assert(false);
                return 0;
            }

            // For encoding detection only.
            const Ch *Peek4() const {
                return (current_ + 4 <= bufferLast_) ? current_ : 0;
            }

        private:
            void Read() {
                if (current_ < bufferLast_)
                    ++current_;
                else if (!eof_) {
                    count_ += readCount_;
                    readCount_ = std::fread(buffer_, 1, bufferSize_, fp_);
                    bufferLast_ = buffer_ + readCount_ - 1;
                    current_ = buffer_;

                    if (readCount_ < bufferSize_) {
                        buffer_[readCount_] = '\0';
                        ++bufferLast_;
                        eof_ = true;
                    }
                }
            }

            std::FILE *fp_;
            Ch *buffer_;
            size_t bufferSize_;
            Ch *bufferLast_;
            Ch *current_;
            size_t readCount_;
            size_t count_;  //!< Number of characters read
            bool eof_;
        };
    }
}


#endif //MUDUO_FILEREADSTREAM_H
