FROM ubuntu:artful
MAINTAINER perl@immunant.com

ARG USER=docker
ARG UID=1000
ARG GID=1000
ENV HOME=/home/$USER
ENV DEBIAN_FRONTEND=noninteractive

USER root
RUN groupadd -f -g $GID $USER
RUN useradd --create-home -u $UID -g $GID --create-home --shell /bin/bash $USER 
WORKDIR $HOME

# Provision debian packages (dirmngr is required for gnupg2 key retrieval)
RUN apt-get update -qq && \
    apt-get install -y --install-recommends dirmngr  && \
    apt-get install -y autoconf \
                       automake \
                       clang-5.0 \
                       cmake \
                       curl \
                       git \
                       gnupg2 \
                       gperf \
                       htop \
                       ipython3 \
                       libreadline-dev \
                       make \
                       ninja-build \
                       python3 \
                       python3-pip \
                       tmux \
                       unzip \
                       vim \
                       wget

RUN update-alternatives --install /usr/bin/clang clang /usr/bin/clang-5.0 100 && \
    update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-5.0 100 

# Provision Python3 packages (using --no-cache-dir to avoid warnings)
ARG PY_REQUIREMENTS="cbor\ncolorlog\nmako\nplumbum\npsutil\ntyping"
RUN echo "$PY_REQUIREMENTS" > /tmp/requirements.txt
RUN pip3 install --no-cache-dir -r /tmp/requirements.txt

USER $USER
# Provision rust language and packages (should not run as root)
ARG RUST_VER=nightly-2018-06-20
RUN curl https://sh.rustup.rs -sSf | sh -s -- -y --default-toolchain $RUST_VER
RUN echo "source ~/.cargo/env" >> ~/.bashrc
# Required for idiomize tests
# RUN . $HOME/.cargo/env && \
#    rustup run $RUST_VER cargo install --force rustfmt
RUN . $HOME/.cargo/env && \
   rustup component add rustfmt-preview
