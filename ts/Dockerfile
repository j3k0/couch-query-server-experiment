FROM apache/couchdb:3.2.1

ARG NODE_VERSION=v16.14.2

RUN \
  apt-get update \
  && apt-get install -y \
    nodejs \
    npm \
  && rm -fr /var/lib/apt/lists/* /var/lib/dpkg/info/*

RUN \
  curl https://nodejs.org/dist/${NODE_VERSION}/node-${NODE_VERSION}-linux-arm64.tar.gz -o /node.tar.gz \
  && cd / \
  && tar xzf node.tar.gz \
  && rm -fr node.tar.gz \
  && mv node-* /opt/node \
  && mkdir /iaptic

COPY package.json package-lock.json /iaptic/
RUN cd /iaptic/ && npm install
COPY index.ts /iaptic/
RUN cd /iaptic/ && npm run build

COPY extracouch.c /iaptic/extracouch.c
RUN cd /iaptic && gcc -o extracouch -Wall extracouch.c

ENV COUCHDB_QUERY_SERVER_IAPTIC="/opt/node/bin/node /iaptic/build/index.js"
ENV COUCHDB_QUERY_SERVER_EXTRACOUCH="/iaptic/extracouch no-args"
