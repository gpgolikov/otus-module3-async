FROM ubuntu:xenial

RUN apt update && apt install -y curl

RUN curl -sSL "https://build.travis-ci.org/files/gpg/llvm-toolchain-xenial-7.asc" | apt-key add -
RUN echo "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-7 main" | tee -a /etc/apt/sources.list > /dev/null

RUN curl -sSL "https://bintray.com/user/downloadSubjectPublicKey?username=bintray" | apt-key add -
RUN echo "deb http://dl.bintray.com/gpgolikov/otus-cpp xenial main" | tee -a /etc/apt/sources.list.d/otus.list > /dev/null

RUN apt update && apt install -y libc++1-7

CMD ["/bin/bash", "-l"]
