FROM alpine as builder

WORKDIR /build

RUN \
apk --no-cache add build-base linux-headers boost-dev && \
apk add --no-cache --repository=https://dl-cdn.alpinelinux.org/alpine/v3.16/main/ libexecinfo-dev

ADD . .

RUN cd src && make -j16

FROM alpine

WORKDIR /app

RUN apk --no-cache add boost && \
apk add --no-cache --repository=https://dl-cdn.alpinelinux.org/alpine/v3.16/main/ libexecinfo

COPY --from=builder /build/src/proxy_server /app/proxy_server

CMD echo "$PROXY_CONF" > proxy.conf && /app/proxy_server
