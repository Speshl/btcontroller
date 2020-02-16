FROM linuxserver/letsencrypt
COPY build /btcontroller
COPY build /config/www